#include <sys/filesystem.h>
#include <fstream>
#include <Log.h>
#include <cstring>

namespace IO {
    namespace Filesystem {

        /** Theorical definition of a file
            including a hash in case it is inside a zip file
            and the name of the file itself
            **/

        uint32_t File::generateHash(std::string filename) {
            return 0;
        }

        /**Definition of the two types of file systems**/
        /**First is the definition of the filesystem contained within an uncompresed zip file*/
        ZipFileSystem::ZipFileSystem(std::string) {

        }
        ZipFileSystem::~ZipFileSystem() {

        }

        File ZipFileSystem::FindFile(const std::string) {
            return File();
        }

        /**Second is the real filesystem, i.e. not contained within a zip file, a hard drive*/
        PhysicalFileSystem::PhysicalFileSystem(std::string) {

        }

        PhysicalFileSystem::~PhysicalFileSystem() {

        }

        File PhysicalFileSystem::FindFile(const std::string filename) {
            File fndFile;
            if(!std::ifstream(filename)) {
                Global::Log::WriteToLog("the requested file: " + filename + " doesn't exist.");
                fndFile.name = "";
                return fndFile;
            }

            return fndFile;
        }

        /**Now is the main class that controls the interaction between the virtual filesystem and the user*/
        const std::vector<std::string> Filesystem::getFileNamesInDirectory(std::string path, std::string ext = "*") {
            std::vector<std::string> names;
#ifdef _WIN32File
            std::string search_path = path + "/*." + ext;
            WIN32_FIND_DATA fd;
            HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
            if(hFind != INVALID_HANDLE_VALUE) {
                do {
                    if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                        names.push_back(fd.cFileName);
                    }
                } while(::FindNextFile(hFind, &fd));
                ::FindClose(hFind);
            }
#endif // _WIN32
#ifdef __linux__
           /* int len = strlen(path.c_str());
            DIR dirp, dp;
            dirp = opendir(".");
            while ((dp = readdir(dirp)) != NULL) {
                if (dp->d_namlen == len && !strcmp(dp->d_name, path)) {
                    (void)closedir(dirp);
                    names.push_back(dp->d_name);
                }
            }
            (void)closedir(dirp);*/
#endif // __linux__
            return names;
        }

        bool Filesystem::FileExists(std::string path) {
            return false;
        }

        bool Filesystem::Mount(std::string path, std::string identifier) {
            if(path != "" && identifier != "") {
                m_MountedSystems[identifier] = path;
                return true;
            }
            return false;
        }

        bool Filesystem::FileSystemIsZip(File file) {
            if(file.type == 1) {
                return true;
            }
            return false;
        }
    }
}
