#pragma once
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace engine {
template <typename T> struct Ref {
  static bool isSame(Ref<T> a, Ref<T> b) { return a.m_pIndex == b.m_pIndex && a.m_pGeneration == b.m_pGeneration; }

  Ref() = default;

  static Ref<T> makeEmpty() { return Ref(0, 0, -1); }

  bool empty() { return m_pID == -1; }

  inline uint32_t Index() { return m_pIndex; }
  uint32_t Id() { return m_pID; }
  uint8_t Generation() { return m_pGeneration; }
  const bool isValid() const { return m_pGeneration != 0; }

  inline friend bool operator==(const Ref<T> &a, const Ref<T> &b) {
    return a.m_pID == b.m_pID && a.m_pIndex == b.m_pIndex && a.m_pGeneration == b.m_pGeneration;
  }

  inline friend bool operator!=(const Ref<T> &a, const Ref<T> &b) {
    return a.m_pID != b.m_pID || a.m_pIndex != b.m_pIndex || a.m_pGeneration != b.m_pGeneration;
  }

  uint32_t m_pIndex{};

private:
  Ref(uint32_t i, uint32_t b, uint32_t id) : m_pIndex(i), m_pID(id), m_pGeneration(b) {}

  uint32_t m_pID = -1;
  uint8_t m_pGeneration{};

  template <typename A, typename B> friend class Pool;
};

template <typename T, typename R> struct Pool {
  std::size_t size() { return m_Count; }
  const size_t c_MaxObjects = 1000;
  size_t m_Count = 0;
  R *m_Data = nullptr;
  std::unordered_map<size_t, size_t> m_IdMap;
  std::unordered_map<size_t, size_t> m_GenerationCache;
  std::vector<Ref<T>> m_FreeRefs{};

  void Initialize() { m_Data = new R[c_MaxObjects]; }

  template <typename... Args> Ref<T> emplace(std::string name, Args... args) {
    size_t current_index = 0;
    size_t generation = 1;
    if (m_FreeRefs.size() > 0) {
      auto ref = m_FreeRefs.back();
      current_index = ref.Index();
      generation = ref.Generation();
      m_FreeRefs.pop_back();
    } else {
      current_index = m_Count++;
    }

    if (current_index >= c_MaxObjects) {
      throw std::runtime_error("Error: Too many allocated objects in Object Pool");
      std::exit(255);
    }
    size_t id = std::hash<std::string>{}(name);

    m_IdMap[id] = current_index;
    m_GenerationCache[id] = generation;

    m_Data[current_index] = R(std::forward<Args>(args)...);

    Ref<T> ref(current_index, generation, id);
    current_index++;

    return ref;
  }

  Ref<T> insert(std::string name, R element) {
    size_t current_index = 0;
    size_t generation = 1;
    if (m_FreeRefs.size() > 0) {
      auto ref = m_FreeRefs.back();
      current_index = ref.Index();
      generation = ref.Generation();
      m_FreeRefs.pop_back();
    } else {
      current_index = m_Count++;
    }

    if (current_index >= c_MaxObjects) {
      throw std::runtime_error("Error: Too many allocated objects in Object Pool");
      std::exit(255);
    }
    size_t id = std::hash<std::string>{}(name);

    m_IdMap[id] = current_index;
    m_GenerationCache[id] = generation;

    m_Data[current_index] = element;

    Ref<T> ref(current_index, generation, id);
    current_index++;

    return ref;
  }

  Ref<T> getRef(size_t id) {
    if(m_IdMap.contains(id)) {
      Ref<T> ref(m_IdMap[id], m_GenerationCache[id], id);
      return ref;
    }
    return Ref<T>::makeEmpty();
  }

  [[nodiscard]] R *get(Ref<T> ref) {
    if (ref.isValid()) {
      return &m_Data[ref.m_pIndex];
    }
    throw std::runtime_error("Reference to object is no longer valid");
    std::exit(255);
    return nullptr;
  }

  [[nodiscard]] R *get(size_t id) {
    if (m_IdMap.contains(id)) {
      return &m_Data[m_IdMap[id]];
    }
    // throw std::runtime_error("Object not found");
    // std::exit(255);
    return nullptr;
  }

  [[nodiscard]] uint32_t getId(Ref<T> ref) { return ref.m_pIndex; }

  void destroy(Ref<T> ref) {
    m_IdMap.erase(ref.m_pID);
    m_GenerationCache.erase(ref.m_pID);
    m_FreeRefs.push_back(ref);
    ref.m_pGeneration++;
  }

  auto begin() { return &m_Data[0]; }

  auto end() { return &m_Data[m_Count]; }
};
} // namespace engine