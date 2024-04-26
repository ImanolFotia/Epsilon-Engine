using System.Collections.Generic;
using System.Data;

namespace Game
{
    public static class ObjectManager
    {
        static List<GameObject> m_pObjects;

        static ObjectManager()
        {
            m_pObjects = new List<GameObject>();
        }

        public static void AddObject(GameObject gameObject)
        {
            Console.WriteLine("Object added to Manager");
            m_pObjects.Add(gameObject);
        }

        public static void Update()
        {
        }

        public static void RemoveObject(GameObject gameObject)
        {
            Console.WriteLine("Object deleted from Manager");
            m_pObjects.Remove(gameObject);
        }
    }
}