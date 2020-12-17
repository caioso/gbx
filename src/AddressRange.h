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
    AddressRange(uint16_t begin, uint16_t end, std::optional<RangeType> range);
    uint16_t Begin() const;
    uint16_t End() const;

private:
    void SetAllInclusive(uint16_t begin, uint16_t end);
    void SetBeginInclusive(uint16_t begin, uint16_t end);
    void SetEndInclusive(uint16_t begin, uint16_t end);
    void SetNoneInclusive(uint16_t begin, uint16_t end);

    uint16_t _begin;
    uint16_t _end;
};

}