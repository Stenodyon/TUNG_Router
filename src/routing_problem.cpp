#include "routing_problem.hpp"

#include <optional>

#include "fileutils.h"
#include "stringutils.h"
#include "command_parser.hpp"

int_t get_side(const std::string & name);
void place_chip(grid<int> & map, const chip & ic);

routing_problem::routing_problem(std::string filename) : map(32, 32)
{
    // TODO: Move all those actions as class methods of routing_problem
    // TODO: Custom parser for the side (+create Side enum)
    std::optional<vi2> board_size;
    parser::CommandParser parser;
    std::function<void(void)> precondition =
        [&board_size]() {
            bool value = (bool)board_size;
            if(!value)
                throw std::invalid_argument("Board size not set (board width height)");
        };
    std::function<void(int,int)> board_action =
        [&board_size, this]
        (int width, int height)
        {
            board_size = {width, height};
            this->map = grid<int>(width, height);
        };
    std::function<void(std::string, int, int)> chip_action =
        [&precondition, this]
        (std::string name, int width, int height)
        {
            precondition();
            this->types.insert({name, {(uint_t)width, (uint_t)height}});
        };
    std::function<void(std::string, std::string, int, std::string)>
        pin_action =
        [&precondition, this](std::string type_name, std::string _side,
                int offset, std::string label)
        {
            precondition();
            int_t side = get_side(_side);
            if(side == -1)
            {
                std::ostringstream sstream;
                sstream << "Unknown side: '" << _side << "'";
                throw std::invalid_argument(sstream.str());
            }
            if(this->types.find(type_name) == this->types.end())
            {
                std::ostringstream sstream;
                sstream << "Undefined type: '" << type_name << "'";
                throw std::invalid_argument(sstream.str());
            }
            chip_type & type = this->types.at(type_name);
            type.add_pin(side, offset, label);
        };
    std::function<void(std::string, int, int, std::string)> instance_action =
        [&precondition, this]
        (std::string type_name, int x_pos, int y_pos, std::string name)
        {
            precondition();
            if(this->types.find(type_name) == this->types.end())
            {
                std::ostringstream sstream;
                sstream << "Undefined type: '" << type_name << "'";
                throw std::invalid_argument(sstream.str());
            }
            chip_type & type = this->types.at(type_name);
            this->chips.insert({name, {{x_pos, y_pos}, type}});
        };
    std::function<void(std::string, std::string, std::string)> net_action =
        [&precondition, this]
        (std::string name, std::string chip_name, std::string pin_label)
        {
            precondition();
            if(this->chips.find(chip_name) == this->chips.end())
            {
                std::ostringstream sstream;
                sstream << "Undefined chip: '" << chip_name << "'";
                throw std::invalid_argument(sstream.str());
            }
            chip & _chip = chips.at(chip_name);
            if(!_chip.type.has_pin_label(pin_label))
            {
                std::ostringstream sstream;
                sstream << "Chip '" << chip_name << "' has no pin labelled '"
                    << pin_label << "'";
                throw std::invalid_argument(sstream.str());
            }
            const auto& [side, pin_number] = _chip.type.get_pin_by_label(pin_label);
            auto pin_pos = _chip.get_pin_pos(side, pin_number);
            this->nets[name].push_back(pin_pos);
            io_pins.insert({side, pin_pos});
        };
    parser.add_command("board", board_action);
    parser.add_command("chip", chip_action);
    parser.add_command("pin", pin_action);
    parser.add_command("instance", instance_action);
    parser.add_command("net", net_action);

    std::string contents = read_file(filename);
    try {
        parser.parse(contents);
    } catch(parser::parse_error& error) {
        std::cerr << error.what();
        exit(-1);
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
