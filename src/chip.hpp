#pragma once

#include <vector>
#include <unordered_map>

#include "v2.hpp"
#include "types.hpp"

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

struct chip_type
{
    uint_t width, height;
    std::vector<uint_t> pins[4];
    std::unordered_map<std::string, vu2> pin_labels;

    chip_type(uint_t width, uint_t height);

    void add_pin(uint_t side, uint_t pos, std::string label);
    bool has_pin_label(const std::string & label) const;
    vu2 get_pin_by_label(const std::string & label) const;
    vi2 get_pin_offset(uint_t side, uint_t pin_number) const;
};

struct chip
{
    vi2 pos;
    const chip_type & type;

    chip(vi2 pos, const chip_type & type);
    vi2 get_pin_pos(uint_t side, uint_t pin_number) const;
};
