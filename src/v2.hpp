#pragma once

#include <cstdint>
#include <cmath>
#include <iostream>

typedef uint_fast64_t uint;

template <typename int_t, typename T>
int_t sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

template <typename int_t>
struct v2
{
    int_t x, y;

    v2(const v2<int_t> & copy);
    v2(int_t x, int_t y);
    v2();

    v2<int_t> operator+(const v2<int_t> & other) const;
    v2<int_t> operator-(const v2<int_t> & other) const;

    v2<int_t> & operator+=(const v2<int_t> & other);
    v2<int_t> & operator-=(const v2<int_t> & other);

    v2<int_t> operator*(const int_t & value) const;
    v2<int_t> operator/(const int_t & value) const;

    bool operator==(const v2<int_t> & other) const;
    bool operator!=(const v2<int_t> & other) const;
    bool operator<(const v2<int_t> & other) const;

    int_t manhattan() const;
    bool aligned() const { return (x == 0) || (y == 0); }
    v2<int_t> normalized() const;

    template <typename _int_t>
    friend std::ostream & operator<<(std::ostream & out,
            const v2<_int_t> & vector);
};

template <typename int_t>
v2<int_t>::v2(const v2<int_t> & copy) : x(copy.x), y(copy.y) {}

template <typename int_t>
v2<int_t>::v2(int_t x, int_t y) : x(x), y(y) {}

template <typename int_t>
v2<int_t>::v2() : x(0), y(0) {}

template <typename int_t>
v2<int_t> v2<int_t>::operator+(const v2<int_t> & other) const
{
    return {x + other.x, y + other.y};
}

template <typename int_t>
v2<int_t> v2<int_t>::operator-(const v2<int_t> & other) const
{
    return {x - other.x, y - other.y};
}

template <typename int_t>
v2<int_t> & v2<int_t>::operator+=(const v2<int_t> & other)
{
    x += other.x; y += other.y;
    return *this;
}

template <typename int_t>
v2<int_t> & v2<int_t>::operator-=(const v2<int_t> & other)
{
    x -= other.x; y -= other.y;
    return *this;
}

template <typename int_t>
v2<int_t> v2<int_t>::operator*(const int_t & value) const
{
    return {x * value, y * value};
}

template <typename int_t>
v2<int_t> v2<int_t>::operator/(const int_t & value) const
{
    return {x / value, y / value};
}

template <typename int_t>
bool v2<int_t>::operator==(const v2<int_t> & other) const
{
    return (x == other.x) && (y == other.y);
}

template <typename int_t>
bool v2<int_t>::operator!=(const v2<int_t> & other) const
{
    return !(*this == other);
}

template <typename int_t>
bool v2<int_t>::operator<(const v2<int_t> & other) const
{
    return (y < other.y) || ((y == other.y) && (x < other.x));
}

template <typename int_t>
int_t v2<int_t>::manhattan() const
{
    return std::abs(x) + std::abs(y);
}

template <typename int_t>
std::ostream & operator<<(std::ostream & out, const v2<int_t> & vector)
{
    out << "<" << vector.x << ", " << vector.y << ">";
    return out;
}

template <typename int_t>
v2<int_t> v2<int_t>::normalized() const
{
    return { sgn<int_t, int_t>(x), sgn<int_t, int_t>(y) };
}

namespace std
{
    template <typename int_t>
    struct hash<v2<int_t>>
    {
        size_t operator()(const v2<int_t> & vec) const
        {
            size_t seed = 2;
            seed ^= vec.x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= vec.y + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

typedef v2<int_fast64_t> vi2;
typedef v2<uint_fast64_t> vu2;
