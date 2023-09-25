using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Types
{

    [StructLayout(LayoutKind.Sequential)]
    public class Transform 
    {
        public Transform() { }
        public Math.Vec3 Position = new Math.Vec3(0.0f);
        public Math.Quat Rotation = new Math.Quat(0.0f);
        public Math.Vec3 Scale = new Math.Vec3(1.0f);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct EntityArgs
    {
        public IntPtr objectType;
        public Transform transform;
        public int CppEntity;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct StringArray
    {
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_LPSTR)]
        public string[] elements;
    };


    [StructLayout(LayoutKind.Sequential)]
    public abstract class Node
    {
        [System.Text.Json.Serialization.JsonIgnore]
        public IntPtr NativePtr = IntPtr.Zero;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public class RenderNode : Node
    {
        public bool HasAnimation { get; set; }

        public bool IsVisible { get; set; }

        public bool IsHidden { get; set; } = false;

        public bool IsLoaded { get; set; }

        public bool IsInstanced { get; set; }

        public string Name { get; set; }
    }
}
