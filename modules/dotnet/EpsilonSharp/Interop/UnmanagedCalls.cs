using EpsilonSharp.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Types;

namespace EpsilonSharp
{
    public static partial class Epsilon
    {
        public delegate void setTransformCallBackDelegate(IntPtr scene_hnd, int entity_id, Transform transform);

        public delegate void setKeyboardStateDelegate(Keyboard keyboard);

        public delegate void registerCallbackDelegate(IntPtr func_ptr);

        public delegate void registerSetTransformDelegate(IntPtr func_ptr);

        public delegate int InstantiateEntityDelegate(EntityArgs args);

        public delegate void OnUpdateDelegate();

        public delegate Transform getTransformDelegate(int index);

        public delegate StringArray getStringDelegate();

        public delegate void setScenePtrDelegate(IntPtr ptr);

        public delegate void setEntityTransformDelegate(IntPtr obj, EntityArgs args);


        public delegate string getEntityFieldsDelegate(IntPtr obj);

        public delegate void removeEntityDelegate(int index);

        public delegate void CallBackDelegate(IntPtr scene_hnd, int entity_id, Transform transform);

        public delegate void registerUpdateReferenceCallbackDelegate(IntPtr func_ptr);
        public delegate void UpdateReferenceCallbackDelegate(IntPtr scene, IntPtr unmanaged, IntPtr managed);

        public delegate IntPtr CreateEntityDelegate(IntPtr unmanagedObject, IntPtr o_name);

        public delegate void SetPropertyDelegate(IntPtr obj, IntPtr prop_name, float value);

        public delegate void SetPropertyBoolDelegate(IntPtr obj, IntPtr prop_name, bool value);

        public delegate void ReloadAssembliesDelegate();

    }
}
