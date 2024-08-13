using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace common
{
    namespace Attributes
    {
        [System.AttributeUsage(System.AttributeTargets.Field |
                               System.AttributeTargets.Property,
                                AllowMultiple = true)]
        public class RangeAttribute : System.Attribute
        {
            public float Min;
            public float Max;
            public RangeAttribute() { }

        }
    }
}
