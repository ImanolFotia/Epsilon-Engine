#pragma once

#include <vector>

namespace framework {
    template <typename Type>
    class SparseSet
    {

        using packed_type = Type;
        using sparse_type = std::unique_ptr<Type>;

    public:
        SparseSet() = default;

        SparseSet(const T&) = delete;

        SparseSet(T&&) = delete;

        ~SparseSet() { }

        void emplace(Type element) const
        {

        }

        auto size() const noexcept {
            return _packed_.size();
        }

        std::size_t capacity() {}

        void swap(SparseSet& _external_)
        {
            decltype(_external_) tmp = _external_;
            _external_ = *this;
            *this = _external_;
        }

        //All elements are deleted but everything else stays the same, including array sizes
        void clear()
        {
            _packed_.clear();
            _sparse_.clear();
        }

        void resize(std::size_t s)
        {
            _packed_.resize(s);
        }
        void reserve(std::size_t s)
        {
            _packed_.reserve(s);
        }

        void shrink_to_fit() {
            _packed_.shrink_to_fit();
            _sparse_.shrink_to_fit();
        }

        const T& begin() { return _packed_[0]; }
        const T& end() { return _packed_[mSize - 1]; }

    private:

        std::vector<packed_type> _packed_;
        std::vector<sparse_type> _sparse_;
    };
}