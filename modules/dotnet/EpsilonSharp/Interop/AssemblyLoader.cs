
using System.Reflection;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

public class AssemblyLoader : AssemblyLoadContext
{

    static WeakReference weakReference;
    public AssemblyLoader(string pluginPath, bool isCollectible = true) : base (isCollectible)
    {
        _resolver = new AssemblyDependencyResolver(pluginPath);

    }

    public AssemblyLoader(string pluginPath,
            AssemblyLoadContext mainLoadContext, bool isCollectible) : base(isCollectible)
    {

        _resolver = new AssemblyDependencyResolver(pluginPath);
        _mainLoadContext = mainLoadContext;
    }

    private readonly AssemblyDependencyResolver _resolver;
    private readonly ICollection<string> _sharedAssemblies;
    private readonly AssemblyLoadContext _mainLoadContext;

    protected override Assembly? Load(AssemblyName assemblyName)
    {
        string assemblyPath = _resolver.ResolveAssemblyToPath(assemblyName);
        if (assemblyPath != null)
        {
            using var dllFile = File.Open(assemblyPath, FileMode.Open, FileAccess.Read, FileShare.Read);
            string pdbPath = Path.ChangeExtension(assemblyPath, ".pdb");

            if (File.Exists(pdbPath))
            {
                using var pdbFile = File.Open(pdbPath, FileMode.Open, FileAccess.Read, FileShare.Read);
                return LoadFromStream(dllFile, pdbFile);
            }

            return LoadFromStream(dllFile);
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
