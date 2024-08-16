#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <memory>
#include "zip.hpp"
#include <core/framework/log.hpp>

namespace engine {

	struct Filesystem {

		static std::unordered_map<std::string, Zip > m_Container;

		static bool Mount(const std::string& name) {
			struct stat s;
			if (stat(name.c_str(), &s) == 0) {
				if (s.st_mode & S_IFDIR) {
					//it's a directory
				}
				else if (s.st_mode & S_IFREG) {
					m_Container.insert({ name, Zip(name)});
					Log::Info(name, ": Virtual File System Mounted");
				}
				else {
					//something else
					return false;
				}
			}
			else {
				//error
				return false;
			}
			return true;
		}

		static bool Unmount(std::string name) {
			std::unordered_map<std::string, Zip >::iterator it;
			it = m_Container.find(name);
			m_Container.erase(it);
			return true;
		}

		template <typename T>
		static std::shared_ptr<cifstream> open(std::string name) {

			std::shared_ptr<cifstream> outFile;

			for (auto& container : m_Container) {
				outFile = container.second.getFileByName(name);
				if (outFile->m_Exists) {
					outFile->ContainerName = container.first;
					break;
				}
			}

			if (outFile->m_Exists == false) {
				Log::Error(name, ": No such file or directory");
				return nullptr;
			}

			outFile->m_isopen = true;

			return outFile;
		}

		static bool is_present(const std::string& name) {

			std::shared_ptr<cifstream> outFile;

			for (auto& container : m_Container) {
				outFile = container.second.getFileByName(name);
				if (outFile == nullptr) continue;

				if (outFile->m_Exists) {
					return true;
				}
			}

			return false;

		}

		template <typename T>
		static bool close(T file) {
			file.close();
		}

	};

}