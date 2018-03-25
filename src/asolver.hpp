#pragma once

#include <vector>

#include "grid.hpp"
#include "types.hpp"

#define TURN_COST 20

#define VOID 0
#define OBSTRUCTION 1
#define HOR_WIRE 2
#define VERT_WIRE 4
#define SLASH_WIRE 8
#define BSLASH_WIRE 16
#define DIR_MASK 0b11110

std::vector<vi2> find_path(const vi2 & start,
        const std::vector<vi2> & ends,
        grid<int> * map);
