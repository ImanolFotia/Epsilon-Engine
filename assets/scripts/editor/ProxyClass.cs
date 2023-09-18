using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Editor
{


    class ProxyClass : MarshalByRefObject
    {
        //you may specified any parameter you want, if you get `xxx is not marked as serializable` error, see explanation below
        public string ExecuteAssembly(string param1)
        {
            /*
             * All the code executed here is under the new app domain that we just created above
             * We also have different session state here, so if you want data from main domain's session, you should pass it as a parameter
             */
            //load your DLL file here
            Debug.WriteLine(AppDomain.CurrentDomain.FriendlyName);
            //will print "new:50536e71-51ad-4bad-9bf8-67c54382bb46" which is the name that we just gave to the new created app domain

            Assembly asm = Assembly.LoadFrom(@"PATH/TO/THE/DLL");

            Type baseClass = asm.GetType("MyAssembly.MyClass");
            MethodInfo targetMethod = baseClass.GetMethod("MyMethod");

            string result = (string)targetMethod.Invoke(null, new object[] { });

            return result;
        }
    }
}