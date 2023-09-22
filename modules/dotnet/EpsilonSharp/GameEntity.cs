using common;
using System;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using System.Runtime.Serialization;
using System.Text.Json;
using System.Windows.Input;
using System.Xml.Linq;
using Types;

namespace EpsilonSharp
{


    public static partial class Epsilon
    {
        static private IntPtr SceneHandle;

        static List<Assembly> m_pAssemblies = new List<Assembly>();

        private static CallBackDelegate? callback;
        public static setTransformCallBackDelegate? transformCallback;
        static DomainManager domainMgr;

        static bool isInstance = false;

        static BridgeDictionary m_pBridgeDictionary;


        static Epsilon()
        {
            if(isInstance == true)
            {
                throw new Exception("Double Epsilon initialization!");
            }

            var guid = Guid.NewGuid();

            Console.WriteLine(guid);

            isInstance = true;

            Console.WriteLine("statically instantiating Epsilon");

            string pluginPath = Directory.GetCurrentDirectory() + "/assets/scripts/Game/bin/x64/Debug/net8.0/Game.dll";

            domainMgr = new DomainManager(pluginPath);

            m_pBridgeDictionary  =new BridgeDictionary();

            var assembly = domainMgr.LoadFromAssemblyName(AssemblyName.GetAssemblyName(pluginPath));
#if DEBUG
            if (!Debugger.IsAttached)
                Debugger.Launch();
#endif

            Console.WriteLine(Directory.GetCurrentDirectory());
            m_pAssemblies.Add(assembly);
        }

        public static void registerCallback(IntPtr func_ptr)
        {
            callback = (CallBackDelegate)
            Marshal.GetDelegateForFunctionPointer(func_ptr, typeof(CallBackDelegate));
        }

        public static void registerSetTransform(IntPtr func_ptr)
        {
            callbackPtr = func_ptr;
            transformCallback = (setTransformCallBackDelegate)
            Marshal.GetDelegateForFunctionPointer(func_ptr, typeof(setTransformCallBackDelegate));
        }

        static IntPtr callbackPtr;

        [UnmanagedCallersOnly]
        public unsafe static IntPtr CreateEntity(IntPtr unmanagedObject, IntPtr o_name, IntPtr node_name)
        {
            try
            {
                string objType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ?
                Marshal.PtrToStringUni(o_name) :
                Marshal.PtrToStringUTF8(o_name);

                string nodeName = Marshal.PtrToStringAnsi(node_name);

                Type objectType = null;
                foreach (var assembly in m_pAssemblies)
                {
                    objectType = Utils.SearchTypeInAssembly(assembly, objType);
                    if (objectType != null) break;
                }

                if (objectType == null)
                {
                    throw new Exception("Object type is null");

                }

                var ctor = objectType.GetConstructor(
                        BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance,
                        null, Type.EmptyTypes, null) ?? throw new Exception("ctor is null");

                dynamic obj = FormatterServices.GetUninitializedObject(objectType);

                _ = ctor!.Invoke(obj, null);

                m_pBridgeDictionary.Insert(unmanagedObject, obj);

                string jsonObj = JsonSerializer.Serialize(objectType.ToPropertyDictionary((object)obj));

                obj.setCallback(callbackPtr);
                obj.NativePtr = unmanagedObject;
                obj.setPropertiesStr(jsonObj);
                obj.name = nodeName;

                NodeManager.GetInstance().AddNode(obj, nodeName);
                obj._NodeManagerPtr = GCHandle.ToIntPtr(GCHandle.Alloc(NodeManager.GetInstance(), GCHandleType.Normal));


                return GCHandle.ToIntPtr(GCHandle.Alloc(obj, GCHandleType.Normal));
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }

            return IntPtr.Zero;

        }

        [UnmanagedCallersOnly]
        public static void UpdateEntity(IntPtr obj, float delta)
        {
            try
            {
                dynamic ent = GCHandle.FromIntPtr(obj).Target;

                ent!.OnUpdate(delta);
                ent!.setTransform(ent.transform);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public static StringArray getString()
        {
            StringArray strArray = new StringArray();

            strArray.elements = new string[] { "Hello string!", "Hello string 2!" };
            return strArray;
        }

        public static void setScenePtr(IntPtr ptr)
        {
            SceneHandle = ptr;
        }

        [UnmanagedCallersOnly]
        public static void SetProperty(IntPtr obj, IntPtr prop_name, float value)
        {
            try
            {
                string objType = Marshal.PtrToStringAnsi(prop_name);

                object entity = GCHandle.FromIntPtr(obj).Target;

                if (entity == null)
                {
                    throw new Exception("Entity is null");
                }

                Type entType = entity!.GetType();

                var propertyType = entType.GetProperty(objType);

                if (propertyType.PropertyType == typeof(float))
                {
                    entType.GetProperty(objType).SetValue(entity, value);
                    return;
                }

                throw new Exception("Value was not float");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }



        [UnmanagedCallersOnly]
        public static void SetPropertyBool(IntPtr obj, IntPtr prop_name, int value)
        {
            try
            {
                string objType = Marshal.PtrToStringAnsi(prop_name);

                object entity = GCHandle.FromIntPtr(obj).Target;

                if (entity == null)
                {
                    throw new Exception("Entity is null");
                }

                Type entType = entity!.GetType();

                var propertyType = entType.GetProperty(objType);

                if (propertyType.PropertyType == typeof(bool))
                {
                    entType.GetProperty(objType).SetValue(entity, value == 0 ? false : true);
                    return;
                }

                throw new Exception("Value was not bool");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public static void setEntityTransform(IntPtr obj, EntityArgs args)
        {
            try
            {
                dynamic? ent = GCHandle.FromIntPtr(obj).Target;
                Type entType = ent!.GetType();
                dynamic t = entType!.GetProperty("transform")!.PropertyType.GetConstructors()[0].Invoke(null);
                t.Position.x = args.transform.Position.x;
                t.Position.y = args.transform.Position.y;
                t.Position.z = args.transform.Position.z;

                t.Scale.x = args.transform.Scale.x;
                t.Scale.y = args.transform.Scale.y;
                t.Scale.z = args.transform.Scale.z;


                t.Rotation.i = args.transform.Rotation.i;
                t.Rotation.j = args.transform.Rotation.j;
                t.Rotation.k = args.transform.Rotation.k;
                t.Rotation.w = args.transform.Rotation.w;

                entType!.GetProperty("transform")!.SetValue(ent, t);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public static void ReloadAssemblies()
        {
            domainMgr.Unload();

            List<Assembly> tmpAssemblies = new List<Assembly>();

            string pluginPath = Directory.GetCurrentDirectory() + "/assets/scripts/Game/bin/x64/Debug/net8.0/Game.dll";
            m_pAssemblies.Clear();
            var assembly = domainMgr.LoadFromAssemblyName(AssemblyName.GetAssemblyName(pluginPath));

            Console.WriteLine(Directory.GetCurrentDirectory());
            m_pAssemblies.Add(assembly);
            foreach (var type in assembly.GetTypes())
            {
                Console.WriteLine($"{type.FullName}");
            }
        }

        public static string getEntityFields(IntPtr obj)
        {
            try
            {
                dynamic? ent = GCHandle.FromIntPtr(obj).Target;
                //string fieldsStr = ent!.GetType().GetMethod("ToString").Invoke(ent, null);
                string fieldsStr = ent.ToString();//Caller.GetFields(ent);
                return fieldsStr.Length == 0 ? "{}" : fieldsStr;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return "{}";
        }

        static void addEntity(GameEntity entity)
        {
            //sEntities.Add(entity);
        }

        static void removeEntity(int index)
        {
            //((dynamic)sEntities.ElementAt(index)).OnDestroy();
            // sEntities.RemoveAt(index);
        }



    }
}
