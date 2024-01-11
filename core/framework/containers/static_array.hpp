#pragma once

namespace framework {

    template <typename T, std::size_t N>
    class StatiArray {
    private:

        using value_type = T;
        using reference_type = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;

        value_type m_pData[N];

    public:
        StatiArray() = default;
        StatiArray(std::initializer_list<value_type>&& in_data) {
            if (in_data.size() != N) {
                throw std::runtime_error(
                    "Initializer list size() differs from array size");
            }
            size_t i = 0;
            for (value_type val : in_data) {
                m_pData[i] = val;
                i++;
            }
        }

        void operator=(std::initializer_list<T>&& in_data) {
            if (in_data.size() != N) {
                throw std::runtime_error(
                    "Initializer list size() differs from array size");
            }

            size_t i = 0;
            for (value_type val : in_data) {
                m_pData[i] = val;
                i++;
            }
        }

        // Element access
        constexpr reference_type at(size_t index) {
            if (index >= N || index < 0) {
                throw std::out_of_range("Index is out of bounds");
            }

            return m_pData[index];
        }

        constexpr reference_type operator[](size_t index) {
            return m_pData[index];
        }

        constexpr reference_type front() {
            return m_pData[0];
        }

        constexpr reference_type back() {
            return m_pData[N - 1];
        }

        pointer data() { return m_pData; }

        // Iterators

        constexpr pointer begin() noexcept { return m_pData; }

        constexpr const_pointer begin() const noexcept { return m_pData; }

        constexpr pointer end() noexcept { return m_pData + N; }

        constexpr const_pointer end() const noexcept { return m_pData + N; }

        constexpr const_pointer cbegin() const noexcept { return m_pData; }

        constexpr const_pointer cend() const noexcept { return m_pData + N; }

        // Capacity

        bool empty() { return N == 0; }

        constexpr size_t size() const noexcept { return N; }

        size_t max_size() { return N; }

        // Operations

        constexpr void fill(const_reference in_data) {
            for (size_t i = 0; i < N; i++) {
                m_pData[i] = in_data;
            }
        }

        constexpr void swap(StatiArray<value_type, N>& other) noexcept {
            for (size_t i = 0; i < N; i++) {
                value_type other_value = other[i];
                other[i] = m_pData[i];
                m_pData[i] = other_value;
            }
        }
    };
}