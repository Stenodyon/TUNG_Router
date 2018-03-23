#pragma once

#include <vector>

#include "grid.hpp"
#include "types.hpp"

#define TURN_COST 20

#define VOID 0
#define OBSTRUCTION 1
#define HOR_WIRE 2
#define VERT_WIRE 3

std::vector<vi2> find_path(vi2 start, std::vector<vi2> ends, grid<int> * map);
