#include "routing_problem.hpp"

#include <optional>

#include "fileutils.h"
#include "stringutils.h"

int_t get_side(const std::string & name);
void place_chip(grid<int> & map, const chip & ic);

routing_problem::routing_problem(std::string filename) : map(32, 32)
{
    std::string contents = read_file(filename);
    std::vector<std::string> lines = split(contents, "\n");
    uint_t line_count = 1;
    std::optional<vi2> board_size;
    for(std::string line : lines)
    {
        line = line.substr(0, line.find("#"));
        trim_spaces(line);

        std::vector<std::string> components = split(line, "\\s+");
        if(components.size() == 0
                || (components.size() == 1 && components[0] == ""))
            continue;
        if(components[0] == "board") // board size
        {
            if(components.size() != 3)
            {
                std::cerr << line_count << ": ";
                std::cerr << "parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            int width = std::stoi(components[1]);
            int height = std::stoi(components[2]);
            board_size = {width, height};
            map = grid<int>(width, height);
        }
        else if(!board_size)
        {
            std::cerr << line_count << ": ";
            std::cerr << "Undefined board size" << std::endl;
            exit(-1);
        }
        else if(components[0] == "chip") // chip type definition
        {
            if(components.size() != 4)
            {
                std::cerr << line_count << ": ";
                std::cerr << "parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string name = components[1];
            uint_t width = (uint_t)std::stoi(components[2]);
            uint_t height = (uint_t)std::stoi(components[3]);
            types.insert({name, {width, height}});
        }
        else if(components[0] == "pin")
        {
            if(components.size() != 5)
            {
                std::cerr << line_count << ": parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string type_name = components[1];
            int_t side = get_side(components[2]);
            int position = std::stoi(components[3]);
            std::string label = components[4];

            if(side == -1)
            {
                std::cerr << line_count << ": ";
                std::cerr << "Unknown side " << components[2] << std::endl;
                exit(-1);
            }
            if(types.find(type_name) == types.end())
            {
                std::cerr << line_count << ": ";
                std::cerr << "Undefined type " << type_name << std::endl;
                exit(-1);
            }
            chip_type & type = types.at(type_name);
            type.add_pin(side, position, label);
        }
        else if(components[0] == "instance")
        {
            if(components.size() != 5)
            {
                std::cerr << line_count << ": parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string name = components[1];
            std::string type_name = components[2];
            int x_pos = std::stoi(components[3]);
            int y_pos = std::stoi(components[4]);
            if(types.find(type_name) == types.end())
            {
                std::cerr << line_count << ": ";
                std::cerr << "Undefined type " << type_name << std::endl;
                exit(-1);
            }
            chip_type & type = types.at(type_name);
            chips.insert({name, {{x_pos, y_pos}, type}});
        }
        else if(components[0] == "net")
        {
            if(components.size() != 4)
            {
                std::cerr << line_count << ": parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string name = components[1];
            std::string chip_name = components[2];
            std::string pin_label = components[3];
            if(chips.find(chip_name) == chips.end())
            {
                std::cerr << line_count << ": ";
                std::cerr << "Undefined chip " << chip_name << std::endl;
                exit(-1);
            }
            chip & _chip = chips.at(chip_name);
            if(!_chip.type.has_pin_label(pin_label))
            {
                std::cerr << line_count << ": ";
                std::cerr << "Chip " << chip_name << " has no pin labelled "
                    << pin_label << std::endl;
                exit(-1);
            }
            const auto& [side, pin_number] = _chip.type.get_pin_by_label(pin_label);
            auto pin_pos = _chip.get_pin_pos(side, pin_number);
            nets[name].push_back(pin_pos);
        }
        else
        {
            std::cerr << line_count << ": parse error: '" << line << "'" << std::endl;
            exit(-1);
        }
        line_count++;
    }
    map.fill(0);
    for(const auto & [net_name, net] : nets)
    {
        if(net.size() > 2)
        {
            const auto& key = net[0];
            for(auto pin = std::next(net.begin(), 1); pin != net.end(); pin++)
                to_route.push_back({key, *pin});
        }

        else if(net.size() == 1)
            std::cout << "Warning: net " << net_name
                << " contains only 1 pin" << std::endl;
        else if(net.size() == 2)
            to_route.push_back({net[0], net[1]});
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    for(const auto & [chip_name, _chip] : chips)
#pragma GCC diagnostic pop
        place_chip(map, _chip);
}

int_t get_side(const std::string & name)
{
    if(name == "n")
        return NORTH;
    else if(name == "s")
        return SOUTH;
    else if(name == "e")
        return EAST;
    else if(name == "w")
        return WEST;
    else
        return -1;
}

void place_chip(grid<int> & map, const chip & ic)
{
    map.fill(ic.pos, {(signed)ic.type.width, (signed)ic.type.height}, 1);
}
