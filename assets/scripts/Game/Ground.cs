using EpsilonSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Game
{
    public class Ground : GameEntity
    {

        public float Height { get; set; } = 0.0f;

        public float X { get; set; } = 15.0f;
        public float Z { get; set; } = 15.0f;


        public override void OnUpdate(float delta) {
            transform.Scale.x = X; transform.Scale.y = Z;
            transform.Position.y = Height;
        }
    }
}
