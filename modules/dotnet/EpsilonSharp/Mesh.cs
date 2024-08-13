using Math;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EpsilonSharp
{
    public class Mesh
    {
        public Mesh() { }

        public Vertex[] Vertices
        {
            get;set;
        }

        public int[] Indices { get; set; }
    }
}
