using System;
using System.Runtime.InteropServices;

namespace Epsilon {
        
    public static class Imports {
        [DllImport("libEpsilon.so")]
        public extern static void EpsilonInit([MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("libEpsilon.so")]
        public extern static void CreateWindow([MarshalAs(UnmanagedType.LPStr)] string name, int w, int h);
        
        [DllImport("libEpsilon.so")]
        public extern static void CreateContext([MarshalAs(UnmanagedType.LPStr)] string name, int api);
        
        [DllImport("libEpsilon.so")]
        public extern static void EpsilonRun();
    }
}