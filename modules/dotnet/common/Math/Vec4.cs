using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Math
{

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

        public float Length() => (float)System.Math.Sqrt(((x * x) + (y * y) + (z * z) + (w * w)));
    }

}
