using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace EpsilonSharp.Input
{

    public enum Key
    {
        ZERO = 48,
        ONE = 49,
        TWO = 50,
        THREE = 51,
        FOUR = 52,
        FIVE = 53,
        SIX = 54,
        SEVEN = 55,
        EIGHT = 56,
        NINE = 57,

        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        M = 76,
        N = 77,
        O = 78,
        P = 79,
        Q = 80,
        R = 81,
        S = 82,
        T = 83,
        U = 84,
        V = 85,
        W = 86,
        X = 87,
        Y = 88,
        Z = 89,

        ARROW_RIGHT = 262,
        ARROW_LEFT = 263,
        ARROW_UP = 265,
        ARROW_DOWN = 264,

        ADD = 334,
        SUBTRACT = 333,
        MULTIPLY = 332,
        DIVIDE = 331,

        ESCAPE = 256,
        SPACE = 32,
        LEFT_SHIFT = 340,

        GRAVE_ACCENT = 96
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Keyboard
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1024)]
        public int[] KEYS;

        public int getKey(Key key)
        {
            return KEYS[(int)key];
        }
    }
}
