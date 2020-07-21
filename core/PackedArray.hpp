#pragma once

template <typename T>
class PackedArray {
    public:
        PackedArray() = default;

        void emplace(T) {}
        void emplace_back() {}

        std::size_t size() {}
        std::size_t capacity() {}
        void swap() {}
        void clear() {}
    
        void resize() {}
        void reserve() {}
        void shrink_to_fit() {} 

        const T & begin() {return _packed_[0]; }
        const T & end() {return _packed_[mSize-1]; }
    private:
        std::size_t mCapacity;
        std::size_t mSize;

        T* _packed_;
        T* _sparse_;
};