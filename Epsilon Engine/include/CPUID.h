#pragma once

#include <string>
#include <iostream>
#include <limits.h>
#include <stdint.h>
#include <thread>
#include <exception>
class CPUID
{
    uint32_t regs[4];

public:
    explicit CPUID(unsigned i)
    {
        try
        {
            asm volatile
            ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
             : "a" (i), "c" (0));
            // ECX is set to zero for CPUID function 4
        }
        catch(...)
        {
            throw;
        }
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

    bool getEndianes()
    {
        int a=1;
        return !((char*)&a)[0];
    }

    unsigned getNumberOfThreads()
    {

        unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
        return  concurentThreadsSupported;
    }

    void printHardwareInformation()
    {
        std::cout << "CPU Manufacturer: " << getCPUVendor() << std:: endl;
        std::cout << "Number of Threads: " << getNumberOfThreads() << std::endl;
        std::cout << "Endianness: " << std::string(getEndianes() ? "Big-Endian" : "Little-Endian") << std::endl;
    }
};
