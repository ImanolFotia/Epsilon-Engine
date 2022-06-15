#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <list>
#include <unordered_map>

namespace engine
{

    template <typename T>
    struct Ref
    {
        using type = T;
        Ref() = default;

        bool isValid() { return m_pGeneration == 0; }
    private:
        Ref(uint32_t i, uint32_t g) : m_pIndex(i), m_pGeneration(g) {}
        uint32_t m_pIndex = 0;
        uint32_t m_pGeneration = 0;

        template<typename U, typename S> friend struct Pool;
    };

    template <typename T, typename R>
    struct Pool
    {

        Ref<T> getNewRef(R r) { 
            Ref<T> ref(m_pNumReferences, 0);
            references[m_pNumReferences] = r;
            m_pNumReferences++;
            return ref;
         }

    private:
        uint32_t m_pNumReferences = 0;
        std::unordered_map<uint32_t, R> references;
    };
}