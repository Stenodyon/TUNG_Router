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

grid<char> render(const grid<int> & map);
void print(const grid<char> & grid_render);
void place_pegs(grid<int> & map, const std::vector<vi2> & path);
char merge_tiles(const char & prev, const char new_char);

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
    auto paths = problem.route(best);

    GraphicsOutput graphics;
    graphics.draw_map(problem.get_map());
    for(const auto& path : paths)
        graphics.draw_path(path);
    graphics.loop();
    return 0;
}

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
