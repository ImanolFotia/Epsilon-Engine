using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Math
{
    [StructLayout(LayoutKind.Sequential)]
    public class Quat
    {

        public static Quat operator +(Quat a, Quat b)
        {
            return new Quat(b.i + a.i, b.j + a.j, b.k + a.k, b.w + a.w);
        }

        public static Quat operator -(Quat a, Quat b)
        {
            return new Quat(b.i - a.i, b.j - a.j, b.k - a.k, b.w - a.w);
        }
        public Quat(float a)
        {
            i = a;
            j = a;
            k = a;
            w = a;

        }
        public Quat(float a, float b, float c, float d)
        {
            i = a;
            j = b;
            k = c;
            w = d;
        }
        public float i;
        public float j;
        public float k;
        public float w;
    };
}
