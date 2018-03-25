#pragma once

#include <iostream>

#include "types.hpp"

struct progress_bar
{
    uint_t value, max;

    progress_bar(uint_t max) : value(0), max(max) {}

    void increment() { value++; }

    friend std::ostream & operator<<(std::ostream & out, const progress_bar & bar);
};
