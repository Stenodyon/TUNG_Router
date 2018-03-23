#include "router.hpp"
#include "asolver.hpp"

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

std::vector<std::vector<vi2>> route(
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
            std::cout << "Failed to create connection " << paths.size() << std::endl;
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
