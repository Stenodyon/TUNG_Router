#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "v2.hpp"
#include "grid.hpp"
#include "chip.hpp"
#include "types.hpp"

struct routing_problem
{
    std::unordered_map<std::string, chip_type> types;
    std::unordered_map<std::string, chip> chips;
    std::unordered_map<std::string, std::vector<vi2>> nets;
    grid<int> map;
    std::vector<connection> to_route;

    routing_problem(std::string filename);
};
