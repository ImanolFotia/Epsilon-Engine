#pragma once

namespace framework {

    static std::size_t allocated = 0;

    inline void* operator new(size_t size) {
        std::cout << "allocating: " << size << " bytes" << std::endl;
        allocated += size;
        return malloc(size);
    }

    void operator delete(void* ptr, std::size_t sz) {
        std::cout << "deallocating: " << sz << " bytes" << std::endl;
        allocated -= sz;
        free(ptr);
    }

    static void print_info() {
        std::cout << "allocated: " << allocated << std::endl;
    }
}
