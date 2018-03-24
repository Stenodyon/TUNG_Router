/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "stringutils.h"

#include <regex>
#include <cctype>

std::vector<std::string> split(std::string src, std::string delimiter)
{
    std::regex re{ delimiter };
    std::vector<std::string> container{
        std::sregex_token_iterator(src.begin(), src.end(), re, -1),
        std::sregex_token_iterator() };
    return container;
}

void trim_spaces(std::string & str)
{
    str.erase(str.begin(),
        std::find_if(str.begin(), str.end(),
            [](char ch) { return !std::isspace(ch); }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); }).base(),
        str.end());
}

std::string remove_extension(std::string filename)
{
    auto last_index = filename.find_last_of(".");
    return filename.substr(0, last_index);
}
