using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Types
{


    public static class ext
    {
        public static float Dot(this Vec4 a, Vec4 b) => (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
        public static float Dot(Vec3 a, Vec3 b) => (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

        public static string ToString2(this string str)
        {
            return str.ToString() + "fgfgg";
        }
    }



    public class Vec4
    {
        public Vec4(float a)
        {
            x = a;
            y = a;
            z = a;
            w = a;
        }
        public float x, y, z, w;

        public float Length() => (float)Math.Sqrt(((x * x) + (y * y) + (z * z) + (w * w)));
        //public float Dot(Vec4 other) => (other.x * x) + (other.y * y) + (other.z * z) + (other.w * w);
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Vec3
    {

        public Vec3(float a)
        {
            x = a;
            y = a;
            z = a;

            var t = ext.Dot(new Vec4(1.0f), new Vec4(2.0f));
            Vec4 aa = new Vec4(1.0f);
            aa.Dot(new Vec4(2.0f));
        }
        public Vec3(float a, float b, float c)
        {
            x = a;
            y = b;
            z = c;
        }
        public float x;
        public float y;
        public float z;
    };
    [StructLayout(LayoutKind.Sequential)]
    public class Quat
    {
        public float i;
        public float j;
        public float k;
        public float w;
    };

    [StructLayout(LayoutKind.Sequential)]
    public class Transform
    {
        public Transform() { }
        public Vec3 Position = new Vec3(0.0f);
        public Quat Rotation;
        public Vec3 Scale = new Vec3(1.0f);
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



}
