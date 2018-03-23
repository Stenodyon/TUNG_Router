#pragma once

#include <utility>
#include <vector>

#include "v2.hpp"
#include "grid.hpp"

typedef std::pair<vi2, vi2> connection;

std::vector<vi2> simplify(const std::vector<vi2> & path);
std::vector<std::vector<vi2>> route(
        const std::vector<connection> connections,
        const grid<int> & map);
