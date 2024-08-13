using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static EpsilonSharp.Epsilon;
using Types;
using System.Xml.Linq;
using System.Runtime.Remoting;
using System.Runtime.Loader;
using System.Reflection;

namespace EpsilonSharp
{


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public abstract class GameEntity : Node
    {

        ~GameEntity()
        {
            Console.WriteLine($"{Name} Entity destroyed");
        }
        public GameEntity(Transform transform)
        {
            this.transform = transform;
        }
        public GameEntity()
        {
            transform = new Transform();
        }

        public virtual void OnReady() { }
        public virtual void OnCreate() { }
        public virtual void OnDestroy() { }
        public virtual void OnRender() { }
        public virtual void OnUpdate(float delta) { }

        public Transform getTransform() { return transform; }

        public override string ToString()
        {
            return m_pFieldsStr;
        }

        public void setPropertiesStr(string fields)
        {
            m_pFieldsStr = fields;
        }

        public void setTransform(Transform t)
        {
            transform = t;
            if (transformCallback != null)
                transformCallback(NativePtr, CppEntity, t);
        }

        public void setCallback(IntPtr c)
        {
            transformCallback = (setTransformCallBackDelegate)
            Marshal.GetDelegateForFunctionPointer(c, typeof(setTransformCallBackDelegate));
        }

        virtual public void setT(Transform t)
        {
            transform = t;
        }
        public setTransformCallBackDelegate transformCallback;
        public Transform transform { get; set; }
        public int CppEntity;
        [MarshalAs(UnmanagedType.BStr)]
        public string m_pFieldsStr;
        public IntPtr _NodeManagerPtr;
        public IntPtr ManagedPtr;
        public string Name { get; set; }

        public T GetNode<T>(string name)
        {
            dynamic nodeManagerPtr = GCHandle.FromIntPtr(_NodeManagerPtr).Target;
            return nodeManagerPtr.GetNode<T>(name);
        }

    }


}
