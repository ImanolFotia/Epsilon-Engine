#pragma once

#ifdef _WIN32
#include "win32/win_funcs.hpp"
#endif
#include <sys/wait.h>

#include <core/framework/string_utils.hpp>

namespace Editor::UI {
class NativeFileDialog {
  std::string m_pDirectory{};
  std::vector<std::string> m_pSupportedExtensions{};
  std::vector<std::string> m_pLastOpenPath{};

  std::string m_pLastSelectedFile{};

public:
  void Setup(const std::string &directory, std::initializer_list<std::string> extensions = {}) { m_pDirectory = directory; }

  std::vector<std::string> GetLastPath() { return m_pLastOpenPath; }

  const std::string &getLastSelectedFile() { return m_pLastSelectedFile; }

  void Show() {
#ifdef _WIN32
    IFileDialog *pfd;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
      if (SUCCEEDED(pfd->Show(NULL))) {
        IShellItem *psi;
        if (SUCCEEDED(pfd->GetResult(&psi))) {
          PWSTR pszPath;
          if (SUCCEEDED(GetIDListName(psi, &pszPath, &m_pLastOpenPath))) {
            char result[500];
            wcstombs(result, pszPath, 500);
            // MessageBox(NULL, result, "Selected Item", MB_OK);
            CoTaskMemFree(pszPath);
          }
          psi->Release();
        }
      }
      pfd->Release();

      if (m_pLastOpenPath.size() > 0)
        std::cout << m_pLastOpenPath.back() << std::endl;
    }
#else

    int link[2];
    int pid;

    if (pipe(link) == -1)
      return;
    if (pid = fork(); pid == -1)
      return;

    if (pid == 0) {
      dup2(link[1], STDOUT_FILENO);
      close(link[0]);
      close(link[1]);
      execl("/usr/bin/zenity", "zenity", "--file-selection", "--multiple", "--title", "Open File/s", m_pDirectory.c_str(), nullptr);
    } else {
      int status;
      close(link[1]);
      int w = waitpid(pid, &status, 0);

      if (w >= 0) {
        if (WIFEXITED(status)) {
          char path[4096];
          ssize_t nbytes = read(link[0], path, sizeof(path));
          std::string result = std::string(path, nbytes);

          m_pLastOpenPath = framework::StringUtils::split(result, "|");

          for (auto &p : m_pLastOpenPath) {
            std::erase_if(p, [](const char c) {return c == '\n' || c == '\r'; });
            std::println("{}", p);
          }
        }
      }
    }
#endif
  }
};
} // namespace Editor::UI