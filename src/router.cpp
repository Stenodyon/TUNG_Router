#include "router.hpp"
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

uint_t evaluate(const std::vector<std::vector<vi2>> & paths)
{
    return footprint(paths) + wire_length(paths);
    //return wire_length(paths);
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

std::vector<std::vector<vi2>> router::route()
{
    std::vector<connection> _connections(connections.size());
    do
    {
        uint_t pos = 0;
        for(auto index : permutation.values_)
        {
            _connections[pos] = connections[index - 1];
            pos++;
        }
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
                propose_candidate(paths);
            }
        }
        ++permutation;
        prog_bar.increment();
        if(prog_bar.value % (prog_bar.max / 100) == 0)
        {
            std::cout << prog_bar;
            if(best_score != -1)
                std::cout << " " << best_score << "        ";
        }
    }
    while(!permutation.complete());
    std::cout << std::endl;
    return best_candidate;
}

void router::propose_candidate(const std::vector<std::vector<vi2>>& candidate)
{
    uint_t current_score = evaluate(candidate);
    if(current_score < best_score)
    {
        best_candidate = candidate;
        best_score = current_score;
        on_best_candidate(candidate);
    }
}

std::vector<std::vector<vi2>> router::attempt_route(
        const std::vector<connection> & connections,
        const grid<int> & map)
{
    current_map = map;
    std::vector<std::vector<vi2>> paths;

    for(auto conn : connections)
    {
        current_map[conn.first] = OBSTRUCTION;
        current_map[conn.second] = OBSTRUCTION;
    }

    for(auto conn : connections)
    {
        current_map[conn.first] = VOID;
        current_map[conn.second] = VOID;

        auto path = find_path(conn.first, {conn.second}, &current_map);
        if(path.empty())
        {
#ifdef _DEBUG
            std::cout << "Failed to create connection " << paths.size() + 1 << std::endl;
#endif
            return paths;
        }

        auto simplified_path = simplify(path);
        paths.push_back(simplified_path);
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
