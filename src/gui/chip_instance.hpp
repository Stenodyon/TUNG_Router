#pragma once

#include <string>

#include "../chip.hpp"
#include "../v2.hpp"

struct ChipInstance
{
    public:
        std::string chip_name, instance_name;
        chip_type * chip;

        vi2 pos;

        ChipInstance() : ChipInstance("", "", nullptr) {}
        ChipInstance(const ChipInstance & other);
        ChipInstance(std::string chip_name, std::string instance_name,
                chip_type * chip);

    private:
};
