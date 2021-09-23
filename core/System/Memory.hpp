#pragma once

#include <windows.h>
#include <psapi.h>

namespace Epsilon::System
{
    class Memory
    {
    public:
        Memory()
        {
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            mTotalVirtualMemory = memInfo.ullTotalPageFile;

            mTotalPhysicalMemory = memInfo.ullTotalPhys;
        }

        uint64_t TotalVirtualMemory()
        {
            //if(mTotalVirtualMemory < 0) mTotalVirtualMemory = -mTotalVirtualMemory;
            return mTotalVirtualMemory;
        }

        uint64_t CurrentVirtualUsage()
        {
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            return memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
        }

        uint64_t CurrentVirtualUsageByCurrentProcess()
        {
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
            return pmc.PrivateUsage;
        }
        
        uint64_t TotalPhysicalMemory()
        {
            //if(mTotalPhysicalMemory < 0) mTotalPhysicalMemory = -mTotalPhysicalMemory;
            return mTotalPhysicalMemory;
        }

        uint64_t CurrentPhysicalUsage()
        {
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            return memInfo.ullTotalPhys - memInfo.ullAvailPhys;
        }
        
        uint64_t CurrentPhysicalUsageByCurrentProcess()
        {
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
            return pmc.WorkingSetSize;
        }

    private:
        uint64_t mTotalPhysicalMemory = 0;
        uint64_t mTotalVirtualMemory = 0;
    };
}