#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "router.hpp"
#include "types.hpp"
#include "chip.hpp"
#include "graphics.hpp"
#include "CLParser.h"
#include "fileutils.h"
#include "stringutils.h"
#include "routing_problem.hpp"
#include "board_generator.hpp"

grid<char> render(const grid<int> & map);
void print(const grid<char> & grid_render);
void place_pegs(grid<int> & map, const std::vector<vi2> & path);
char merge_tiles(const char & prev, const char new_char);
void generate(const vu2& size, const std::vector<std::vector<vi2>>& paths);

#ifdef _TESTING
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#else
int main(int argc, char * argv[])
{
    if(argc != 2 && argc != 3)
    {
        std::cout << "Usage: router.exe routing_file" << std::endl;
        return -1;
    }
    CLParser cl_args(argc, argv);
    bool best = cl_args.has_option("-b");
    std::string filename = cl_args.get_last();

    routing_problem problem(filename);
    const auto& [width, height] = problem.map.get_size();
    GraphicsOutput graphics(width, height);
    router _router(problem, best);
    _router.on_best_candidate = [&graphics, &problem] (const std::vector<std::vector<vi2>>& candidate)
    {
        graphics.clear();
        graphics.draw_map(problem.map);
        for(const auto& path : candidate)
            graphics.draw_path(path);
        graphics.present();
    };
    auto paths = _router.route();

    generate({width, height}, paths);

    graphics.loop();
    return 0;
}
#endif

grid<char> render(const grid<int> & map)
{
    auto size = map.get_size();
    grid<char> buffer(size.x, size.y);
    for(int_t y = 0; (unsigned)y < size.y; y++)
    {
        for(int_t x = 0; (unsigned)x < size.x; x++)
        {
            vi2 pos{x, y};
            if(map[pos])
                buffer[pos] = '#';
            else
                buffer[pos] = ' ';
        }
    }
    return buffer;
}

void print(const grid<char> & grid_render)
{
    auto size = grid_render.get_size();
    for(int_t y = 0; (unsigned)y < size.y; y++)
    {
        for(int_t x = 0; (unsigned)x < size.x; x++)
        {
            vi2 pos{x, y};
            std::cout << grid_render[pos];
        }
        std::cout << std::endl;
    }
}

void place_pegs(grid<int> & map, const std::vector<vi2> & path)
{
    for(auto pos : path)
        map[pos] = 1;
}

char merge_tiles(const char & prev, const char new_char)
{
    if(new_char == 'o') return new_char;
    else if(prev == ' ') return new_char;
    else if(prev == '/' && new_char == '\\') return 'X';
    else if(prev == '\\' && new_char == '/') return 'X';
    else if(prev == '-' && new_char == '|') return '+';
    else if(prev == '|' && new_char == '-') return '+';
    else return '*';
}

void generate(const vu2& size, const std::vector<std::vector<vi2>>& paths)
{
    std::vector<vi2> pegs;
    std::vector<std::pair<vi2, vi2>> connections;

    for(const auto& path : paths)
    {
        for(const auto& node : path)
            pegs.push_back(node);
        for(uint32_t index = 1; index < path.size(); index++)
            connections.push_back({path[index-1], path[index]});
    }

    const auto& [width, height] = size;
    Board board{width, height, {0., .5, 0.}, {0., 0., 0.}, {0., 0., 0.}};

    std::unordered_map<vi2, Peg*> phys_pegs;
    for(auto peg : pegs)
        phys_pegs[peg] = board.add_peg(peg);
    for(auto [from, to] : connections)
        board.add_wire(phys_pegs[from], phys_pegs[to]);

    BoardGenerator generator;
    generator.generate("test_board.tungboard", board);
}
