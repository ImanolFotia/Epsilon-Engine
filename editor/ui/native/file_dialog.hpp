#pragma once

#ifdef _WIN32
#include "win32/win_funcs.hpp"
#endif

namespace Editor::UI {
	class NativeFileDialog {
		std::string m_pDirectory{};
		std::vector<std::string> m_pSupportedExtensions{};
        std::vector<std::string> m_pLastOpenPath{};
	public:
		void Setup(const std::string& directory, std::initializer_list<std::string> extensions = {}) {
			m_pDirectory = directory;
		}

        void Show() {
#ifdef _WIN32
            IFileDialog* pfd;
            if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
            {
                if (SUCCEEDED(pfd->Show(NULL)))
                {
                    IShellItem* psi;
                    if (SUCCEEDED(pfd->GetResult(&psi)))
                    {
                        PWSTR pszPath;
                        if (SUCCEEDED(GetIDListName(psi, &pszPath, &m_pLastOpenPath)))
                        {
                            char result[500]; 
                            wcstombs(result, pszPath, 500);
                            MessageBox(NULL, result, "Selected Item", MB_OK);
                            CoTaskMemFree(pszPath);
                        }
                        psi->Release();
                    }
                }
                pfd->Release();

                if(m_pLastOpenPath.size() > 0)
                std::cout << m_pLastOpenPath.back() << std::endl;
            }

#endif
		}
	};
}