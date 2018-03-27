#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <memory>

#include "v2.hpp"

using stream = std::fstream;

struct LengthPrefixedString
{
    const std::string value;

    LengthPrefixedString operator+(const std::string & append);

    friend std::fstream& operator<<(std::fstream& out,
            const LengthPrefixedString& lpstring);
};

std::fstream& operator<<(std::fstream& out, const uint8_t value);
std::fstream& operator<<(std::fstream& out, const uint32_t value);
std::fstream& operator<<(std::fstream& out, const float value);

LengthPrefixedString operator "" _lp(const char* str, size_t length);

uint32_t next_id();

struct ClassTypeInfo
{
    LengthPrefixedString name;
    uint32_t library_id;

    stream& serialize(stream& out) const;
};

struct ClassInfo
{
    uint32_t id;
    LengthPrefixedString name;
    std::vector<LengthPrefixedString> members;

    stream& serialize(stream& out) const;
};

struct AdditionalInfo
{
    private:
        virtual stream& serialize(stream& out) const = 0;
    public:
        friend stream& operator<<(stream& out,
                const AdditionalInfo& additional_info);
};

template <typename T>
struct AdditionalInfo_impl : public AdditionalInfo
{
    public:
        T value;

        AdditionalInfo_impl(T value) : value(value) {}

    private:
        stream& serialize(stream& out) const override;
};

template <typename T>
stream& AdditionalInfo_impl<T>::serialize(stream& out) const
{
    out << value;
    return out;
}

struct MemberTypeInfo
{
    std::vector<uint8_t> type_enums;
    std::vector<std::shared_ptr<AdditionalInfo>> additional_infos;

    stream& serialize(stream& out) const;
};

struct ClassWithMembersAndTypes
{
    ClassInfo class_info;
    MemberTypeInfo member_type_info;
    uint32_t library_id;

    stream& serialize(stream& out) const;
};

struct Class;

struct BinaryArray
{
    uint32_t id;
    uint32_t length;
    Class* _class;

    std::shared_ptr<AdditionalInfo> additional_info();

    stream& serialize(stream& out) const;
};

struct Class
{
    LengthPrefixedString name;
    std::vector<LengthPrefixedString> members;
    uint32_t library_id;
    MemberTypeInfo member_type_info;

    ClassTypeInfo class_type_info();
    ClassInfo class_info(uint32_t id);
    std::shared_ptr<AdditionalInfo> additional_info();
    ClassWithMembersAndTypes class_mem_types(uint32_t id);
    BinaryArray binary_array(uint32_t id);
};

struct Object
{
    virtual stream& serialize(stream& out) const = 0;
};

template <typename T>
struct Primitive : public Object
{
    T value;

    Primitive(T value) : value(value) {}

    stream& serialize(stream& out) const override
    {
        return out << value;
    }
};

stream& operator<<(stream& out, const ClassWithMembersAndTypes& class_with);
stream& operator<<(stream& out, const Object& object);

template <Class& _class>
struct ClassObject : public Object
{
    inline static uint32_t first_id = 0;
    uint32_t id;
    std::vector<Object*> values;

    ClassObject(std::vector<Object*> values)
        : values(values)
    {
        id = next_id();
        if(first_id == 0)
            first_id = id;
    }

    stream& serialize(stream& out) const override
    {
        if(id == first_id)
            out << _class.class_mem_types(id);
        else
            out << (uint8_t)0x01 << id << first_id;
        for(const auto& value : values)
            out << *value;
        return out;
    }
};

stream& operator<<(stream& out, const BinaryArray& binary_array);

template <Class& _class>
struct ArrayObject : public Object, public std::vector<Object*>
{
    uint32_t id;

    ArrayObject()
    {
        id = next_id();
    }

    stream& serialize(stream& out) const override
    {
        BinaryArray binary_array = _class.binary_array(id);
        binary_array.length = size();
        out << binary_array;
        for(const auto& value : *this)
            out << *value;
        return out;
    }
};

extern std::shared_ptr<AdditionalInfo> INT;
extern std::shared_ptr<AdditionalInfo> FLOAT;

extern Class saved_object;
extern Class serialized_vector;
extern Class serialized_color;
extern Class board_class;

struct SerializableVector3 : public ClassObject<serialized_vector>
{
    Primitive<float> x, y, z;

    SerializableVector3(float x, float y, float z);
};

struct SerializableColor : public ClassObject<serialized_color>
{
    Primitive<float> r, g, b;

    SerializableColor(float r, float g, float b);
};

struct Board : public ClassObject<board_class>
{
    Primitive<uint32_t> x, z;
    SerializableColor color;
    SerializableVector3 local_pos, local_angles;
    ArrayObject<saved_object> children;

    Board(uint32_t x, uint32_t z, SerializableColor color,
            SerializableVector3 local_pos, SerializableVector3 local_angles);
};

class BoardGenerator
{
    private:
    public:
        void generate(const std::string& filename);
};
