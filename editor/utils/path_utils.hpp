#pragma once

#include <string>

namespace Editor {
static inline std::string getExtension(const std::string &path) {
  size_t dot_pos = path.find_last_of('.');
  if (dot_pos == std::string::npos)
    return "";

  auto result = path.substr(dot_pos + 1, path.length());
  std::erase_if(result, [](char c) { return c == '\n' || c == '\r'; });
  return result;
}
} // namespace Editor