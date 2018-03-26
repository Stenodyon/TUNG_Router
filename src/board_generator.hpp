#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cassert>

#include "v2.hpp"

#pragma pack(push, 1)
struct v3
{
    float x, y, z;
};
#pragma pack(pop)

static_assert(sizeof(v3) == 12, "v3 not 3 * 4 bytes");

struct LengthPrefixedString
{
    const std::string value;

    friend std::fstream& operator<<(std::fstream& out,
            const LengthPrefixedString& lpstring);
};

LengthPrefixedString operator "" _lp(const char* str, size_t length);

struct BTE;
struct Class;

struct ClassInstance
{
    uint32_t id, ref_id;
    const Class* _class;
    std::vector<BTE> member_btes;

    friend std::fstream& operator<<(std::fstream& out,
            const ClassInstance& instance);

    ClassInstance& operator=(const ClassInstance& copy);
};

struct BTE
{
    uint8_t enum_val = 0x00;
    uint8_t bte = 0x08;
    union {
        uint32_t int32;
        float float32;
        ClassInstance* instance;
    };

    BTE(uint32_t int32);
    BTE(float float32);
    BTE(ClassInstance* instance);
    BTE(const BTE& other);
    ~BTE() {}
    BTE& operator=(const BTE& other);

    friend std::fstream& operator<<(std::fstream& out,
            const BTE& bte);
};

struct Class
{
    static std::unordered_map<std::string, uint32_t> ids;
    const LengthPrefixedString name;
    const uint32_t library_id;
    const std::vector<LengthPrefixedString> member_names;
    const bool is_array = false;

    ClassInstance instantiate(std::vector<BTE> btes) const;
};

const Class Class_Color{
    "SerializableColor"_lp,
    0x00000002,
    {
        "r"_lp, "g"_lp, "b"_lp
    }
};

struct SerializableColor
{
    float r, g, b;

    ClassInstance serialize();
};

const Class Class_Vector{
    "SerializableVector3"_lp,
    0x00000002,
    {
        "x"_lp, "y"_lp, "z"_lp
    }
};

struct SerializableVector3
{
    float x, y, z;

    ClassInstance serialize();
};

const Class Class_Children {
    "SavedObjects.SavedObjectV2[]"_lp,
    0x00000002,
    {
    }
};

const Class Class_Board{
    "SavedObjects.SavedCircuitBoard"_lp,
    0x00000002,
    {
        "x"_lp, "z"_lp,
        "color"_lp,
        "LocalPosition"_lp,
        "LocalEulerAngles"_lp,
        "Children"_lp
    }
};

class BoardGenerator
{
    private:
    public:
        void generate(const std::string& filename);
};
