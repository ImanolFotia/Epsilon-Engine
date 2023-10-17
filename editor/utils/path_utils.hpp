#pragma once

#include <string>

namespace Editor {
	static inline std::string getExtension(const std::string& path) {
		size_t dot_pos = path.find_last_of('.');
		if (dot_pos == std::string::npos) return "";

		return path.substr(dot_pos + 1, path.length());
	}
}