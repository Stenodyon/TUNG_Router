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

#include "fileutils.h"

#include <iostream>

std::ifstream::pos_type file_size(const std::string& filename)
{
    std::streampos fsize = 0;
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cerr << "File '" << filename << "' does not exist" << std::endl;
        exit(-1);
    }

    fsize = file.tellg();
    file.seekg(0, std::ios::end);
    fsize = file.tellg() - fsize;
    file.close();

    return fsize;
}

std::string read_file(std::string filename)
{
    std::ifstream stream(filename);
    std::string contents;

    stream.seekg(0, std::ios::end);
    contents.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);

    contents.assign((std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    return contents;
}

void write_bin(uint8_t * data, size_t size, const std::string & filename)
{
    std::fstream file(filename, std::ios::out | std::ios::binary);
    file.write((char*)data, size);
}

uint8_t * read_bin(const std::string & filename, size_t & size)
{
    std::fstream file(filename, std::ios::in | std::ios::binary);
    if(!file.good())
        return nullptr;

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);
    uint8_t * data = (uint8_t*)malloc(size * sizeof(uint8_t));
    if(data == nullptr)
        return nullptr;

    file.read((char*)data, size);
    return data;
}
