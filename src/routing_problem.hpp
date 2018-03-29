#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <set>

#include "v2.hpp"
#include "grid.hpp"
#include "chip.hpp"
#include "types.hpp"

struct routing_problem
{
    std::unordered_map<std::string, chip_type> types;
    std::unordered_map<std::string, chip> chips;
    std::unordered_map<std::string, std::vector<vi2>> nets;
    std::set<std::pair<int, vi2>> io_pins; // side, position
    grid<int> map;
    std::vector<connection> to_route;

    routing_problem(std::string filename);
};
