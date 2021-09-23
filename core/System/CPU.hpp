#pragma once

#include <pch.hpp>

#ifdef _WIN32
#include <windows.h>
#include <TCHAR.h>
#include <pdh.h>
#include <intrin.h>
#endif

namespace Epsilon::System
{
    class CPU
    {
        class CPU_Info
        {
        public:
            struct cpu_core
            {
                double max_frequency;
                double base_frequency;
                double current_frequency;

                double current_usage;
                double current_usage_by_this_process;
            };

        public:
            CPU_Info()
            {
#ifdef _WIN32
                PdhOpenQuery(NULL, 0, &cpuQuery);
                // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
                PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
                PdhCollectQueryData(cpuQuery);

                ////////////////////////////////
                SYSTEM_INFO sysInfo;
                FILETIME ftime, fsys, fuser;

                GetSystemInfo(&sysInfo);
                mNumCores = sysInfo.dwNumberOfProcessors;
                mCores = new cpu_core[mNumCores];

                GetSystemTimeAsFileTime(&ftime);
                memcpy(&lastCPU, &ftime, sizeof(FILETIME));

                self = GetCurrentProcess();
                GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
                memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
                memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

                wchar_t Buffer[_MAX_PATH];
                DWORD BufSize = _MAX_PATH;
                DWORD dwMHz = _MAX_PATH;
                HKEY hKey;

                // open the key where the proc speed is hidden:
                long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                           "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                                           0,
                                           KEY_READ,
                                           &hKey);
                if (lError != ERROR_SUCCESS)
                { // if the key is not found, tell the user why:
                    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                  NULL,
                                  lError,
                                  0,
                                  (char *)Buffer,
                                  _MAX_PATH,
                                  0);
                    wprintf(Buffer);
                    return;
                }

                // query the key:
                RegQueryValueEx(hKey, "~MHz", NULL, NULL, (LPBYTE)&dwMHz, &BufSize);
                mBaseFrequency = dwMHz;

#endif
            }

            ~CPU_Info()
            {
                if (mCores != nullptr)
                    delete[] mCores;
            }

            double CurrentTotalUsage()
            {
#ifdef _WIN32
                PDH_FMT_COUNTERVALUE counterVal;

                PdhCollectQueryData(cpuQuery);
                PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
                return counterVal.doubleValue;
#else
                return 0.0;
#endif
            }

            double CurrentTotalUsageByThisProcess()
            {
#ifdef _WIN32
                FILETIME ftime, fsys, fuser;
                ULARGE_INTEGER now, sys, user;
                double percent;

                GetSystemTimeAsFileTime(&ftime);
                memcpy(&now, &ftime, sizeof(FILETIME));

                GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
                memcpy(&sys, &fsys, sizeof(FILETIME));
                memcpy(&user, &fuser, sizeof(FILETIME));
                percent = (sys.QuadPart - lastSysCPU.QuadPart) +
                          (user.QuadPart - lastUserCPU.QuadPart);
                percent /= (now.QuadPart - lastCPU.QuadPart);
                percent /= mNumCores;
                lastCPU = now;
                lastUserCPU = user;
                lastSysCPU = sys;

                return percent * 100;
#else
                return 0.0;
#endif
            }

            double BaseFrequency()
            {
                return mBaseFrequency;
            }

        private:
            uint32_t mNumCores;
            uint32_t mTotalPhysicalCores = 0;
            uint32_t mTotalThreads = 0;
            double mBaseFrequency = 0.0;

            double mCurrentTotalUsage;
            double mCurrentTotalUsageByThisProcess;

            cpu_core *mCores;

#ifdef _WIN32
            PDH_HQUERY cpuQuery;
            PDH_HCOUNTER cpuTotal;

            ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
            HANDLE self;
#endif
        };

        class CPUID
        {
            uint32_t regs[4];

        public:
            explicit CPUID(unsigned i = 0)
            {
                try
                {
#if defined(_MSC_VER) || defined(__INTEL_COMPILER) // Microsoft or Intel compiler, intrin.h included
                    __cpuidex((int *)regs, i, 0);
#else
                    // ECX is set to zero for CPUID function 4
                    asm volatile("cpuid"
                                 : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                                 : "a"(i), "c"(0));
#endif
                }
                catch (...)
                {
                    throw;
                }

                int CPUInfo[4] = {-1};
                unsigned nExIds, k = 0;
                char CPUBrandString[0x40];
                // Get the information associated with each extended ID.
                __cpuid(CPUInfo, 0x80000000);
                nExIds = CPUInfo[0];
                for (k = 0x80000000; k <= nExIds; ++k)
                {
                    __cpuid(CPUInfo, k);
                    // Interpret CPU brand string
                    if (k == 0x80000002)
                        memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
                    else if (k == 0x80000003)
                        memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
                    else if (k == 0x80000004)
                        memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
                }
                //string includes manufacturer, model and clockspeed
                mProcessorName = CPUBrandString;
            }

            const uint32_t &EAX() const
            {
                return regs[0];
            }
            const uint32_t &EBX() const
            {
                return regs[1];
            }
            const uint32_t &ECX() const
            {
                return regs[2];
            }
            const uint32_t &EDX() const
            {
                return regs[3];
            }

            std::string getCPUVendor()
            {
                std::string vendor;
                vendor += std::string((const char *)&EBX(), 4);
                vendor += std::string((const char *)&EDX(), 4);
                vendor += std::string((const char *)&ECX(), 4);

                return vendor;
            }

            std::string getProcessorName() {
                return mProcessorName;
            }

            bool getEndianes()
            {
                int a = 1;
                return !((char *)&a)[0];
            }

            unsigned getNumberOfThreads()
            {

                unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
                return concurentThreadsSupported;
            }

            void printHardwareInformation()
            {
                std::cout << "CPU Manufacturer: " << getCPUVendor() << std::endl;
                std::cout << "Number of Threads: " << getNumberOfThreads() << std::endl;
                std::cout << "Endianness: " << std::string(getEndianes() ? "Big-Endian" : "Little-Endian") << std::endl;

                Log::WriteToLog("CPU Manufacturer: " + getCPUVendor());
                Log::WriteToLog("Number of Threads: " + Helpers::intTostring(getNumberOfThreads()));
                Log::WriteToLog("Endianness: " + std::string(getEndianes() ? "Big-Endian" : "Little-Endian"));
            }

            private:
                std::string mProcessorName;
        };

    public:
        CPU()
        {
            mCPUID = std::make_shared<CPUID>(0);
            mCPU_Info = std::make_shared<CPU_Info>();
        }

        auto getInfo() -> std::shared_ptr<CPU_Info>
        {
            return mCPU_Info;
        }

        auto getCPUID() -> std::shared_ptr<CPUID>
        {
            return mCPUID;
        }

    private:
        std::shared_ptr<CPU_Info> mCPU_Info;
        std::shared_ptr<CPUID> mCPUID;
    };
}