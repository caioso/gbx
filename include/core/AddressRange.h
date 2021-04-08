#pragma once

#include <cstdint>
#include <optional>

namespace gbxcore
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
    AddressRange(size_t, size_t, std::optional<RangeType>);
    size_t Begin() const;
    size_t End() const;

private:
    inline void SetAllInclusive(size_t, size_t);
    inline void SetBeginInclusive(size_t, size_t);
    inline void SetEndInclusive(size_t, size_t);
    inline void SetNoneInclusive(size_t, size_t);

    size_t _begin;
    size_t _end;
};

}