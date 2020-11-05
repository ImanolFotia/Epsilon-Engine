#include <pch.hpp>

namespace Epsilon {
template <typename Type>
class SparseSet
{

    using packed_type = Type;
    using sparse_type = std::unique_ptr<Type>;

public:
    SparseSet() = default;

    SparseSet(const Type &) = delete;

    SparseSet(Type &&) = delete;

    template<class ...Args>
    void emplace(auto it, Args... args)
    {
        _packed_.emplace(it, args...);
    }
    
    template<class ...Args>
    void emplace_back(Args... args)
    {
        _packed_.emplace_back({args...});
    }

    void insert(packed_type element) const
    {
        _packed_.insert(element);
    }
    
    void insert(size_t at, packed_type element) const
    {
        _packed_.insert(at, element);
    }

    auto size() const noexcept {
        return _packed_.size();
    }

    std::size_t capacity() {return _packed_.capacity(); }

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
    }

    const Type & at(size_t i) {
        return _packed_.at(i);
    }

    const auto begin() { return _packed_.begin(); }
    const auto end() { return _packed_.end(); }

private:

    std::vector<packed_type> _packed_;
    std::vector<sparse_type> _sparse_;
};

struct entity {
    entity(int x) : id(x) {}
    int id;
};
};