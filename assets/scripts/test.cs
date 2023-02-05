using System;
using System.Runtime.CompilerServices;

using Epsilon;

public class MonoEmbed {

    /*[MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern static string gimme();*/
    

    static int Main()
    {
        Epsilon.Test test = new Epsilon.Test();
        
        test.onCreate();

        test.onReady();

        //while(!test.ShouldClose)
        //    test.onRender();

        //test.onExit();
        /*
        System.Console.WriteLine(gimme());

        Epsilon.GameClass myClass = new Epsilon.GameClass();

        myClass.print();

        if (gimme().Equals("All your monos are belong to us!"))
            return 0;*/
        return 0;
    }
}
