using System;
using System.Runtime.InteropServices;

using Epsilon;
using Epsilon.Types;

namespace MainProgram
{
    public static class Program
    {

        public static void Main(string[] args)
        {
            Imports.EpsilonInit("Hello from P/Invoke!");

            Imports.CreateWindow("Hello from P/Invoke!", 640, 480);

            Imports.CreateContext("Hello from P/Invoke!", renderer_type.vulkan);

            Imports.EpsilonRun();
        }
    }
}

