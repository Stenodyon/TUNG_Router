#define _USE_MATH_DEFINES
#include "board_generator.hpp"

#include <cstring>

// TODO snapping peg direction & wiring
// TODO clean up the ClassObject constructor

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
        const bool value)
{
    uint8_t int_value = value ? 0x01 : 0x00;
    out.write((char*)&int_value, 1);
    return out;
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

template <Class& _class>
uint32_t ClassObject<_class>::first_id = 0;

SerializableVector3::SerializableVector3(float _x, float _y, float _z)
    : x(_x), y(_y), z(_z),
    ClassObject<serialized_vector>({&x, &y, &z})
{
}

const std::vector<const Object*> SerializableVector3::get_values() const
{
    return {&x, &y, &z};
}

float SerializableVector3::distance_to(const SerializableVector3& other) const
{
    float _x = other.x - x;
    float _y = other.y - y;
    float _z = other.z - z;
    return std::sqrt(_x * _x + _y * _y + _z * _z);
}

SerializableVector3 SerializableVector3::middle(const SerializableVector3& other) const
{
    return SerializableVector3{
        (x + other.x) / 2,
        (y + other.y) / 2,
        (z + other.z) / 2
    };
}

SerializableVector3 SerializableVector3::look_at(const SerializableVector3& other) const
{
    static const float rad2deg = 180. / M_PI;
    float _x = other.x - x;
    float _y = other.y - y;
    float _z = other.z - z;

    float new_x = -std::atan2(_y, std::sqrt(_x * _x + _z * _z));
    float new_y = std::atan2(_x, _z * std::cos(new_x));
    float new_z = 0;

    return SerializableVector3{
        new_x * rad2deg,
        new_y * rad2deg,
        new_z * rad2deg
    };
}

SerializableVector3 SerializableVector3::operator+(const SerializableVector3& other) const
{
    return SerializableVector3{
        x + other.x,
        y + other.y,
        z + other.z
    };
}

SerializableVector3 SerializableVector3::copy() const
{
    return SerializableVector3(x, y, z);
}

const std::vector<const Object*> SerializableColor::get_values() const
{
    return {&r, &g, &b};
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
            {&x, &z, &color, &local_pos, &local_angles, &children}),
    parent(nullptr)
{
}

const std::vector<const Object*> Board::get_values() const
{
    children.clear();
    for(auto peg : pegs_container)
        children.push_back(dynamic_cast<Object*>(peg.get()));
    for(auto wire : wires)
        children.push_back(wire.get());
    for(auto board : boards)
        children.push_back(board.get());
    return {&x, &z, &color, &local_pos, &local_angles, &children};
}


PegBase* Board::add_peg(const vi2& pos)
{
    if(pegs.find(pos) == pegs.end())
    {
        auto new_x = x - 1 - pos.x;
        auto new_z = pos.y;
        auto peg = std::make_shared<Peg>(
            SerializableVector3{ 0.15f + new_x * 0.30f, 0.075f, 0.15f + new_z * 0.30f },
            SerializableVector3{ 0.f, 0.f, 0.f }
        );
        peg->parent = this;
        pegs_container.push_back(peg);
        pegs[pos] = peg.get();
    }
    return pegs[pos];
}

PegBase* Board::add_snapping_peg(const vi2& pos, uint8_t side)
{
    if(pegs.find(pos) == pegs.end())
    {
        auto new_x = x - 1 - pos.x;
        auto new_z = pos.y;
        auto peg = std::make_shared<SnappingPeg>(
            SerializableVector3{ 0.15f + new_x * 0.30f, 0.075f, 0.15f + new_z * 0.30f },
            side
        );
        peg->parent = this;
        pegs_container.push_back(peg);
        pegs[pos] = peg.get();
    }
    return pegs[pos];
}

void Board::add_wire(const vi2& from, const vi2& to)
{
    if(pegs.find(from) == pegs.end())
        return;
    if(pegs.find(to) == pegs.end())
        return;
    auto wire = std::make_shared<Wire>(*(pegs[from]), *(pegs[to]));
    wires.push_back(wire);
}

void Board::add_wire(PegBase* from, PegBase* to)
{
    auto wire = std::make_shared<Wire>(*from, *to);
    wires.push_back(wire);
}

Board* Board::add_board(const vi2& pos, const vi2& size)
{
    auto new_x = x - 1 - pos.x - (size.x - 1);
    auto new_z = pos.y;
    auto board = std::make_shared<Board>(
            size.x, size.y,
            SerializableColor{ 0., 0., 0. },
            SerializableVector3{ new_x * 0.30f, 2 * 0.075f, new_z * 0.30f },
            SerializableVector3{ 0.f, 0.f, 0.f }
            );
    boards.push_back(board);
    board->parent = this;
    return board.get();
}

Peg::Peg(SerializableVector3 _local_pos, SerializableVector3 _local_angles)
    : PegBase(_local_pos, _local_angles),
    ClassObject<peg_class>({&local_pos, &local_angles, &children})
{
}

const SerializableVector3 PegBase::get_pos() const
{
    auto pos = local_pos;
    Board * current_parent = parent;
    while(current_parent != nullptr)
    {
        auto parent_pos = current_parent->local_pos;
        //parent_pos.y = parent_pos.y / 2;
        pos = pos + parent_pos;
        current_parent = current_parent->parent;
    }
    return pos;
}

float side_angle[4] = {
    180.f, 0.f, 270.f, 90.f
};

SnappingPeg::SnappingPeg(SerializableVector3 _local_pos, uint8_t side)
    : PegBase(_local_pos, {0., side_angle[side], 0.}),
    ClassObject<snapping_peg_class>({}),
    side(side)
{
}

const SerializableVector3 directions[4] = {
    { 0., 0., 0.075 },
    { 0., 0., -.075 },
    { 0.075, 0., 0. },
    { -.075, 0., 0. },
};

const std::vector<const Object*> Peg::get_values() const
{
    return {&local_pos, &local_angles, &children};
}

float PegBase::distance_to(const PegBase * other) const
{
    return get_pos().distance_to(other->get_pos());
}

const SerializableVector3 SnappingPeg::get_pos() const
{
    return PegBase::get_pos() + directions[side];
}

const std::vector<const Object*> SnappingPeg::get_values() const
{
    return {&local_pos, &local_angles, &children};
}

Wire::Wire(PegBase& peg_a, PegBase& peg_b)
    //: input_input(true), length(peg_a.distance_to(peg_b)),
    : input_input(true),
    length(peg_a.get_pos().distance_to(peg_b.get_pos())),
    local_pos(peg_a.get_pos().middle(peg_b.get_pos())),
    local_angles(peg_a.get_pos().look_at(peg_b.get_pos())),
    ClassObject<wire_class>(
            {&input_input, &length, &local_pos, &local_angles, &children})
{
    local_pos.y = local_pos.y + .27f;
}

const std::vector<const Object*> Wire::get_values() const
{
    return {&input_input, &length, &local_pos, &local_angles, &children};
}

stream& operator<<(stream& out, const Object& object)
{
    return object.serialize(out);
}

std::shared_ptr<AdditionalInfo> BOOL_info(new AdditionalInfo_impl<uint8_t>(0x01));
std::shared_ptr<AdditionalInfo> INT_info(new AdditionalInfo_impl<uint8_t>(0x08));
std::shared_ptr<AdditionalInfo> FLOAT_info(new AdditionalInfo_impl<uint8_t>(0x0B));

Class saved_object { "SavedObjects.SavedObjectV2"_lp,
    {}, 0x00000002, {}
};

Class serialized_vector { "SerializableVector3"_lp,
    { "x"_lp, "y"_lp, "z"_lp },
    0x00000002,
    {
        { 0x00, 0x00, 0x00 },
        { FLOAT_info, FLOAT_info, FLOAT_info }
    }
};

Class serialized_color { "SerializableColor"_lp,
    { "r"_lp, "g"_lp, "b"_lp },
    0x00000002,
    {
        { 0x00, 0x00, 0x00 },
        { FLOAT_info, FLOAT_info, FLOAT_info }
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
        { INT_info, INT_info,
            serialized_color.additional_info(),
            serialized_vector.additional_info(),
            serialized_vector.additional_info(),
            saved_object.binary_array(0).additional_info() // Eww
        }
    }
};

Class peg_class { "SavedObjects.SavedPeg"_lp,
    {
        "LocalPosition"_lp,
        "LocalEulerAngles"_lp,
        "Children"_lp
    },
    0x00000002,
    {
        { 0x04, 0x04, 0x04 },
        {
            serialized_vector.additional_info(),
            serialized_vector.additional_info(),
            saved_object.binary_array(0).additional_info() // Eww
        }
    }
};

Class snapping_peg_class { "SavedObjects.SavedSnappingPeg"_lp,
    {
        "LocalPosition"_lp,
        "LocalEulerAngles"_lp,
        "Children"_lp
    },
    0x00000002,
    {
        { 0x04, 0x04, 0x04 },
        {
            serialized_vector.additional_info(),
            serialized_vector.additional_info(),
            saved_object.binary_array(0).additional_info() // Eww
        }
    }
};

Class wire_class { "SavedObjects.SavedWire"_lp,
    {
        "InputInput"_lp,
        "length"_lp,
        "LocalPosition"_lp,
        "LocalEulerAngles"_lp,
        "Children"_lp
    },
    0x00000002,
    {
        { 0x00, 0x00, 0x04, 0x04, 0x04 },
        {
            BOOL_info, FLOAT_info,
            serialized_vector.additional_info(),
            serialized_vector.additional_info(),
            saved_object.binary_array(0).additional_info() // Eww
        }
    }
};

void BoardGenerator::generate(const std::string & filename, Board& board)
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

    //board.add_board({0, 0}, {2, 3});

#if 0
#define WAIT_A_MINUTE
#endif
#ifndef WAIT_A_MINUTE
    file << board;
#else
    Board test{5, 5, color, pos, angles};
    PegBase * a = test.add_peg({0, 0});
    Board * sub_board = test.add_board({2, 2}, {1, 1});
    PegBase * b = sub_board->add_snapping_peg({0, 0}, 0);
    //b->local_pos.y = b->local_pos.y + 0.5f;
    PegBase * c = test.add_peg({1, 1});
    c->local_pos = a->get_pos().middle(b->get_pos());
    c->local_angles = a->get_pos().look_at(b->get_pos());
    PegBase * d = test.add_peg({4, 0});
    PegBase * e = test.add_peg({3, 0});
    e->local_pos = d->get_pos().middle(b->get_pos());
    e->local_angles = d->get_pos().look_at(b->get_pos());
    PegBase * f = test.add_peg({0, 4});
    PegBase * g = test.add_peg({0, 3});
    g->local_pos = f->get_pos().middle(b->get_pos());
    g->local_angles = f->get_pos().look_at(b->get_pos());
    PegBase * h = test.add_peg({4, 4});
    PegBase * i = test.add_peg({4, 3});
    i->local_pos = h->get_pos().middle(b->get_pos());
    i->local_angles = h->get_pos().look_at(b->get_pos());
    PegBase * j = test.add_peg({0, 2});
    PegBase * k = test.add_peg({0, 1});
    k->local_pos = j->get_pos().middle(b->get_pos());
    k->local_angles = j->get_pos().look_at(b->get_pos());
    PegBase * l = test.add_peg({4, 2});
    PegBase * m = test.add_peg({4, 1});
    m->local_pos = l->get_pos().middle(b->get_pos());
    m->local_angles = l->get_pos().look_at(b->get_pos());
    PegBase * n = test.add_peg({2, 0});
    PegBase * o = test.add_peg({2, 1});
    o->local_pos = n->get_pos().middle(b->get_pos());
    o->local_angles = n->get_pos().look_at(b->get_pos());
    PegBase * p = test.add_peg({2, 4});
    PegBase * q = test.add_peg({2, 3});
    q->local_pos = p->get_pos().middle(b->get_pos());
    q->local_angles = p->get_pos().look_at(b->get_pos());
    file << test;
#endif
    file << (uint8_t)0x0B;
}
