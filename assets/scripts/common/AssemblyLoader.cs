
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

public class DomainManager : AssemblyLoadContext
{
    private AssemblyDependencyResolver _resolver;
    public DomainManager(string pluginPath)
    {
        _resolver = new AssemblyDependencyResolver(pluginPath);
    }

    protected override Assembly? Load(AssemblyName assemblyName)
    {
        string assemblyPath = _resolver.ResolveAssemblyToPath(assemblyName);
        if (assemblyPath != null)
        {
            using var dllFile = File.Open(assemblyPath, FileMode.Open, FileAccess.Read, FileShare.Read);
            return LoadFromStream(dllFile);
            //return LoadFromAssemblyPath(dllFile);
        }

        return null;
    }

    protected override IntPtr LoadUnmanagedDll(string unmanagedDllName) 
    {
        string libraryPath = _resolver.ResolveUnmanagedDllToPath(unmanagedDllName);
        if (libraryPath != null)
        {
            return LoadUnmanagedDllFromPath(libraryPath);
        }

        return IntPtr.Zero;
    }

}
