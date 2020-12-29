#pragma once

#include <cstdint>
#include <optional>

namespace gbx
{

enum class RangeType
{
    AllInclusive,
    BeginInclusive,
    EndInclusive,
    NoneInclusive
};

class AddressRange
{
public:
    AddressRange(uint16_t, uint16_t, std::optional<RangeType>);
    uint16_t Begin() const;
    uint16_t End() const;

private:
    inline void SetAllInclusive(uint16_t, uint16_t);
    inline void SetBeginInclusive(uint16_t, uint16_t);
    inline void SetEndInclusive(uint16_t, uint16_t);
    inline void SetNoneInclusive(uint16_t, uint16_t);

    uint16_t _begin;
    uint16_t _end;
};

}