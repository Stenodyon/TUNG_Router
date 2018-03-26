#include "board_generator.hpp"

#include <cstring>

static const uint8_t header[] = {
    0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x41,
    0x73, 0x73, 0x65, 0x6D, 0x62, 0x6C, 0x79, 0x2D, 0x43, 0x53, 0x68, 0x61,
    0x72, 0x70
};

uint32_t new_id()
{
    static uint32_t next_id = 1;
    return next_id++;
}

LengthPrefixedString operator "" _lp(const char* str, size_t)
{
    return LengthPrefixedString{str};
}

std::fstream& operator<<(std::fstream& out,
        const LengthPrefixedString& lpstring)
{
    uint8_t length = (uint8_t)lpstring.value.length();
    out.write((char*)&length, 1);
    out.write(lpstring.value.c_str(), length);
    return out;
}

std::fstream& operator<<(std::fstream& out,
        const uint8_t value)
{
    out.write((char*)&value, 1);
    return out;
}

std::fstream& operator<<(std::fstream& out,
        const uint32_t value)
{
    out.write((char*)&value, 4);
    return out;
}

std::fstream& operator<<(std::fstream& out,
        const float value)
{
    out.write((char*)&value, 4);
    return out;
}

std::unordered_map<std::string, uint32_t> Class::ids;

ClassInstance Class::instantiate(std::vector<BTE> btes) const
{
    uint32_t id = new_id();
    uint32_t ref_id = id;
    if(ids.find(name.value) != ids.end())
        ref_id = ids[name.value];
    else
        ids[name.value] = id;

    return ClassInstance{ id, ref_id, this, btes };
}

static const uint8_t verbose_class = 0x05;
static const uint8_t class_ref = 0x01;

std::fstream& operator<<(std::fstream& out,
        const BTE& bte)
{
    if(bte.enum_val == 0x00) // primitive
    {
        out << bte.bte;
    }
    else if(bte.enum_val == 0x04) // class
    {
        out << bte.instance->_class->name;
        out << bte.instance->_class->library_id;
    }
    else
    {
        assert(false);
    }
    return out;
}

std::fstream& operator<<(std::fstream& out,
        const std::vector<BTE> & btes)
{
    for(const auto& bte : btes)
        out << bte.enum_val;
    for(const auto& bte : btes)
        out << bte;
    for(const auto& bte : btes)
    {
        if(bte.enum_val == 0)
        {
            if(bte.bte == 0x08)
                out << bte.int32;
            else if(bte.bte == 0x0B)
                out << bte.float32;
            else
                assert(false);
        }
        else if(bte.enum_val == 0x04)
        {
            out << *bte.instance;
        }
        else
        {
            assert(false);
        }
    }
    return out;
}

std::fstream& operator<<(std::fstream& out,
        const ClassInstance& instance)
{
    if(instance.id == instance.ref_id)
    {
        out << verbose_class;
        out << instance.id;
        out << instance._class->name;
        uint32_t member_count = instance._class->member_names.size();
        out << member_count;
        for(const auto& member_name : instance._class->member_names)
            out << member_name;
        out << instance.member_btes;
        out << instance._class->library_id;
    }
    else
    {
        out << class_ref;
        out << instance.id;
        out << instance.ref_id;
    }
    return out;
}

ClassInstance& ClassInstance::operator=(const ClassInstance& copy)
{
    id = copy.id; ref_id = copy.ref_id;
    _class = copy._class;
    member_btes = copy.member_btes;
    return *this;
}

BTE::BTE(uint32_t int32)
    : enum_val(0), bte(0x08), int32(int32)
{}

BTE::BTE(float float32)
    : enum_val(0), bte(0x0B), float32(float32)
{}

BTE::BTE(ClassInstance* instance)
    : enum_val(0x04), bte(0), instance(instance)
{}

BTE::BTE(const BTE& other)
    : enum_val(other.enum_val), bte(other.bte)
{
    if(enum_val == 0)
    {
        if(bte == 0x08)
            int32 = other.int32;
        else if(bte == 0x0B)
            float32 = other.float32;
        else
            assert(false);
    }
    else if(enum_val == 0x04)
    {
        instance = other.instance;
    }
    else
    {
        assert(false);
    }
}

BTE& BTE::operator=(const BTE& other)
{
    enum_val = other.enum_val;
    bte = other.bte;
    if(enum_val == 0)
    {
        if(bte == 0x08)
            int32 = other.int32;
        else if(bte == 0x0B)
            float32 = other.float32;
        else
            assert(false);
    }
    else if(enum_val == 0x04)
    {
        instance = other.instance;
    }
    else
    {
        assert(false);
    }
    return *this;
}

ClassInstance SerializableColor::serialize()
{
    return Class_Color.instantiate({
            BTE((float)r),
            BTE((float)g),
            BTE((float)b),
            });
}

ClassInstance SerializableVector3::serialize()
{
    return Class_Color.instantiate({
            BTE((float)x),
            BTE((float)y),
            BTE((float)z),
            });
}

void BoardGenerator::generate(const std::string & filename)
{
    std::fstream file(filename, std::ios::binary | std::ios::out);
    if(!file.good())
    {
        std::cerr << "Could not open file " << filename << std::endl;
        exit(-1);
    }
    auto board_color = (SerializableColor{0., .5, 0.}).serialize();
    auto board_local_pos = (SerializableVector3{0., 0., 0.}).serialize();
    auto board_local_angles = (SerializableVector3{0., 0., 0.}).serialize();
    file.write((char*)header, sizeof(header));
    file << Class_Board.instantiate(
            {
                BTE((uint32_t)3),
                BTE((uint32_t)4),
                BTE(&board_color),
                BTE(&board_local_pos),
                BTE(&board_local_angles),
            }
            );
    char end = 0x0B;
    file.write(&end, 1);
}
