#pragma once

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

namespace Epsilon::System
{
    class Memory
    {
    public:
        Memory()
        {

#ifdef _WIN32
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            mTotalVirtualMemory = memInfo.ullTotalPageFile;

            mTotalPhysicalMemory = memInfo.ullTotalPhys;
#endif
        }

        uint64_t TotalVirtualMemory()
        {
            //if(mTotalVirtualMemory < 0) mTotalVirtualMemory = -mTotalVirtualMemory;
            return mTotalVirtualMemory;
        }

        uint64_t CurrentVirtualUsage()
        {

#ifdef _WIN32
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            return memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
#endif
            return 0;
        }

        uint64_t CurrentVirtualUsageByCurrentProcess()
        {

#ifdef _WIN32
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
            return pmc.PrivateUsage;
#endif
            return 0;
        }

        uint64_t TotalPhysicalMemory()
        {
            //if(mTotalPhysicalMemory < 0) mTotalPhysicalMemory = -mTotalPhysicalMemory;
            return mTotalPhysicalMemory;
        }

        uint64_t CurrentPhysicalUsage()
        {

#ifdef _WIN32
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            return memInfo.ullTotalPhys - memInfo.ullAvailPhys;
#endif
            return 0;
        }

        uint64_t CurrentPhysicalUsageByCurrentProcess()
        {

#ifdef _WIN32
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
            return pmc.WorkingSetSize;
#endif
            return 0;
        }

    private:
        uint64_t mTotalPhysicalMemory = 0;
        uint64_t mTotalVirtualMemory = 0;
    };
}