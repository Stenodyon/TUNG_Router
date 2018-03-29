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

std::fstream& operator<<(std::fstream& out, const bool value);
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
    virtual void set_id(uint32_t /*id*/) const {}
};

struct NullObject : public Object
{
    stream& serialize(stream& out) const override
    {
        return out << (uint8_t)0x0A;
    }
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

    operator T () const { return value; }
};

stream& operator<<(stream& out, const ClassWithMembersAndTypes& class_with);
stream& operator<<(stream& out, const Object& object);

template <Class& _class>
struct ClassObject : public Object
{
    static uint32_t first_id;
    mutable uint32_t id;
    std::vector<Object*> values;

    ClassObject(std::vector<Object*> values)
        : id(0), values(values)
    {
    }

    virtual const std::vector<const Object*> get_values() const = 0;

    void set_id(uint32_t _id) const override
    {
        id = _id;
        if(first_id == 0)
        {
            first_id = id;
            std::cout << "ClassObject<" << _class.name.value << ">::first_id = "
                << first_id << std::endl;
        }
    }

    stream& serialize(stream& out) const override
    {
        if(id == 0) set_id(next_id());
        if(id == first_id)
            out << _class.class_mem_types(id);
        else
            out << (uint8_t)0x01 << id << first_id;
        for(const auto& value : get_values())
            out << *value;
        return out;
    }
};

stream& operator<<(stream& out, const BinaryArray& binary_array);

template <Class& _class>
struct ArrayObject : public Object, public std::vector<const Object*>
{
    ArrayObject()
    {
    }

    stream& serialize(stream& out) const override
    {
        uint32_t id = next_id();
        BinaryArray binary_array = _class.binary_array(id);
        binary_array.length = size();
        out << binary_array;
        for(const auto& value : *this)
        {
            uint32_t item_id = next_id();
            out << (uint8_t)0x09 << item_id;
            value->set_id(item_id);
        }
        for(const auto& value : *this)
            out << *value;
        return out;
    }
};

extern std::shared_ptr<AdditionalInfo> BOOL_info;
extern std::shared_ptr<AdditionalInfo> INT_info;
extern std::shared_ptr<AdditionalInfo> FLOAT_info;

extern Class saved_object;
extern Class serialized_vector;
extern Class serialized_color;
extern Class board_class;
extern Class peg_class;
extern Class snapping_peg_class;
extern Class wire_class;

struct SerializableVector3 : public ClassObject<serialized_vector>
{
    Primitive<float> x, y, z;

    SerializableVector3(float x, float y, float z);

    const std::vector<const Object*> get_values() const override;

    float distance_to(const SerializableVector3& other) const;
    SerializableVector3 middle(const SerializableVector3& other) const;
    SerializableVector3 look_at(const SerializableVector3& other) const;
    SerializableVector3 operator+(const SerializableVector3& other) const;

    SerializableVector3 copy() const;
};

struct SerializableColor : public ClassObject<serialized_color>
{
    Primitive<float> r, g, b;

    SerializableColor(float r, float g, float b);

    const std::vector<const Object*> get_values() const override;
};

struct PegBase
{
    SerializableVector3 local_pos, local_angles;
    NullObject children;

    PegBase(const SerializableVector3& _local_pos,
            const SerializableVector3& _local_angles)
        : local_pos(_local_pos), local_angles(_local_angles)
    {}

    virtual const SerializableVector3 get_pos() const = 0;
    float distance_to(const PegBase * other) const;
};

struct Peg : public PegBase, public ClassObject<peg_class>
{
    Peg(SerializableVector3 local_pos, SerializableVector3 local_angles);

    const SerializableVector3 get_pos() const override;
    const std::vector<const Object*> get_values() const override;
};

struct SnappingPeg : public PegBase, public ClassObject<snapping_peg_class>
{
    uint8_t side;

    SnappingPeg(SerializableVector3 local_pos, uint8_t side);

    const SerializableVector3 get_pos() const override;
    const std::vector<const Object*> get_values() const override;

    float distance_to(const PegBase * other) const;
};

struct Wire : public ClassObject<wire_class>
{
    Primitive<bool> input_input;
    Primitive<float> length;
    SerializableVector3 local_pos, local_angles;
    NullObject children;

    Wire(PegBase & peg_a, PegBase & peg_b);

    const std::vector<const Object*> get_values() const override;
};

struct Board : public ClassObject<board_class>
{
    Primitive<uint32_t> x, z;
    SerializableColor color;
    SerializableVector3 local_pos, local_angles;
    mutable ArrayObject<saved_object> children;

    std::vector<std::shared_ptr<PegBase>> pegs_container;
    std::unordered_map<vi2, PegBase*> pegs;
    std::vector<std::shared_ptr<Wire>> wires;
    std::vector<std::shared_ptr<Board>> boards;

    Board(uint32_t x, uint32_t z, SerializableColor color,
            SerializableVector3 local_pos, SerializableVector3 local_angles);

    const std::vector<const Object*> get_values() const override;

    PegBase* add_peg(const vi2& pos);
    PegBase* add_snapping_peg(const vi2& pos, uint8_t side);
    void add_wire(const vi2& from, const vi2& to);
    void add_wire(PegBase* from, PegBase* to);
    Board* add_board(const vi2& pos, const vi2& size);
};

class BoardGenerator
{
    private:
    public:
        void generate(const std::string& filename, Board& board);
};
