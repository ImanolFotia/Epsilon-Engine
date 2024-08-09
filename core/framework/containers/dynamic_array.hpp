#pragma once
#include <stdexcept>

namespace framework {

    template <typename T>
    class DynamicArray {
        struct DynamicArrayIterator {
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;  // or also value_type*
            using reference = T&;

            DynamicArrayIterator(pointer ptr) : m_pPtr(ptr) {}

            reference operator*() const { return *m_pPtr; }
            pointer operator->() { return m_pPtr; }

            // Prefix increment
            DynamicArrayIterator& operator++() {
                m_pPtr++;
                return *this;
            }

            // Postfix increment
            DynamicArrayIterator operator++(int) {
                DynamicArrayIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            // Prefix decrement
            DynamicArrayIterator& operator--() {
                m_pPtr--;
                return *this;
            }

            // Postfix decrement
            DynamicArrayIterator operator--(int) {
                DynamicArrayIterator tmp = *this;
                --(*this);
                return tmp;
            }

            DynamicArrayIterator& operator-(size_t val) {
                m_pPtr = m_pPtr - val;
                return *this;
            }

            difference_type operator-(const DynamicArrayIterator& a) const {
                return m_pPtr - a.m_pPtr;
            }

            DynamicArrayIterator& operator+(size_t val) {
                m_pPtr = m_pPtr + val;
                return *this;
            }

            friend bool operator==(const DynamicArrayIterator& a,
                const DynamicArrayIterator& b) {
                return a.m_pPtr == b.m_pPtr;
            };
            friend bool operator!=(const DynamicArrayIterator& a,
                const DynamicArrayIterator& b) {
                return a.m_pPtr != b.m_pPtr;
            };
            pointer m_pPtr;
        };

    private:
        using value_type = T;
        using reference_type = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = DynamicArrayIterator;
        using const_iterator = const DynamicArrayIterator;
        using difference_type = std::ptrdiff_t;

        using size_type = std::size_t;

        pointer m_pData = nullptr;
        size_type m_pSize = 0;
        size_type m_pCapacity = 15;

        void change_capacity(size_type new_capacity) {
            size_type old_capacity = m_pCapacity;
            m_pCapacity = new_capacity;

            pointer new_data = new value_type[m_pCapacity];

            size_type working_capacity =
                new_capacity > old_capacity ? old_capacity : new_capacity;

            for (size_type i = 0; i < working_capacity; i++) {
                new_data[i] = m_pData[i];
            }

            delete[] m_pData;
            m_pData = new_data;
        }

    public:
        DynamicArray() { m_pData = new value_type[m_pCapacity]; }

        DynamicArray(const DynamicArray& other) {
            if (m_pData) delete[] m_pData;
            this->m_pSize = other.m_pSize;
            this->m_pCapacity = other.m_pCapacity;
            m_pData = new value_type[this->m_pCapacity];
            size_t index = 0;
            for (auto other_val : other) {
                m_pData[index] = other_val;
            }
        }

        DynamicArray(DynamicArray&& other) {
            if (m_pData) delete[] m_pData;
            this->m_pSize = std::move(other.m_pSize);
            this->m_pCapacity = std::move(other.m_pCapacity);
            m_pData = other.m_pData;
            other.m_pData = nullptr;
        }


        DynamicArray(std::initializer_list<value_type>&& in_data) {
            m_pData = new value_type[m_pCapacity];
            if (in_data.size() > m_pCapacity) {
                change_capacity(in_data.size());
                m_pSize = in_data.size();
            }
            if (in_data.size() > m_pSize) {
                m_pSize = in_data.size();
            }
            size_t i = 0;
            for (value_type val : in_data) {
                m_pData[i] = val;
                i++;
            }
        }

        ~DynamicArray() {
            if (m_pData != nullptr) delete[] m_pData;
        }

        void operator=(std::initializer_list<T>&& in_data) {
            if (in_data.size() > m_pCapacity) {
                change_capacity(in_data.size());
                m_pSize = in_data.size();
            }
            if (in_data.size() > m_pSize) {
                m_pSize = in_data.size();
            }

            size_t i = 0;
            for (value_type val : in_data) {
                m_pData[i] = val;
                i++;
            }
        }

        constexpr DynamicArray<T>& operator=(const DynamicArray<T>& other) {
            if (m_pData) delete[] m_pData;
            this->m_pSize = other.m_pSize;
            this->m_pCapacity = other.m_pCapacity;
            m_pData = new value_type[this->m_pCapacity];
            size_t index = 0;
            for (auto other_val : other) {
                m_pData[index] = other_val;
            }

            return *this;
        }

        constexpr DynamicArray<T>& operator=(DynamicArray<T>&& other) {
            if (m_pData) delete[] m_pData;
            this->m_pSize = std::move(other.m_pSize);
            this->m_pCapacity = std::move(other.m_pCapacity);
            m_pData = other.m_pData;
            other.m_pData = nullptr;
            return *this;
        }

        // Element access
        constexpr reference_type at(size_type pos) {
            if (pos >= m_pSize || pos < 0) {
                throw std::out_of_range("Index is out of bounds");
            }

            return m_pData[pos];
        }
/*
        constexpr const_reference at(size_type pos) const {
            if (pos >= m_pSize || pos < 0) {
                throw std::out_of_range("Index is out of bounds");
            }

            return m_pData[pos];
        }*/

        constexpr reference_type operator[](size_type pos) { return m_pData[pos]; }
/*
        constexpr const_reference operator[](size_type pos) const {
            return m_pData[pos];
        }
        */

        constexpr reference_type front() { return m_pData[0]; }

        constexpr const_reference front() const { return m_pData[0]; }

        constexpr reference_type back() { return m_pData[m_pSize - 1]; }

        constexpr const_reference back() const { return m_pData[m_pSize - 1]; }

        pointer data() { return m_pData; }

        // Iterators

        constexpr iterator begin() noexcept { return iterator(m_pData); }

        constexpr const_iterator begin() const noexcept {
            return const_iterator(m_pData);
        }

        constexpr iterator end() noexcept { return iterator(m_pData + m_pSize); }

        constexpr const_iterator end() const noexcept {
            return const_iterator(m_pData + m_pSize);
        }

        constexpr const_iterator cbegin() const noexcept {
            return const_iterator(m_pData);
        }

        constexpr const_iterator cend() const noexcept {
            return const_iterator(m_pData + m_pSize);
        }

        // Capacity

        bool empty() { return m_pSize == 0; }

        constexpr size_t size() const noexcept { return m_pSize; }

        size_t max_size() { return std::numeric_limits<std::ptrdiff_t>::max(); }

        size_t capacity() { return m_pCapacity; }

        void reserve(size_type new_size) {
            if (new_size > m_pCapacity) {
                change_capacity(new_size);
            }
        }

        void shrink_to_fit() {
            if (m_pSize < m_pCapacity) {
                change_capacity(m_pSize);
            }
        }

        // Modifiers

        constexpr void clear() noexcept {
            if (m_pData != nullptr) {
                delete[] m_pData;
                m_pData = nullptr;
            }
            m_pSize = 0;
            m_pCapacity = 0;
        }

        /*constexpr iterator insert(const_iterator pos, const T& value);
        constexpr iterator insert(const_iterator pos, T&& value);
        constexpr iterator insert(const_iterator pos, size_type count, const T& value);
        constexpr iterator insert(const_iterator pos, InputIt first, InputIt last); constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist);
        */
        template <class... Args>
        constexpr reference_type emplace(const_reference pos, Args&&... args) {
            throw std::runtime_error("emplace not implemented");
        }


        template <class... Args>
        constexpr reference_type emplace_back(Args&&... args) {
            if (m_pSize >= m_pCapacity) {
                change_capacity(m_pCapacity + 1 * 2);
            }

            m_pData[m_pSize] = value_type(args...);
            m_pSize++;

            return back();
        }

        constexpr void push_back(const T& value) {
            if (m_pSize >= m_pCapacity) {
                change_capacity(m_pCapacity + 1 * 2);
            }

            m_pData[m_pSize] = value;
            m_pSize++;
        }

        constexpr void push_back(T&& value) {
            if (m_pSize >= m_pCapacity) {
                change_capacity(m_pCapacity + 1 * 2);
            }

            m_pData[m_pSize] = value;
            m_pSize++;
        }

        constexpr void pop_back() {
            m_pSize--;
            m_pSize = m_pSize < 0 ? 0 : m_pSize;
        }

        void resize(size_type new_size) {
            change_capacity(new_size+1);

            m_pSize = new_size;
        }

        constexpr void swap(DynamicArray<value_type>& other) noexcept {
            pointer other_data_ref = other.data();
            size_type other_size = other.size(), other_capacity = other.capacity();

            other = m_pData;
            other.m_pSize = m_pSize;
            other.m_pCapacity = m_pCapacity;

            m_pData = other_data_ref;
            m_pSize = other_size;
            m_pCapacity = other_capacity;
        }

        constexpr iterator erase(const_iterator pos) {
            assert(pos != end() && "iterator is end");

            difference_type ps = pos - cbegin();
            iterator p = begin() + ps;

            for (iterator it = p; p != end();) {
                *(it++) = *(++p);
            }
            m_pSize--;
            change_capacity(m_pSize);

            if (pos != end()) return iterator(begin() + ps - 1);

            return end();
        }
    };
}