#include "router.hpp"

#include <cstdlib>
#include <ctime>
#include <algorithm>

#include "asolver.hpp"
#include "sjt.hpp"
#include "types.hpp"
#include "progress.hpp"

int merge_path(const int & prev, const int & new_value)
{
    if(prev == OBSTRUCTION || new_value == OBSTRUCTION) return OBSTRUCTION;
    else return prev | new_value;
}

std::vector<vi2> simplify(const std::vector<vi2> & path)
{
    if(path.size() <= 2)
        return path; // already simplest form
    std::vector<vi2> simplified = { *path.begin() };
    auto prev_pos = std::next(path.begin(), 1);
    vi2 prev_dir = *prev_pos - *(path.begin());
    for(auto pos = std::next(path.begin(), 2);
            pos != path.end();
            (pos++, prev_pos++))
    {
        vi2 dir = *pos - *prev_pos;
        if(dir != prev_dir)
        {
            simplified.push_back(*prev_pos);
            prev_dir = dir;
        }
    }
    simplified.push_back(*path.rbegin());
    return simplified;
}

uint_t factorial(uint_t val)
{
    uint_t result = 1;
    for(uint_t value = 2; value <= val; value++)
        result *= value;
    return result;
}

uint_t footprint(const std::vector<std::vector<vi2>> & paths)
{
    uint_t min_x = -1, min_y = -1, max_x = 0, max_y = 0;
    for(auto path : paths)
    {
        for(auto peg : path)
        {
            if(peg.x < min_x)
                min_x = peg.x;
            if(peg.y < min_y)
                min_y = peg.y;
            if(peg.x > max_x)
                max_x = peg.x;
            if(peg.y > max_y)
                max_y = peg.y;
        }
    }
    return (max_x - min_x) * (max_y - min_y);
}

uint_t wire_length(const std::vector<std::vector<vi2>> & paths)
{
    uint_t cost = 0;
    for(auto path : paths)
    {
        for(uint_t i = 1; i < path.size(); i++)
        {
            auto diff = path[i] - path[i-1];
            if(!diff.aligned()) cost += 3 * std::abs(diff.x);
            else cost += 2 * diff.manhattan();
        }
    }
    return cost;
}

uint_t node_count(const std::vector<std::vector<vi2>> & paths)
{
    uint_t cost = 0;
    for(auto path : paths)
        cost += path.size();
    return cost;
}

uint_t evaluate(const std::vector<std::vector<vi2>> & paths)
{
    //return footprint(paths) + wire_length(paths);
    return wire_length(paths) + node_count(paths);
}

router::router(routing_problem & problem, bool find_best)
    : map(problem.map), current_map(map),
    connections(problem.to_route),
    permutation(connections.size()),
    prog_bar(factorial(connections.size())),
    find_best(find_best),
    best_score(-1),
    on_best_candidate([](const std::vector<std::vector<vi2>>&) {})
{
}

#define RANDOM_WALK
std::vector<std::vector<vi2>> router::route()
{
    std::srand(std::time(0));
#ifdef RANDOM_WALK
#define _connections connections
#else
    std::vector<connection> _connections;
    _connections.reserve(connections.size());
#endif
    do
    {
#ifdef RANDOM_WALK
        int_t swap_a = std::rand() % connections.size();
        int_t swap_b = std::rand() % connections.size();
        std::iter_swap(connections.begin() + swap_a,
                connections.begin() + swap_b);
#else
        uint_t pos = 0;
        for(auto index : permutation.values_)
        {
            _connections[pos] = connections[index - 1];
            pos++;
        }
#endif
        auto paths = attempt_route(_connections, map);
        if(paths.size() >= connections.size())
        {
            if(!find_best)
            {
                std::cout << std::endl;
                on_best_candidate(paths);
                return paths;
            }
            else
            {
                if(!propose_candidate(paths))
                {
#ifdef RANDOM_WALK
                    std::iter_swap(connections.begin() + swap_a,
                            connections.begin() + swap_b);
#endif
                }
            }
        }
        ++permutation;
#if 0
        prog_bar.increment();
        //if(prog_bar.value % (prog_bar.max / 100) == 0)
        if(prog_bar.value % 100000)
        {
            std::cout << prog_bar;
            if(best_score != -1)
                std::cout << " " << best_score << "        ";
        }
#endif
    }
    while(!permutation.complete());
    std::cout << std::endl;
    return best_candidate;
}

bool router::propose_candidate(const std::vector<std::vector<vi2>>& candidate)
{
    uint_t current_score = evaluate(candidate);
    if(current_score < best_score)
    {
        best_candidate = candidate;
        best_score = current_score;
        on_best_candidate(candidate);
        std::cout << "\r" << best_score << "        " << std::endl;
        return true;
    }
    return false;
}

std::vector<std::vector<vi2>> router::attempt_route(
        const std::vector<connection> & connections,
        const grid<int> & map)
{
    current_map = map;
    std::vector<std::vector<vi2>> paths;
    paths.reserve(connections.size());
    std::unordered_map<vi2, std::vector<vi2>> endpoints;

    for(const auto& [start, end] : connections)
    {
        current_map[start] = OBSTRUCTION;
        current_map[end] = OBSTRUCTION;
    }

    for(const auto& [start, end] : connections)
    {
        current_map[start] = VOID;
        current_map[end] = VOID;

        endpoints[start].push_back(start);

        for(const auto& point : endpoints[start])
            current_map[point] = VOID;
        auto path = find_path(end, endpoints[start], &current_map);
        //auto path = find_path(start, {end}, &current_map);
        if(path.empty())
        {
#ifdef _DEBUG
            std::cout << "Failed to create connection " << paths.size() + 1 << std::endl;
#endif
            return paths;
        }

        auto simplified_path = simplify(path);
        endpoints[start].insert(endpoints[start].end(),
                simplified_path.begin(),
                simplified_path.end());
        paths.push_back(simplified_path);
        for(const auto& point : endpoints[start])
            current_map[point] = OBSTRUCTION;
        current_map.apply_path(simplified_path,
                OBSTRUCTION,
                HOR_WIRE,
                VERT_WIRE,
                SLASH_WIRE,
                BSLASH_WIRE,
                merge_path);
    }
    return paths;
}
