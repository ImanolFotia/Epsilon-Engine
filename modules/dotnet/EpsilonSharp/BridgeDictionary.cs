using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EpsilonSharp
{
    public class BridgeDictionary
    {
        Dictionary<IntPtr, object> PtrToObject;
        Dictionary<object, IntPtr> ObjectToPtr;

        public BridgeDictionary() {
            PtrToObject = new Dictionary<IntPtr, object>();
            ObjectToPtr = new Dictionary<object, IntPtr>();
        }

        public void Insert(IntPtr Ptr, object Obj)
        {
            PtrToObject[Ptr] = Obj;
            ObjectToPtr[Obj] = Ptr;
        }

        public void Remove(IntPtr Ptr)
        {
            object obj = PtrToObject[Ptr];
            PtrToObject.Remove(Ptr);
            ObjectToPtr.Remove(obj);
        }

        public void Clear()
        {
            ObjectToPtr.Clear();
            PtrToObject.Clear();
            ObjectToPtr = null;
            PtrToObject = null;
        }

        public bool Contains(object Obj)
        {
            return ObjectToPtr.ContainsKey(Obj);
        }

        public bool Contains(IntPtr Ptr)
        {
            return PtrToObject.ContainsKey(Ptr);
        }

        public object GetManaged(IntPtr Ptr)
        {
            return PtrToObject[Ptr];
        }

        public IntPtr GetUnmanaged(object Obj)
        {
            return ObjectToPtr[Obj];
        }
    }
}
