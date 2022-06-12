#pragma once

#include <engine/types.hpp>

namespace engine
{

    struct IndirectPack
    {
        Mesh *mesh;
        Material *material;
        uint32_t first;
        uint32_t count;
    };

    template <typename T>
    struct Ref
    {
        using type = T;
        Ref() = default;

    private:
        Ref(uint32_t i, uint32_t b) : m_pIndex(i) {}
        uint32_t m_pIndex = 0;
    };

    template <typename T, typename R>
    struct Pool {
        using DataArray = std::list<Ref<T>>;
        using DataArrayIterator = std::list<Ref<T>>::iterator;

        using RefArray = std::list<R>;
        using RefArrayIterator = std::list<R>::iterator;

        private:
            DataArray m_pData;
    };

}