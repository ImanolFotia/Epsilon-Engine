#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <mutex>

namespace engine
{
    template <typename T>
    struct Ref
    {
        static bool isSame(Ref<T> a, Ref<T> b)
        {
            return a.m_pIndex == b.m_pIndex && a.m_pGeneration == b.m_pGeneration;
        }

        Ref() = default;

        static Ref<T> makeEmpty()
        {
            return Ref(0, 0, -1);
        }

        bool empty()
        {
            return m_pID == -1;
        }

        inline uint32_t Index() { return m_pIndex; }
        uint32_t Id() { return m_pID; }
        const bool isValid() const { return m_pGeneration != 0; }

        uint32_t m_pIndex{};

    private:
        Ref(uint32_t i, uint32_t b, uint32_t id) : m_pIndex(i), m_pGeneration(b), m_pID(id) {}


        uint32_t m_pID = -1;
        uint8_t m_pGeneration{};

        template <typename A, typename B>
        friend class Pool;
    };

    template <typename T, typename R>
    struct Pool
    {
        using InternalDataArray = std::list<R>;
        using FreeRefArray = std::list<Ref<T>>;
        using GenerationArray = std::unordered_map<unsigned int, uint32_t>;
        using IdArray = std::unordered_map<uint32_t, uint32_t>;
        using IndexArray = std::vector<typename std::list<R>::iterator>;


        std::size_t size() {
            return m_pInternalData.size();
        }

        template <typename... Args>
        Ref<T> emplace(std::string name, Args... args)
        {
           // const std::lock_guard<std::mutex> lock(m_pMutex);
            if (m_pFreeRefs.size() > 0)
            {
                Ref<T> ref = m_pFreeRefs.back();
                m_pFreeRefs.pop_back();

                ref.m_pGeneration = ++m_pGeneration[ref.m_pIndex];
                auto it = m_pInternalData.emplace(m_pIndexArray.at(ref.m_pIndex),
                                                  args...);
                m_pIndexArray.at(ref.m_pIndex) = it;
                std::cout << "reused reference\n";
                return ref;
            }

            m_pInternalData.emplace_back(args...);
            m_pIndexArray.push_back(std::prev(m_pInternalData.end()));

            uint32_t index = m_pIndexArray.size() - 1;
            uint32_t id = std::hash<std::string>{}(name);

            Ref<T> ref(index, 1, id);
            ref.m_pIndex = index;
            m_pGeneration[index] = 1;
            m_pIdArray[id] = index;

            return ref;
        }
        Ref<T> insert(std::string name, R element)
        {
            //const std::lock_guard<std::mutex> lock(m_pMutex);
            if (m_pFreeRefs.size() > 0)
            {
                Ref<T> ref = m_pFreeRefs.back();
                m_pFreeRefs.pop_back();

                ref.m_pGeneration = ++m_pGeneration[ref.m_pIndex];

                m_pInternalData.insert(m_pIndexArray.at(ref.m_pIndex), element);
                return ref;
            }

            m_pInternalData.push_back(element);
            m_pIndexArray.push_back(std::prev(m_pInternalData.end()));
            uint32_t index = m_pIndexArray.size() - 1;
            uint32_t id = std::hash<std::string>{}(name);

            Ref<T> ref(index, 1, id);
            m_pGeneration[index] = 1;
            m_pIdArray[id] = index;

            return std::move(ref);
        }

        [[nodiscard]] R *get(Ref<T> ref)
        {
            if (m_pIndexArray.size() > ref.m_pIndex && ref.isValid())
            {
                {
                    return &(*m_pIndexArray[ref.m_pIndex]);
                }
            }

            return nullptr;
        }

        [[nodiscard]] R *get(uint32_t id)
        {
            if (m_pIdArray.contains(id))
            {
                auto index = m_pIdArray.at(id);

                if (m_pIndexArray.size() > index)
                {
                    return &(*m_pIndexArray.at(index));
                }
            }

            return nullptr;
        }


        [[nodiscard]] uint32_t getId(Ref<T> ref)
        {
            if (m_pIndexArray.size() > ref.m_pIndex && ref.isValid())
            {
                if (m_pGeneration[ref.m_pIndex] == ref.m_pGeneration)
                {
                    return ref.m_pIndex;
                }
            }
            std::cout << "object is invalid\n";

            return -1;
        }

        void destroy(Ref<T> &ref)
        {
            //const std::lock_guard<std::mutex> lock(m_pMutex);
            std::cout << "internal data size before: " << m_pInternalData.size()
                      << std::endl;
            if (m_pIndexArray.size() > ref.m_pIndex)
            {
                if (ref.m_pGeneration ==
                    m_pGeneration[ref.m_pIndex])
                { // we dont want to add this free
                  // reference twice
                    m_pFreeRefs.insert(m_pFreeRefs.end(), ref);
                    m_pInternalData.erase(m_pIndexArray.at(ref.m_pIndex));
                    m_pIdArray.erase(ref.Id);
                    ref.m_pGeneration = 0; // invalidate reference
                }
            }

            std::cout << "internal data size after: " << m_pInternalData.size()
                      << std::endl;
        }

        auto begin()
        {
            return m_pInternalData.begin();
        }

        auto end()
        {
            return m_pInternalData.end();
        }

    private:
        GenerationArray m_pGeneration;
        IndexArray m_pIndexArray;
        InternalDataArray m_pInternalData;
        FreeRefArray m_pFreeRefs;
        IdArray m_pIdArray;


        std::mutex m_pMutex;
    };

}