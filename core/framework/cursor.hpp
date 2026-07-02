#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>

static std::string GetSystemCursorTheme() {
#ifdef __linux__
    const char* env_theme = std::getenv("XCURSOR_THEME");
    if (env_theme && std::string(env_theme) != "") {
        return env_theme;
    }

    std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
    std::string config_path = home + "/.config/gtk-3.0/settings.ini";

    if (std::filesystem::exists(config_path)) {
        std::ifstream file(config_path);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("gtk-cursor-theme-name") != std::string::npos) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string theme = line.substr(pos + 1);
                    // Trim whitespace
                    theme.erase(0, theme.find_first_not_of(" \t\r\n"));
                    theme.erase(theme.find_last_not_of(" \t\r\n") + 1);
                    return theme;
                }
            }
        }
    }
#endif

    return "default";
}
