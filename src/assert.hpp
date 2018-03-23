#pragma once

#include <cassert>
#include <iostream>

#define ASSERT(a, b, op) \
    if( !(a op b) ) \
    { \
        std::cerr << "# ---- #" << std::endl; \
        std::cerr << a << " " #op " " << b << std::endl; \
        assert(a op b); \
    }
