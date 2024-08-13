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
		virtual void addFileWatch(const std::string& filename, FileNotification notification, std::function<void(void)> callback) = 0;

		virtual void removeFileWatch(const std::string& filename) = 0;

		virtual void Notify() = 0;
	};
}