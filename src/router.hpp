#pragma once

#include <utility>
#include <vector>
#include <functional>

#include "v2.hpp"
#include "grid.hpp"
#include "sjt.hpp"
#include "progress.hpp"
#include "routing_problem.hpp"
#include "types.hpp"

std::vector<vi2> simplify(const std::vector<vi2> & path);

class router
{
    private:
        const grid<int> map;
        grid<int> current_map;
        std::vector<connection> connections;
        permutation_iterator permutation;
        progress_bar prog_bar;
        bool find_best;

        uint_t best_score;
        std::vector<std::vector<vi2>> best_candidate;

        void propose_candidate(const std::vector<std::vector<vi2>>& candidate);
        std::vector<std::vector<vi2>> attempt_route(
                const std::vector<connection> & connections,
                const grid<int> & _map);
    public:
        router(routing_problem &problem, bool find_best);

        std::vector<std::vector<vi2>> route();

        std::function<void(const std::vector<std::vector<vi2>>&)>
            on_best_candidate;
};
