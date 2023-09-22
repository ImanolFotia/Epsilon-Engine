using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Types;

namespace EpsilonSharp
{
    public class Caller
    {
        [UnsafeAccessor(UnsafeAccessorKind.StaticMethod, Name = "ToString")]
        public static extern string GetFields(dynamic entity);
    }
}
