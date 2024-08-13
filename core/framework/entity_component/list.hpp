#pragma once

#include <vector>
#include <initializer_list>

namespace framework {
    template <typename T>
    class List {
    public:
        List() {}
        List(const std::initializer_list<T>& li) noexcept {
            _list = li;
        }

        [[maybe_unused]] const std::vector<T>& get_list() const noexcept {
            return _list;
        }

        auto end() {
            return _list.end();
        }

        auto begin() {
            return _list.begin();
        }

        auto cend() {
            return _list.cend();
        }

        auto cbegin() {
            return _list.cbegin();
        }

        void insert_back(const T& x) {
            _list.insert(_list.cend(), x);
        }

        void insert_front(const T& x) {
            _list.insert(_list.cbegin(), x);
        }

    private:
        std::vector<T> _list;
    };
}