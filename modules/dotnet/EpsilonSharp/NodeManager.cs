using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EpsilonSharp
{
    public class NodeManager
    {
        static Dictionary<string, Dictionary<string, object>> m_pNodeRegistry;
        //public int prop { get; set; }
        public static NodeManager instance { get; set; } = null;

        public static NodeManager GetInstance()
        {
            if (instance == null)
            {
                instance = new NodeManager();
            }
            return instance;
        }
        static NodeManager()
        {
            Console.WriteLine("Created node manager");
            if (m_pNodeRegistry == null)
            {
                m_pNodeRegistry = new Dictionary<string, Dictionary<string, object>>();

            }
        }
        public T GetNode<T>(string name)
        {

            if (!m_pNodeRegistry.ContainsKey(typeof(T).FullName))
            {
                return default(T);
            }

            if (!m_pNodeRegistry[typeof(T).FullName].ContainsKey(name))
            {
                return default(T);
            }

            return (T)m_pNodeRegistry[typeof(T).FullName][name];
        }

        public void AddNode<T>(T node, string name)
        {
            if (m_pNodeRegistry == null)
            {
                m_pNodeRegistry = new Dictionary<string, Dictionary<string, object>>();

            }
            if (!m_pNodeRegistry.ContainsKey(typeof(T).FullName))
            {
                m_pNodeRegistry[typeof(T).FullName] = new Dictionary<string, object>();
            }

            m_pNodeRegistry[typeof(T).FullName][name] = node;
        }
    }
}
