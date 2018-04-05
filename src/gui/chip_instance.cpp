#include "chip_instance.hpp"

ChipInstance::ChipInstance(const ChipInstance & other)
    : chip_name(other.chip_name),
    instance_name(other.instance_name),
    chip(other.chip),
    pos(other.pos)
{
}

ChipInstance::ChipInstance(std::string chip_name, std::string instance_name,
        chip_type * chip)
    : chip_name(chip_name), instance_name(instance_name), chip(chip)
{
}
