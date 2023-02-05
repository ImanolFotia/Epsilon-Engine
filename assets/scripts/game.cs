
using System;
using System.Runtime.CompilerServices;


namespace Epsilon
{

    public class GameClass
    {

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern void printFromCpp();

        public void print()
        {
            printFromCpp();
        }


    }
}
