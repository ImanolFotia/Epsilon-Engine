#pragma once

namespace os {

	enum class FileNotification: std::uint32_t {
		CREATION = 1,
		DELETION = 2,
		CHANGE = 4,
		RENAME = 8,
		OPEN = 16
	};

	class DirectoryWatchBase {
		virtual void addFileWatch(std::string filename, FileNotification notification) = 0;
	};
}