#if 0
/*
 * std::set was giving me bugs so I wrote this slow set class
 */
#pragma once

#include <vector>

template <typename T>
class set
{
    private:
        std::vector<T> container;

    public:
        void add(T value);
        bool remove(T value);
        bool empty() const;
        size_t size() const;
        bool contains(const T& value) const;
        bool contains(const set<T>& other) const;
        set<T> set_union(const set<T>& other) const;
        set<T> set_intersection(const set<T>& other) const;
        set<T> set_difference(const set<T>& other) const;

        set<T> operator|(const set<T>& other) const { return set_union(other); }
        set<T> operator&(const set<T>& other) const { return set_intersection(other); }
        set<T> operator-(const set<T>& other) const { return set_difference(other); }

        bool operator==(const set<T>& other) const;
};
#endif
