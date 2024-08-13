#pragma once

//
// Created by solaire on 2/4/23.
//

// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// Standard headers
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <any>
#include <functional>

// Provided by the AppHost NuGet package and installed as an SDK pack
#include "../headers/nethost.hpp"

// Header files copied from https://github.com/dotnet/core-setup
#include "../headers/coreclr_delegates.hpp"
#include "../headers/hostfxr.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <OleAuto.h>
#define DIR_SEPARATOR L'\\'
#else

#include <dlfcn.h>
#include <limits.h>
#define DIR_SEPARATOR L'/'

#endif

#ifdef WINDOWS
#include <Windows.h>

#define STR(s) L##s
#define CH(c) L##c
#define DIR_SEPARATOR L'\\'

#define string_compare wcscmp

#else
#include <dlfcn.h>
#include <limits.h>

#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

#define string_compare strcmp

#endif

namespace dotnet
{

    static std::string convert_string(const std::wstring &str)
    {
        // setup converter
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;

        // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
        std::string converted_str = converter.to_bytes(str);
        return converted_str;
    }

    static std::wstring convert_wstring(const std::string &str)
    {

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wide = converter.from_bytes(str);

        return wide;
    }

    static void callback()
    {
        std::cout << "Hello world invoked in C++ from C#\n";
    }

    struct CppEntity
    {
        std::string name = "Imanol";
    };

    namespace
    {
        hostfxr_initialize_for_runtime_config_fn init_fptr;
        hostfxr_get_runtime_delegate_fn get_delegate_fptr;
        hostfxr_close_fn close_fptr;
        hostfxr_get_runtime_properties_fn get_runtime_properties;
        hostfxr_error_writer_fn error_writer = nullptr;
    }
}