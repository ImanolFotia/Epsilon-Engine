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

#define STR(s) L##s
#define CH(c) L##c

namespace dotnet
{

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