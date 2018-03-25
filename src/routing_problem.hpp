#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "v2.hpp"
#include "grid.hpp"
#include "chip.hpp"
#include "router.hpp"

class routing_problem
{
    private:
        std::unordered_map<std::string, chip_type> types;
        std::unordered_map<std::string, chip> chips;
        std::unordered_map<std::string, std::vector<vi2>> nets;
        grid<int> map;
        std::vector<connection> to_route;

    public:
        routing_problem(std::string filename);

        const grid<int> & get_map() const { return map; }
        std::vector<std::vector<vi2>> route(bool find_best = false);
};
