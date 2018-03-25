#pragma once

#include <cstring>
#include <algorithm>
#include <functional>

#include "v2.hpp"
#include "assert.hpp"
#include "types.hpp"

template <typename T>
T merger_replace(const T& previous, const T& new_value)
{
    return new_value;
}

template <typename T, typename v_t = vi2>
class grid
{
    private:
        uint width, height;
        T * data;

    public:
        grid(const grid<T, v_t> & copy);
        grid(uint width, uint height);
        ~grid();

        T operator[](const v_t & pos) const;
        T & operator[](const v_t & pos);
        grid<T, v_t> & operator=(const grid<T, v_t> & other);
        void fill(const T & value);
        void fill(vi2 pos, vi2 size, const T & value);

        uint get_width() const { return width; }
        uint get_height() const { return height; }
        vu2 get_size() const { return {width, height}; }

        void apply_path(const std::vector<vi2> & path,
                const T & peg,
                const T & hor_wire,
                const T & vert_wire,
                const T & slash_wire,
                const T & bslash_wire,
                std::function<T(const T&, const T&)> merger = merger_replace);
};

template <typename T, typename v_t>
grid<T, v_t>::grid(const grid<T, v_t> & copy)
    : grid<T, v_t>(copy.width, copy.height)
{
    std::memcpy(data, copy.data, width * height * sizeof(T));
}

template <typename T, typename v_t>
grid<T, v_t>::grid(uint width, uint height)
    : width(width), height(height)
{
    data = (T*)malloc(width * height * sizeof(T));
}

template <typename T, typename v_t>
grid<T, v_t>::~grid()
{
    free(data);
}

template <typename T, typename v_t>
T grid<T, v_t>::operator[](const v_t & pos) const
{
    ASSERT(pos.x, (signed)width, <);
    ASSERT(pos.y, (signed)height, <);
    return data[pos.x + width * pos.y];
}

template <typename T, typename v_t>
T & grid<T, v_t>::operator[](const v_t & pos)
{
    ASSERT(pos.x, (signed)width, <);
    ASSERT(pos.y, (signed)height, <);
    return data[pos.x + width * pos.y];
}

template <typename T, typename v_t>
grid<T, v_t> & grid<T, v_t>::operator=(const grid<T, v_t> & other)
{
    if(width != other.width || height != other.height)
    {
        free(data);
        width = other.width; height = other.height;
        data = (T*)malloc(width * height * sizeof(T));
    }
    std::memcpy(data, other.data, width * height * sizeof(T));
    return *this;
}

template <typename T, typename v_t>
void grid<T, v_t>::fill(const T & value)
{
    for(uint index = 0; index < width * height; index++)
        data[index] = value;
}

template <typename T, typename v_t>
void grid<T, v_t>::fill(vi2 pos, vi2 size, const T & value)
{
    auto min_x = std::max(pos.x, (int_t)0);
    auto max_x = std::min(pos.x + size.x, (int_t)width);
    auto min_y = std::max(pos.y, (int_t)0);
    auto max_y = std::min(pos.y + size.y, (int_t)height);
    for(int_t y = min_y; y < max_y; y++)
    {
        for(int_t x = min_x; x < max_x; x++)
        {
            (*this)[{x, y}] = value;
        }
    }
}

template <typename T, typename v_t>
void grid<T, v_t>::apply_path(
        const std::vector<vi2> & path,
        const T & peg,
        const T & hor_wire,
        const T & vert_wire,
        const T & slash_wire,
        const T & bslash_wire,
        std::function<T(const T&, const T&)> merger)
{
    for(auto prev_pos = path.begin(), pos = std::next(path.begin(), 1);
            pos != path.end();
            (prev_pos++, pos++))
    {
        (*this)[*prev_pos] = merger((*this)[*prev_pos], peg);
        auto dir = (*pos - *prev_pos).normalized();
        for(auto pointer = *prev_pos + dir; pointer != *pos; pointer += dir)
        {
            auto value = (*this)[pointer];
            auto& lvalue = (*this)[pointer];
            if(dir.x == 0)
                lvalue = merger(value, vert_wire);
            else if(dir.y == 0)
                lvalue = merger(value, hor_wire);
            else if(dir.x != dir.y)
                lvalue = merger(value, slash_wire);
            else if(dir.x == dir.y)
                lvalue = merger(value, bslash_wire);
            else
            {
                std::cout << dir << std::endl;
                assert(false);
            }
        }
    }
    (*this)[*path.rbegin()] = merger((*this)[*path.rbegin()], peg);
}
