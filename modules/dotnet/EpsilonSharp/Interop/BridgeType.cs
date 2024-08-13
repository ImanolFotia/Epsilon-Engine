using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Types;

namespace EpsilonSharp
{
    public class BridgeType
    {
        public Type type { get; set; }
        public string name { get; set; }
        public Dictionary<string, object> properties { get; set; }

        public IntPtr UnmanagedPtr { get; set; } = IntPtr.Zero;

        public dynamic transform { get; set; }
    }
}
