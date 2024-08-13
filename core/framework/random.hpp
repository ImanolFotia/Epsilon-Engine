#pragma once

#include <concepts>
#include <random>
#include <type_traits>

namespace framework {

template <typename T> class Random {
  using distribution_type = std::conditional_t<std::is_integral<T>::value, std::uniform_int_distribution<T>,
                                               std::uniform_real_distribution<T>>;

  std::random_device m_RandomDevice{};
  std::mt19937 m_RandomEngine{};
  distribution_type m_Distribution{};

  T m_Seed{};

  T m_Min = 0;
  T m_Max = 1;

public:
  Random() {
    m_Distribution = distribution_type(m_Min, m_Max);
    m_Seed = m_RandomDevice();
    m_RandomEngine.seed(m_Seed);
  }

  Random(T min, T max) : m_Distribution{distribution_type(min, max)}, m_Min{min}, m_Max{max} {
    m_Seed = m_RandomDevice();
    m_RandomEngine.seed(m_Seed);
  }

  T Get() { return m_Distribution(m_RandomEngine); }

  T Next() { return Get(); }

  T Seed() { return m_Seed; }

  void Seed(T seed) {
    m_Seed = seed;
    m_RandomEngine.seed(m_Seed);
  }

  T Min() { return m_Min; }

  T Max() { return m_Max; }
};

template <typename T> static inline T QuickRandom(T min = 0, T max = 1) {
  using distribution_type = std::conditional_t<std::is_integral<T>::value, std::uniform_int_distribution<T>,
                                               std::uniform_real_distribution<T>>;

  auto distribution = distribution_type(min, max);
  std::random_device random_device{};
  std::mt19937 random_engine(random_device());

  return distribution(random_engine);
}
} // namespace framework