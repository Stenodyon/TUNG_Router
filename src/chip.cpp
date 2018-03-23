#include "chip.hpp"

#include <algorithm>
#include <cassert>

chip_type::chip_type(uint_t width, uint_t height)
    : width(width), height(height)
{}

void chip_type::add_pin(uint_t side, uint_t pos)
{
#ifdef _DEBUG
    if(side == NORTH || side == SOUTH)
        ASSERT(pos, width, <);
    if(side == EAST || side == WEST)
        ASSERT(pos, height, <);
#endif
    std::vector<uint_t> & _pins = pins[side];
    auto it_pos = std::lower_bound(
            _pins.begin(), _pins.end(), pos,
            std::greater<uint_t>());
    _pins.insert(it_pos, pos);
}

vi2 chip_type::get_pin_offset(uint_t side, uint_t pin_number) const
{
    switch(side)
    {
        case NORTH:
            return vi2{(signed)pins[NORTH][pin_number], -1};
        case SOUTH:
            return vi2{(signed)pins[SOUTH][pin_number], (signed)height};
        case EAST:
            return vi2{(signed)width, (signed)pins[EAST][pin_number]};
        case WEST:
            return vi2{-1, (signed)pins[WEST][pin_number]};
        default:
            assert(false);
    }
    return {-1, -1};
}

chip::chip(vi2 pos, const chip_type & type)
    : pos(pos), type(type)
{}

vi2 chip::get_pin_pos(uint_t side, uint_t pin_number) const
{
    return pos + type.get_pin_offset(side, pin_number);
}
