#include "board_generator.hpp"

#include <cstring>

static const uint8_t header[] = {
    0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x41,
    0x73, 0x73, 0x65, 0x6D, 0x62, 0x6C, 0x79, 0x2D, 0x43, 0x53, 0x68, 0x61,
    0x72, 0x70
};

uint32_t next_id()
{
    static uint32_t next_id = 1;
    return next_id++;
}

LengthPrefixedString operator "" _lp(const char* str, size_t)
{
    return LengthPrefixedString{str};
}

LengthPrefixedString LengthPrefixedString::operator+(const std::string & append)
{
    std::string new_value = value + append;
    return LengthPrefixedString{new_value};
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

stream& ClassTypeInfo::serialize(stream& out) const
{
    out << name;
    out << library_id;
    return out;
}

stream& operator<<(stream& out, const ClassTypeInfo& class_type_info)
{
    return class_type_info.serialize(out);
}

stream& ClassInfo::serialize(stream& out) const
{
    out << id;
    out << name;
    out << (uint32_t)(members.size());
    for(const auto& member : members)
        out << member;

    return out;
}

stream& operator<<(stream& out, const ClassInfo& class_info)
{
    return class_info.serialize(out);
}

stream& operator<<(stream& out,
        const AdditionalInfo& additional_info)
{
    return additional_info.serialize(out);
}

stream& MemberTypeInfo::serialize(stream& out) const
{
    for(const auto& type : type_enums)
        out << type;
    for(const auto& additional_info : additional_infos)
        out << *additional_info;
    return out;
}

stream& operator<<(stream& out,
        const MemberTypeInfo& member_type_info)
{
    return member_type_info.serialize(out);
}

stream& ClassWithMembersAndTypes::serialize(stream& out) const
{
    out << (uint8_t)0x05;
    out << class_info;
    out << member_type_info;
    out << library_id;
    return out;
}

stream& operator<<(stream& out,
        const ClassWithMembersAndTypes& class_with)
{
    return class_with.serialize(out);
}

ClassTypeInfo Class::class_type_info()
{
    return ClassTypeInfo{ name, library_id };
}

ClassInfo Class::class_info(uint32_t id)
{
    return ClassInfo{ id, name, members };
}

std::shared_ptr<AdditionalInfo> Class::additional_info()
{
    return std::shared_ptr<AdditionalInfo>(
            new AdditionalInfo_impl<ClassTypeInfo>(class_type_info()));
}

ClassWithMembersAndTypes Class::class_mem_types(uint32_t id)
{
    return ClassWithMembersAndTypes{
        class_info(id),
        member_type_info,
        library_id
    };
}

BinaryArray Class::binary_array(uint32_t id)
{
    return BinaryArray{ id, 0, this };
}

std::shared_ptr<AdditionalInfo> BinaryArray::additional_info()
{
    ClassTypeInfo class_type_info
        = static_cast<AdditionalInfo_impl<ClassTypeInfo>*>(
                _class->additional_info().get())->value;
    return std::shared_ptr<AdditionalInfo>(
            new AdditionalInfo_impl<ClassTypeInfo>(
                ClassTypeInfo{class_type_info.name + "[]",
                    class_type_info.library_id}));
}

stream& BinaryArray::serialize(stream& out) const
{
    out << (uint8_t)0x07;
    out << id;
    out << (uint8_t)0x00;
    out << (uint32_t)0x01;
    out << length;
    out << (uint8_t)0x04;
    out << *(_class->additional_info());
    return out;
}

stream& operator<<(stream& out, const BinaryArray& binary_array)
{
    return binary_array.serialize(out);
}

SerializableVector3::SerializableVector3(float _x, float _y, float _z)
    : x(_x), y(_y), z(_z),
    ClassObject<serialized_vector>({&x, &y, &z})
{
}

SerializableColor::SerializableColor(float _r, float _g, float _b)
    : r(_r), g(_g), b(_b),
    ClassObject<serialized_color>({&r, &g, &b})
{
}

Board::Board(uint32_t _x, uint32_t _z, SerializableColor _color,
        SerializableVector3 _local_pos, SerializableVector3 _local_angles)
    : x(_x), z(_z),
    color(_color), local_pos(_local_pos), local_angles(_local_angles),
    ClassObject<board_class>(
            {&x, &z, &color, &local_pos, &local_angles, &children})
{
}

stream& operator<<(stream& out, const Object& object)
{
    return object.serialize(out);
}

std::shared_ptr<AdditionalInfo> INT(new AdditionalInfo_impl<uint8_t>(0x08));
std::shared_ptr<AdditionalInfo> FLOAT(new AdditionalInfo_impl<uint8_t>(0x0B));

Class saved_object { "SavedObjects.SavedObjectV2"_lp,
    {}, 0x00000002, {}
};

Class serialized_vector { "SerializableVector3"_lp,
    { "x"_lp, "y"_lp, "z"_lp },
    0x00000002,
    {
        { 0x00, 0x00, 0x00 },
        { FLOAT, FLOAT, FLOAT }
    }
};

Class serialized_color { "SerializableColor"_lp,
    { "r"_lp, "g"_lp, "b"_lp },
    0x00000002,
    {
        { 0x00, 0x00, 0x00 },
        { FLOAT, FLOAT, FLOAT }
    }
};

Class board_class { "SavedObjects.SavedCircuitBoard"_lp,
    { "x"_lp, "z"_lp,
        "color"_lp, "LocalPosition"_lp, "LocalEulerAngles"_lp,
        "Children"_lp
    },
    0x00000002,
    {
        { 0x00, 0x00,
            0x04, 0x04, 0x04, 0x04
        },
        { INT, INT,
            serialized_color.additional_info(),
            serialized_vector.additional_info(),
            serialized_vector.additional_info(),
            saved_object.binary_array(0).additional_info() // Eww
        }
    }
};

void BoardGenerator::generate(const std::string & filename)
{
    std::fstream file(filename, std::ios::binary | std::ios::out);
    if(!file.good())
    {
        std::cerr << "Could not open file " << filename << std::endl;
        exit(-1);
    }
    file.write((char*)header, sizeof(header));
    SerializableColor color{0., 0.5, 0.};
    SerializableVector3 pos{0.0, 0.0, 0.0};
    SerializableVector3 angles{0.0, 0.0, 0.0};
    file << Board{10, 15, color, pos, angles};
    file << (uint8_t)0x0B;
}
