using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Editor.Epsilon;
using Types;

namespace Editor
{


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public abstract class GameEntity {
    
        ~GameEntity()
        {
            Console.WriteLine("Entity destroyed");
        }
        public GameEntity(Transform transform)
        {
            transform = transform;
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
                transformCallback(InternalPtr, CppEntity, t);
        }

        public void setSceneHandle(IntPtr t)
        {
            scene_handle = t;
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
        public IntPtr scene_handle;

        public IntPtr InternalPtr;
    }

    public class EpsilonObject
    {
        public virtual void OnUpdate() { }
        public EpsilonObject() {
            transform = new Transform();
        }

        public IntPtr? InternalPtr;
        public Transform? transform { get; set; }
        
    }
}
