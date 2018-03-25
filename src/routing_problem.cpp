#include "routing_problem.hpp"

#include "fileutils.h"
#include "stringutils.h"

int_t get_side(const std::string & name);
void place_chip(grid<int> & map, const chip & ic);

routing_problem::routing_problem(std::string filename) : map(32, 32)
{
    std::string contents = read_file(filename);
    std::vector<std::string> lines = split(contents, "\n");
    uint_t line_count = 1;
    for(std::string line : lines)
    {
        line = line.substr(0, line.find("#"));
        trim_spaces(line);

        std::vector<std::string> components = split(line, "\\s+");
        if(components.size() == 0
                || (components.size() == 1 && components[0] == ""))
            continue;
        if(components[0] == "chip") // chip type definition
        {
            if(components.size() != 4)
            {
                std::cerr << line_count << ": ";
                std::cerr << "parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string name = components[1];
            int width = std::stoi(components[2]);
            int height = std::stoi(components[3]);
            types.insert({name, {width, height}});
        }
        else if(components[0] == "pin")
        {
            if(components.size() != 4)
            {
                std::cerr << line_count << ": parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string type_name = components[1];
            int_t side = get_side(components[2]);
            int position = std::stoi(components[3]);

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
            type.add_pin(side, position);
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
            if(components.size() != 5)
            {
                std::cerr << line_count << ": parse error: '" << line << "'" << std::endl;
                exit(-1);
            }
            std::string name = components[1];
            std::string chip_name = components[2];
            int_t side = get_side(components[3]);
            int pin_number = std::stoi(components[4]);
            if(side == -1)
            {
                std::cerr << line_count << ": ";
                std::cerr << "Unknown side " << components[2] << std::endl;
                exit(-1);
            }
            if(chips.find(chip_name) == chips.end())
            {
                std::cerr << line_count << ": ";
                std::cerr << "Undefined chip " << chip_name << std::endl;
                exit(-1);
            }
            chip & _chip = chips.at(chip_name);
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
            std::cerr << "Error: Nets of more than 2 pins are not supported yet" << std::endl;
            exit(-1);
        }

        if(net.size() == 1)
            std::cout << "Warning: net " << net_name
                << " contains only 1 pin" << std::endl;
        if(net.size() == 2)
            to_route.push_back({net[0], net[1]});
    }
    for(const auto & [chip_name, _chip] : chips)
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
