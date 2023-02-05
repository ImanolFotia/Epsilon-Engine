
using System;
using System.Runtime.CompilerServices;

namespace Epsilon {


    public interface IEpsilon
    {
        void onReady();
        void onCreate();
        void onRender();
        void onExit();
    }

    public abstract class EpsilonBehaviour : IEpsilon {


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CreateWindow(string name, int w, int h);
        public bool ShouldClose {get;set;} = false;
        public int Width {get;set;} = 1280;
        public int Height {get;set;} = 720;

        public virtual void onReady(){}
        public virtual void onCreate(){}
        public virtual void onRender(){}
        public virtual void onExit(){}

        
    }

    public class Test : EpsilonBehaviour {

        public Test() {
            CreateWindow("Setting name from .NET!", 1920, 1080);
        }

        public override void onReady() {
            System.Console.WriteLine("Class is ready!");}

        public override void onCreate() {
            System.Console.WriteLine("Creating test class");
        }

        public override void onRender() {

            GameClass myClass = new GameClass();

            myClass.print();
        }

        public override void onExit() {}
    }
}