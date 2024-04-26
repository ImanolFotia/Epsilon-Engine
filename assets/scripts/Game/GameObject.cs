using EpsilonSharp;
using common.Attributes;
using System.ComponentModel.DataAnnotations;
using RangeAttribute = common.Attributes.RangeAttribute;
using Types;
using System.Reflection;
using System.Runtime.InteropServices;
using Editor;
using Math;
using System;

namespace Game
{

    public class GameObject : GameEntity
    {
        public GameObject() { 
        
            Random random = new Random();
            
            Direction = new Vec3(((float)random.Next(-100, 100)) * 0.01f, ((float)random.Next(-100, 100)) * 0.01f, (random.Next(-100, 100)) * 0.01f);
            Direction = Direction.Normalize();
            //ObjectManager.AddObject(this);
        }

        ~GameObject()
        {
            //ObjectManager.RemoveObject(this);
        }
        public string Name { get; set; } = "My Grass";

        public bool Moves { get; set; } = true;

        [Range(Min = 0.1f, Max = 5.0f)]
        public float Radius { get; set; } = 1.0f;

        [Range(Min = 0, Max = 5)]
        public float Size { get; set; } = 1.0f;

        private float count = 0.0f;

        private Vec3 Direction;
        private Vec3 BoxSize = new Vec3(3.0f);


        public override void OnUpdate(float delta)
        {
            if (Moves)  
            {
                Ground groundNode = GetNode<Ground>("Ground");
                BoxSize.x = groundNode.X * 0.5f;
                BoxSize.y = groundNode.Height;
                BoxSize.z = groundNode.Z * 0.5f;

                CheckCollision();

                transform.Position = transform.Position + (Direction * 10.0f * delta);
            }

        }

        private void CheckCollision()
        {
            if (transform.Position.x >= BoxSize.x)
            {
                Direction = Editor.Math.Reflect(Direction, new Vec3(-1.0f, 0.0f, 0.0f));
            }

            else if (transform.Position.x <= -BoxSize.x)
            {
                Direction = Editor.Math.Reflect(Direction, new Vec3(1.0f, 0.0f, 0.0f));
            }

            if (transform.Position.y >= 10.0f)
            {
                Direction = Editor.Math.Reflect(Direction, new Vec3(0.0f, -1.0f, 0.0f));
            }

            else if(transform.Position.y <= BoxSize.y)
            {
                Direction = Editor.Math.Reflect(Direction, new Vec3(0.0f, 1.0f, 0.0f));
            }

            if (transform.Position.z >= BoxSize.z)
            {
                Direction = Editor.Math.Reflect(Direction, new Vec3(0.0f, 0.0f, -1.0f));
            }

            else if(transform.Position.z <= -BoxSize.z)
            {
                Direction = Editor.Math.Reflect(Direction, new Vec3(0.0f, 0.0f, 1.0f));
            }


        }
    }
}