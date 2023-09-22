using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Types;

namespace Math
{
    public static class Extensions
    {
        public static float Dot(this Vec4 a, Vec4 b) => (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
        public static float Dot(Vec3 a, Vec3 b) => (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

        public static float Distance(this Vec3 a, Vec3 b) => (float)System.Math.Sqrt(((a.x * b.x) + (a.y * b.y) + (a.z * b.z)));



        public static string ToString2(this string str)
        {
            return str.ToString() + "fgfgg";
        }
    }
}
