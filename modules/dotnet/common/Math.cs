using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Types;
using Math;

namespace Editor
{
    static public class Math
    {

        public static float Distance(Vec3 a, Vec3 b)
        {
            return (float)System.Math.Sqrt(System.Math.Abs(Dot(a, b)));
        }
        public static float Length(Vec3 a, Vec3 b)
        {
            return (float)System.Math.Sqrt(System.Math.Abs(Dot(a, b)));
        }
        public static float Length(this Vec3 a)
        {
            return (float)System.Math.Sqrt(System.Math.Abs(Dot(a, a)));
        }

        public static Vec3 Reflect(Vec3 d, Vec3 n) => -2.0f * Dot(d, n) * n + d;

        public static Vec3 Normalize(this Vec3 a)
        {
            float l = System.Math.Abs(a.Length());
            return a / l;
        }

        public static float Dot(this Vec4 a, Vec4 b) => (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
        public static float Dot(Vec3 a, Vec3 b) => (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }
}
