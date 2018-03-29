#include "chip.hpp"

#include <algorithm>
#include <cassert>

#include "assert.hpp"

chip_type::chip_type(uint_t width, uint_t height)
    : width(width), height(height)
{}

void chip_type::add_pin(uint_t side, uint_t pos, std::string label)
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
    pin_labels.insert({label, {side, pos}});
}

vi2 chip_type::get_pin_offset(uint_t side, uint_t pin_number) const
{
#if 0
    if(pin_number >= pins[side].size())
    {
        std::cerr << "Error trying to access pin " << pin_number
            << " on side " << side << " (only " << pins[side].size()
            << " pins on this side)" << std::endl;
        exit(-1);
    }
#endif
    ASSERT(side, 4, <);
    ASSERT(pin_number, pins[side].size(), <);
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

bool chip_type::has_pin_label(const std::string & label) const
{
    return pin_labels.find(label) != pin_labels.end();
}

vu2 chip_type::get_pin_by_label(const std::string & label) const
{
    return pin_labels.at(label);
}

bool chip_type::has_board(int side) const
{
    return pins[side].size() > 0;
}

chip::chip(vi2 pos, const chip_type & type)
    : pos(pos), type(type)
{}

vi2 chip::get_pin_pos(uint_t side, uint_t pin_number) const
{
    ASSERT(side, 4, <);
    vi2 pin_offset;
    switch(side)
    {
        case NORTH:
            ASSERT(pin_number, type.width, <);
            pin_offset = vi2{(signed)pin_number, -1};
            break;
        case SOUTH:
            ASSERT(pin_number, type.width, <);
            pin_offset = vi2{(signed)pin_number, (signed)type.height};
            break;
        case EAST:
            ASSERT(pin_number, type.height, <);
            pin_offset = vi2{(signed)type.width, (signed)pin_number};
            break;
        case WEST:
            ASSERT(pin_number, type.height, <);
            pin_offset = vi2{-1, (signed)pin_number};
            break;
        default:
            assert(false);
    }
    //const auto pin_pos = pos + type.get_pin_offset(side, pin_number); 
    const auto pin_pos = pos + pin_offset;
    ASSERT(pin_pos.x, 0, >=);
    ASSERT(pin_pos.y, 0, >=);
    return pin_pos;
}


bool chip::has_board(int side) const
{
    return type.has_board(side);
}

vi2 chip::get_io_board_pos(int side) const
{
    switch(side)
    {
        case NORTH:
            return pos + vi2{0, -1};
        case SOUTH:
            return pos + vi2{0, type.height};
        case EAST:
            return pos + vi2{type.width, 0};
        case WEST:
            return pos + vi2{-1, 0};
    }
}

vi2 chip::get_io_board_size(int side) const
{
    switch(side)
    {
        case NORTH:
        case SOUTH:
            return {type.width, 1};
        case EAST:
        case WEST:
            return {1, type.height};
    }
}
