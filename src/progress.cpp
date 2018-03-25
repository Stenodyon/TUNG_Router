#include "progress.hpp"

std::ostream & operator<<(std::ostream & out, const progress_bar & bar)
{
    out << "\r\xde";

    const uint_t width = 100;
    double ratio = (double)bar.value / (double)(bar.max - 1);
    uint_t length = width * ratio;

    for(uint_t i = 0; i < length; i++)
        out << "\xb2";
    for(uint_t i = 0; i < (width - length); i++)
        out << "\xb0";
    out << "\xdd";
    return out;
}
