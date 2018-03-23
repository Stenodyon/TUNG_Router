#include <iostream>
#include <set>

#include "router.hpp"
#include "types.hpp"
#include "chip.hpp"

grid<char> render(const grid<int> & map);
void print(const grid<char> & grid_render);
void place_pegs(grid<int> & map, const std::vector<vi2> & path);
void place_chip(grid<int> & map, const chip & ic);

int main()
{
    grid<int> map(32, 32); map.fill(0);
    chip_type type1(3, 8);
    type1.add_pin(EAST, 0);
    type1.add_pin(EAST, 2);
    type1.add_pin(EAST, 4);
    chip testchip({9, 9}, type1);
    chip_type type2(3, 8);
    type2.add_pin(WEST, 0);
    type2.add_pin(WEST, 2);
    type2.add_pin(WEST, 4);
    chip testchip2({23, 10}, type2);
    place_chip(map, testchip);
    place_chip(map, testchip2);

    auto out = render(map);
    auto paths = route({
            {testchip.get_pin_pos(EAST, 0), testchip2.get_pin_pos(WEST, 0)},
            {testchip.get_pin_pos(EAST, 1), testchip2.get_pin_pos(WEST, 1)},
            {testchip.get_pin_pos(EAST, 2), testchip2.get_pin_pos(WEST, 2)},
            }, map);
    for(auto path : paths)
        out.apply_path(path, 'o', '-', '|', '+');
    print(out);
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

void place_chip(grid<int> & map, const chip & ic)
{
    map.fill(ic.pos, {(signed)ic.type.width, (signed)ic.type.height}, 1);
}
