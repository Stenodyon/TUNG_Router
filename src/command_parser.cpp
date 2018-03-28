#include "command_parser.hpp"

#include "stringutils.h"

template <>
std::string parser::parse_value<std::string>(const std::string& in)
{
    return in;
}

template <>
int32_t parser::parse_value<int32_t>(const std::string& in)
{
    try {
        return std::stoi(in);
    }
    catch(std::invalid_argument)
    {
        std::ostringstream sstream;
        sstream << "'" << in << "' is not an integer";
        throw std::invalid_argument(sstream.str());
    }
}

CommandParser::~CommandParser()
{
    for(auto& [name, pointer] : commands)
        delete pointer;
}

void CommandParser::parse(const std::string& contents)
{
    std::vector<std::string> lines = split(contents, "\n");
    size_t line_count = 1;
    for(std::string line : lines)
    {
        line = line.substr(0, line.find("#"));
        trim_spaces(line);
        std::vector<std::string> components = split(line, "\\s+");

        if(components.size() == 0
                || (components.size() == 1 && components[0] == ""))
            continue;
        const std::string& command_name = components[0];
        if(commands.find(command_name) == commands.end())
        {
            std::ostringstream sstream;
            sstream << line_count << ": ";
            sstream << "parse error: '" << line << "'" << std::endl;
            throw parse_error(sstream.str());
        }
        CommandBase * command = commands[command_name];

        try
        {
            if(!command->parse(components))
            {
                std::ostringstream sstream;
                sstream << line_count << ": ";
                sstream << "parse error: '" << line << "'" << std::endl;
                throw parse_error(sstream.str());
            }
        }
        catch(std::invalid_argument& e)
        {
            std::ostringstream sstream;
            sstream << line_count << ": ";
            sstream << "parse error: '" << line << "'" << std::endl;
            sstream << e.what() << std::endl;
            throw parse_error(sstream.str());
        }

        line_count++;
    }
}

