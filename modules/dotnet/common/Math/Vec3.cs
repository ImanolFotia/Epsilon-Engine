﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Math
{

    [StructLayout(LayoutKind.Sequential)]
    public class Vec3
    {
        public static Vec3 operator +(Vec3 a, Vec3 b)
        {
            return new Vec3(b.x + a.x, b.y + a.y, b.z + a.z);
        }

        public static Vec3 operator -(Vec3 a, Vec3 b)
        {
            return new Vec3(b.x - a.x, b.y - a.y, b.z - a.z);
        }

        public static Vec3 operator -(Vec3 a, float b)
        {
            return new Vec3(b - a.x, b - a.y, b - a.z);
        }

        public static Vec3 operator -(float a, Vec3 b)
        {
            return new Vec3(b.x - a, b.y - a, b.z - a);
        }

        public static Vec3 operator -(Vec3 v)
        {
            return new Vec3(-v.x,  -v.y, -v.z);
        }


        public static Vec3 operator *(Vec3 a, Vec3 b)
        {
            return new Vec3(b.x * a.x, b.y * a.y, b.z * a.z);
        }

        public static Vec3 operator *(Vec3 a, float b)
        {
            return new Vec3(b * a.x, b * a.y, b * a.z);
        }

        public static Vec3 operator *(float a, Vec3 b)
        {
            return new Vec3(b.x * a, b.y * a, b.z * a);
        }



        public static Vec3 operator /(Vec3 a, Vec3 b)
        {
            return new Vec3(b.x / a.x, b.y / a.y, b.z / a.z);
        }

        public static Vec3 operator /(Vec3 a, float b)
        {
            return new Vec3(a.x /b , a.x / b, a.z / b);
        }

        public static Vec3 operator /(float a, Vec3 b)
        {
            return new Vec3(b.x / a, b.y / a, b.z / a);
        }
        public Vec3(float a)
        {
            x = a;
            y = a;
            z = a;

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

}
