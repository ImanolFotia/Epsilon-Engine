using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace common
{
    public static class Utils
    {

        public static Dictionary<string, Dictionary<string, object>> ToPropertyDictionary(this Type type, object obj)
        {
            var dictionary = new Dictionary<string, Dictionary<string, object>>();
            foreach (var propertyInfo in type.GetRuntimeProperties())
            {
                if (propertyInfo.CanRead && propertyInfo.GetIndexParameters().Length == 0) {
                    dictionary[propertyInfo.Name] = new Dictionary<string, object>();
                    dictionary[propertyInfo.Name]["Value"] = propertyInfo.GetValue(obj, null);
                    var attributes = new Dictionary<string, Dictionary<string, object>>();
                    foreach(var attribute in propertyInfo.GetCustomAttributesData())
                    {
                        attributes[attribute.AttributeType.Name] = new Dictionary<string, object>();
                        foreach (var argument in attribute.NamedArguments)
                        {
                            attributes[attribute.AttributeType.Name][argument.MemberName] = argument.TypedValue.Value;
                        }
                    }
                    dictionary[propertyInfo.Name]["Attributes"] = attributes;
                }
            }
            return dictionary;
        }

        public static Type? SearchTypeInAssembly(Assembly assembly, string objType)
        {
            Type? foundType = null;
                foreach (var type in assembly.GetExportedTypes())
                {
                    if (type.FullName == objType)
                    {
                        foundType = type;
                    }
                }
            

            return foundType;
        }
    }
}
