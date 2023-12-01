using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Math
{

    [StructLayout(LayoutKind.Sequential)]
    public class Vec2
    {
        public static Vec2 operator +(Vec2 a, Vec2 b)
        {
            return new Vec2(b.x + a.x, b.y + a.y);
        }

        public static Vec2 operator -(Vec2 a, Vec2 b)
        {
            return new Vec2(b.x - a.x, b.y - a.y);
        }

        public static Vec2 operator -(Vec2 a, float b)
        {
            return new Vec2(b - a.x, b - a.y);
        }

        public static Vec2 operator -(float a, Vec2 b)
        {
            return new Vec2(b.x - a, b.y - a);
        }

        public static Vec2 operator -(Vec2 v)
        {
            return new Vec2(-v.x,  -v.y);
        }


        public static Vec2 operator *(Vec2 a, Vec2 b)
        {
            return new Vec2(b.x * a.x, b.y * a.y);
        }

        public static Vec2 operator *(Vec2 a, float b)
        {
            return new Vec2(b * a.x, b * a.y);
        }

        public static Vec2 operator *(float a, Vec2 b)
        {
            return new Vec2(b.x * a, b.y * a);
        }



        public static Vec2 operator /(Vec2 a, Vec2 b)
        {
            return new Vec2(b.x / a.x, b.y / a.y);
        }

        public static Vec2 operator /(Vec2 a, float b)
        {
            return new Vec2(a.x /b , a.x / b);
        }

        public static Vec2 operator /(float a, Vec2 b)
        {
            return new Vec2(b.x / a, b.y / a);
        }
        public Vec2(float a)
        {
            x = a;
            y = a;

        }
        public Vec2(float a, float b)
        {
            x = a;
            y = b;
        }
        public float x;
        public float y;
    };

}
