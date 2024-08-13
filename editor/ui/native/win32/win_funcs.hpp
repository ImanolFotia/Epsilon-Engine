#pragma once

#ifdef _WIN32

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "ShellHelpers.h"
#include <strsafe.h>
#include <new>

const DWORD c_idAdd = 601;
const DWORD c_idDone = 602;

#define IDC_PICKITEM              100
#define IDC_PICKCONTAINER         101
#define IDC_FILEOPENBASKETPICKER  102
#define IDC_PICKFILESANDFOLDERS   103

/* Utility Classes and Functions *************************************************************************************************/

/*
    Usage:

    CItemIterator itemIterator(psi);

    while (itemIterator.MoveNext())
    {
        IShellItem2 *psi;
        hr = itemIterator.GetCurrent(IID_PPV_ARGS(&psi));
        if (SUCCEEDED(hr))
        {
            // Perform action on psi
            psi->Release();
        }
    }
*/
class CItemIterator
{
public:
    CItemIterator(IShellItem* psi) : _hr(SHGetIDListFromObject(psi, &_pidlFull)), _psfCur(NULL)
    {
        _Init();
    }

    CItemIterator(PCIDLIST_ABSOLUTE pidl) : _hr(SHILCloneFull(pidl, &_pidlFull)), _psfCur(NULL)
    {
        _Init();
    }

    ~CItemIterator()
    {
        CoTaskMemFree(_pidlFull);
        SafeRelease(&_psfCur);
    }

    bool MoveNext()
    {
        bool fMoreItems = false;
        if (SUCCEEDED(_hr))
        {
            if (NULL == _pidlRel)
            {
                fMoreItems = true;
                _pidlRel = _pidlFull;   // First item - Might be empty if it is the desktop
            }
            else if (!ILIsEmpty(_pidlRel))
            {
                PCUITEMID_CHILD pidlChild = (PCUITEMID_CHILD)_pidlRel;  // Save the current segment for binding
                _pidlRel = ILNext(_pidlRel);

                // If we are not at the end setup for the next iteration
                if (!ILIsEmpty(_pidlRel))
                {
                    const WORD cbSave = _pidlRel->mkid.cb;  // Avoid cloning for the child by truncating temporarily
                    _pidlRel->mkid.cb = 0;                  // Make this a child

                    IShellFolder* psfNew;
                    _hr = _psfCur->BindToObject(pidlChild, NULL, IID_PPV_ARGS(&psfNew));
                    if (SUCCEEDED(_hr))
                    {
                        _psfCur->Release();
                        _psfCur = psfNew;   // Transfer ownership
                        fMoreItems = true;
                    }

                    _pidlRel->mkid.cb = cbSave; // Restore previous ID size
                }
            }
        }
        return fMoreItems;
    }

    HRESULT GetCurrent(REFIID riid, void** ppv)
    {
        *ppv = NULL;
        if (SUCCEEDED(_hr))
        {
            // Create the childID by truncating _pidlRel temporarily
            PUIDLIST_RELATIVE pidlNext = ILNext(_pidlRel);
            const WORD cbSave = pidlNext->mkid.cb;  // Save old cb
            pidlNext->mkid.cb = 0;                  // Make _pidlRel a child

            _hr = SHCreateItemWithParent(NULL, _psfCur, (PCUITEMID_CHILD)_pidlRel, riid, ppv);

            pidlNext->mkid.cb = cbSave;             // Restore old cb
        }
        return _hr;
    }

    HRESULT GetResult() const { return _hr; }
    PCUIDLIST_RELATIVE GetRelativeIDList() const { return _pidlRel; }

private:
    void _Init()
    {
        _pidlRel = NULL;

        if (SUCCEEDED(_hr))
        {
            _hr = SHGetDesktopFolder(&_psfCur);
        }
    }

    HRESULT _hr;
    PIDLIST_ABSOLUTE _pidlFull;
    PUIDLIST_RELATIVE _pidlRel;
    IShellFolder* _psfCur;
};



static HRESULT GetIDListName(IShellItem* psi, PWSTR* ppsz, std::vector<std::string>* dirs)
{
    *ppsz = NULL;
    HRESULT hr = E_FAIL;

    WCHAR szFullName[2048];
    szFullName[0] = 0;
    PWSTR pszOutput = szFullName;
    size_t cchOutput = ARRAYSIZE(szFullName);


    CItemIterator itemIterator(psi);
    while (itemIterator.MoveNext())
    {
        IShellItem2* psi;
        hr = itemIterator.GetCurrent(IID_PPV_ARGS(&psi));
        if (SUCCEEDED(hr))
        {
            PWSTR pszName;
            hr = psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszName);
            if (SUCCEEDED(hr))
            {
                // Ignore errors, this is for debugging only
                StringCchCatEx(pszOutput, cchOutput, L"[", &pszOutput, &cchOutput, 0);
                StringCchCatEx(pszOutput, cchOutput, pszName, &pszOutput, &cchOutput, 0);
                StringCchCatEx(pszOutput, cchOutput, L"]", &pszOutput, &cchOutput, 0);

                char result[500];
                wcstombs(result, pszName, 500);
                dirs->push_back(std::string(result));
                CoTaskMemFree(pszName);
            }
            psi->Release();
        }
    }

    if (SUCCEEDED(hr))
    {
        char result[500];
        wcstombs(result, szFullName, 500);
        hr = SHStrDup((LPCSTR)result, ppsz);
    }
    return hr;
}

static HRESULT GetSelectionFromSite(IUnknown* punkSite, BOOL fNoneImpliesFolder, IShellItemArray** ppsia)
{
    *ppsia = NULL;
    IFolderView2* pfv;
    HRESULT hr = IUnknown_QueryService(punkSite, SID_SFolderView, IID_PPV_ARGS(&pfv));
    if (SUCCEEDED(hr))
    {
        hr = pfv->GetSelection(fNoneImpliesFolder, ppsia);
        pfv->Release();
    }
    return hr;
}

static void DeletePerUserDialogState()
{
    IFileDialog* pfd;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Delete window size, MRU and other saved data for testing initial case
        pfd->ClearClientData();
        pfd->Release();
    }
}

static void ReportSelectedItems(IUnknown* punkSite, IShellItemArray* psia)
{
    DWORD cItems;
    HRESULT hr = psia->GetCount(&cItems);
    for (DWORD i = 0; SUCCEEDED(hr) && (i < cItems); i++)
    {
        IShellItem* psi;
        hr = psia->GetItemAt(i, &psi);
        if (SUCCEEDED(hr))
        {
            PWSTR pszName;
            std::vector<std::string> dirs;
            hr = GetIDListName(psi, &pszName, &dirs);
            if (SUCCEEDED(hr))
            {
                HWND hwnd;
                IUnknown_GetWindow(punkSite, &hwnd);
                int nButton;
                const TASKDIALOG_COMMON_BUTTON_FLAGS buttonFlags = (i == (cItems - 1)) ? TDCBF_OK_BUTTON : TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
                WCHAR szMsg[128];
                StringCchPrintf(szMsg, ARRAYSIZE(szMsg), L"Item %d of %d added to basket", i + 1, cItems);
                if (SUCCEEDED(TaskDialog(hwnd, 0, L"Items Addded to Basket", szMsg, pszName, buttonFlags, NULL, &nButton)))
                {
                    hr = (nButton == IDCANCEL) ? HRESULT_FROM_WIN32(ERROR_CANCELLED) : S_OK;
                }
                CoTaskMemFree(pszName);
            }
            psi->Release();
        }
    }
}

static void ReportSelectedItemsFromSite(IUnknown* punkSite)
{
    IShellItemArray* psia;
    HRESULT hr = GetSelectionFromSite(punkSite, TRUE, &psia);
    if (SUCCEEDED(hr))
    {
        ReportSelectedItems(punkSite, psia);
        psia->Release();
    }
}

#endif