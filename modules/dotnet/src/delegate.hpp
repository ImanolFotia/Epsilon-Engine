#pragma once

#include "common.hpp"
namespace dotnet
{
    template <typename ReturnType, typename... Arguments>
    class DotnetDelegate
    {
    public:
        using FunctionSignature = ReturnType(CORECLR_DELEGATE_CALLTYPE *)(Arguments... args);

        FunctionSignature m_pFunctionPointer;
        std::wstring m_pName;

    public:
        DotnetDelegate(const std::wstring &name, FunctionSignature fnct_ptr) : m_pName(name), m_pFunctionPointer(fnct_ptr)
        {
        }
    };
}