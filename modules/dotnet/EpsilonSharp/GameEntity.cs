using common;
using System;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
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

        private static CallBackDelegate callback;
        private static UpdateReferenceCallbackDelegate UpdateReferenceCallback;
        public static setTransformCallBackDelegate transformCallback;
        static AssemblyLoader domainMgr;

        static bool isInstance = false;

        static BridgeDictionary m_pBridgeDictionary;

        static FileSystemWatcher watcher;
        static WeakReference weakReference;

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

            string pluginPath = Directory.GetCurrentDirectory() + "\\assets\\scripts\\Game\\bin\\x64\\Release\\net8.0\\Game.dll";

            watcher = new FileSystemWatcher(Directory.GetCurrentDirectory() + "\\assets\\scripts\\Game\\bin\\x64\\Release\\net8.0");
            watcher.NotifyFilter = NotifyFilters.Attributes
                                 | NotifyFilters.CreationTime
                                 | NotifyFilters.DirectoryName
                                 | NotifyFilters.FileName
                                 | NotifyFilters.LastAccess
                                 | NotifyFilters.LastWrite
                                 | NotifyFilters.Security
                                 | NotifyFilters.Size;

            //watcher.Changed += OnChanged;
            watcher.Filter = "";
            watcher.IncludeSubdirectories = true;
            watcher.EnableRaisingEvents = true;

            domainMgr = new AssemblyLoader(pluginPath, AssemblyLoadContext.Default, true);
            weakReference = new WeakReference(domainMgr, trackResurrection: true);
            m_pBridgeDictionary = new BridgeDictionary();

            var assembly = domainMgr.LoadFromAssemblyName(AssemblyName.GetAssemblyName(pluginPath));
#if DEBUG
            //if (!Debugger.IsAttached)
                //Debugger.Launch();
#endif

            Console.WriteLine(Directory.GetCurrentDirectory());
            m_pAssemblies.Add(assembly);
        }

        private static void OnChanged(object sender, FileSystemEventArgs e)
        {
            if (e.ChangeType != WatcherChangeTypes.Changed)
            {
                return;
            }
            ReloadAssemblies();
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

                dynamic obj = RuntimeHelpers.GetUninitializedObject(objectType);

                _ = ctor!.Invoke(obj, null);

                m_pBridgeDictionary.Insert(unmanagedObject, obj);

                string jsonObj = JsonSerializer.Serialize(objectType.ToPropertyDictionary((object)obj));

                obj.setCallback(callbackPtr);
                obj.NativePtr = unmanagedObject;
                obj.setPropertiesStr(jsonObj);
                obj.Name = nodeName;

                NodeManager.GetInstance().AddNode(obj, nodeName);
                obj._NodeManagerPtr = GCHandle.ToIntPtr(GCHandle.Alloc(NodeManager.GetInstance(), GCHandleType.Normal));

                obj.ManagedPtr = GCHandle.ToIntPtr(GCHandle.Alloc(obj, GCHandleType.Normal));
                return obj.ManagedPtr;
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
                dynamic ent = GCHandle.FromIntPtr(obj).Target;
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
            List<BridgeType> oldObjectList = new List<BridgeType>(); ;
            foreach(var obj in m_pBridgeDictionary.ToDictionary())
            {
                dynamic managedObject = obj.Value;

                dynamic t = managedObject.GetType().GetProperty("transform")!.PropertyType.GetConstructors()[0].Invoke(null);
                t.Position.x = managedObject.transform.Position.x;
                t.Position.y = managedObject.transform.Position.y;
                t.Position.z = managedObject.transform.Position.z;

                t.Scale.x = managedObject.transform.Scale.x;
                t.Scale.y = managedObject.transform.Scale.y;
                t.Scale.z = managedObject.transform.Scale.z;


                t.Rotation.i = managedObject.transform.Rotation.i;
                t.Rotation.j = managedObject.transform.Rotation.j;
                t.Rotation.k = managedObject.transform.Rotation.k;
                t.Rotation.w = managedObject.transform.Rotation.w;

                oldObjectList.Add(item: new BridgeType { name = managedObject.Name, type = managedObject.GetType(), UnmanagedPtr = obj.Key, transform = t } );
                GCHandle.FromIntPtr(managedObject._NodeManagerPtr).Free();
                GCHandle.FromIntPtr(managedObject.ManagedPtr).Free();
            }
            m_pAssemblies.Clear();

            NodeManager.GetInstance().Clear();
            m_pBridgeDictionary.Clear();
            m_pBridgeDictionary = new BridgeDictionary();

            domainMgr.Unload();
           // while(weakReference.IsAlive)
            {
                GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced);
                GC.WaitForPendingFinalizers();

            }


            string pluginPath = Directory.GetCurrentDirectory() + "/assets/scripts/Game/bin/x64/Release/net8.0/Game.dll";

            domainMgr = new AssemblyLoader(pluginPath, AssemblyLoadContext.Default, true);
            weakReference = new WeakReference(domainMgr, trackResurrection: true);

            var assembly = domainMgr.LoadFromAssemblyName(AssemblyName.GetAssemblyName(pluginPath));
            m_pAssemblies.Add(assembly);


            foreach (var oldObject in oldObjectList)
            {


                Type objectType = null;
                    objectType = Utils.SearchTypeInAssembly(assembly, oldObject.type.FullName);

                var ctor = objectType.GetConstructor(
                        BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance,
                        null, Type.EmptyTypes, null) ?? throw new Exception("ctor is null");

                dynamic obj = RuntimeHelpers.GetUninitializedObject(objectType);

                _ = ctor!.Invoke(obj, null); 
               // m_pBridgeDictionary.
                m_pBridgeDictionary.Insert(oldObject.UnmanagedPtr, obj);

                string jsonObj = JsonSerializer.Serialize(objectType.ToPropertyDictionary((object)obj));

                obj.setCallback(callbackPtr);
                obj.NativePtr = oldObject.UnmanagedPtr;
                obj.setPropertiesStr(jsonObj);
                obj.Name = oldObject.name;


                dynamic t = objectType.GetProperty("transform")!.PropertyType.GetConstructors()[0].Invoke(null);
                t.Position.x = oldObject.transform.Position.x;
                t.Position.y = oldObject.transform.Position.y;
                t.Position.z = oldObject.transform.Position.z;

                t.Scale.x = oldObject.transform.Scale.x;
                t.Scale.y = oldObject.transform.Scale.y;
                t.Scale.z = oldObject.transform.Scale.z;

                t.Rotation.i = oldObject.transform.Rotation.i;
                t.Rotation.j = oldObject.transform.Rotation.j;
                t.Rotation.k = oldObject.transform.Rotation.k;
                t.Rotation.w = oldObject.transform.Rotation.w;

                obj.transform = t;

                NodeManager.GetInstance().AddNode(obj, oldObject.name);
                obj._NodeManagerPtr = GCHandle.ToIntPtr(GCHandle.Alloc(NodeManager.GetInstance(), GCHandleType.Normal));
                obj.ManagedPtr = GCHandle.ToIntPtr(GCHandle.Alloc(obj, GCHandleType.Normal));
                UpdateReferenceCallback(SceneHandle, oldObject.UnmanagedPtr, obj.ManagedPtr);
            }

            //List<Assembly> tmpAssemblies = new List<Assembly>();


            Console.WriteLine(Directory.GetCurrentDirectory());
            foreach (var type in assembly.GetTypes())
            {
                Console.WriteLine($"{type.FullName}");
            }
        }


        public static void registerUpdateReferenceCallback(IntPtr func_ptr)
        {
            UpdateReferenceCallback = (UpdateReferenceCallbackDelegate)
            Marshal.GetDelegateForFunctionPointer(func_ptr, typeof(UpdateReferenceCallbackDelegate));
        }

        public static string getEntityFields(IntPtr obj)
        {
            try
            {
                dynamic ent = GCHandle.FromIntPtr(obj).Target;
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
