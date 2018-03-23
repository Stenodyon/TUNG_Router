#include "router.hpp"
#include "asolver.hpp"
#include "sjt.hpp"
#include "types.hpp"
#include "progress.hpp"

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

std::vector<std::vector<vi2>> route(
        const std::vector<connection> connections,
        const grid<int> & map,
        bool find_best)
{
    permutation_iterator permutation(connections.size());
    uint_t max_attempts = factorial(connections.size());
    progress_bar prog(max_attempts);
    std::vector<std::vector<vi2>> best_candidate;
    uint_t best_footprint = -1;
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
                return paths;
            uint_t current_footprint = footprint(paths);
            if(current_footprint < best_footprint)
            {
                best_candidate = paths;
                best_footprint = current_footprint;
            }
        }
        ++permutation;
        prog.increment();
        if(prog.value % (prog.max / 100) == 0)
        {
            std::cout << prog;
            if(best_footprint != -1)
                std::cout << " " << best_footprint << "        ";
        }
    }
    while(!permutation.complete());
    return best_candidate;
}

std::vector<std::vector<vi2>> attempt_route(
        const std::vector<connection> connections,
        const grid<int> & _map)
{
    grid<int> map(_map);
    std::vector<std::vector<vi2>> paths;

    for(auto conn : connections)
    {
        map[conn.first] = OBSTRUCTION;
        map[conn.second] = OBSTRUCTION;
    }

    for(auto conn : connections)
    {
        map[conn.first] = VOID;
        map[conn.second] = VOID;

        auto path = find_path(conn.first, {conn.second}, &map);
        if(path.empty())
        {
#ifdef _DEBUG
            std::cout << "Failed to create connection " << paths.size() + 1 << std::endl;
#endif
            return paths;
        }

        auto simplified_path = simplify(path);
        paths.push_back(simplified_path);
        map.apply_path(simplified_path,
                OBSTRUCTION,
                HOR_WIRE,
                VERT_WIRE,
                OBSTRUCTION);
    }

    return paths;
}
