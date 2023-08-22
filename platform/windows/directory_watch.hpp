#pragma once

#include "../directory_watch.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef min
#undef max
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>


#include <string>
#include <iostream>
#include <vector>
#include <functional>

namespace os {
	class DirectoryWatch : public DirectoryWatchBase {

		struct WatchedDirectory {
			std::function<void(void)> callback{};
			std::string path{};
			size_t handle_index = 0;

		};

		std::vector<WatchedDirectory> m_pDirectories{};
		std::vector<HANDLE> m_pChangeHandles{};
	public:
		DirectoryWatch() {

		}

		void addFileWatch(std::string filename, FileNotification notification, std::function<void(void)>&& callback) override {

			DWORD dwWaitStatus;
			HANDLE dwChangeHandle;
			TCHAR lpDrive[4];
			TCHAR lpFile[_MAX_FNAME];
			TCHAR lpExt[_MAX_EXT];

			_tsplitpath_s(filename.c_str(), lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

			lpDrive[2] = (TCHAR)'\\';
			lpDrive[3] = (TCHAR)'\0';

			dwChangeHandle = FindFirstChangeNotification(
				filename.c_str(),                         // directory to watch 
				FALSE,                         // do not watch subtree 
				FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 

			if (dwChangeHandle == INVALID_HANDLE_VALUE)
			{
				std::cerr << "\n ERROR: FindFirstChangeNotification function failed.\n";
				return;
			}

			m_pChangeHandles.push_back(dwChangeHandle);
			m_pDirectories.emplace_back(callback, filename, m_pChangeHandles.size() - 1);
		}

		void Notify() {
			dwWaitStatus = WaitForMultipleObjects(m_pChangeHandles.size(), m_pChangeHandles.data(), FALSE, 0);

			if (dwWaitStatus == WAIT_TIMEOUT || dwWaitStatus == WAIT_FAILED) {
				std::cerr << "Error getting the status of the watched files...\nCode: " << dwWaitStatus << std::endl;
				return;
			}

			if (dwWaitStatus < m_pDirectories.size()) {
				m_pDirectories.at(dwWaitStatus).callback();

				if (FindNextChangeNotification(m_pChangeHandles.at(dwWaitStatus)]) == FALSE)
				{
					std::cerr << "\n ERROR: FindNextChangeNotification function failed.\n";
					return;
				}
			}

		}
	};
}

#endif