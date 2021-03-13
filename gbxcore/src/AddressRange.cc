#include "AddressRange.h"

namespace gbxcore
{

AddressRange::AddressRange(uint16_t begin, uint16_t end, std::optional<RangeType> range)
{
    auto rangeType = range.value_or(RangeType::AllInclusive);

    switch (rangeType)
    {
        case RangeType::AllInclusive: 
            SetAllInclusive(begin, end);
             break;
        case RangeType::BeginInclusive:
             SetBeginInclusive(begin, end);
             break;
        case RangeType::EndInclusive:
             SetEndInclusive(begin, end);
             break;
        case RangeType::NoneInclusive:
             SetNoneInclusive(begin, end);
             break;
    }
}

uint16_t AddressRange::Begin() const
{
    return _begin;
}

uint16_t AddressRange::End() const
{
    return _end;
}

inline void AddressRange::SetAllInclusive(uint16_t begin, uint16_t end)
{
    _begin = begin;
    _end = end;
}

inline void AddressRange::SetBeginInclusive(uint16_t begin, uint16_t end)
{
    _begin = begin;
    _end = (end - 1);
}

inline void AddressRange::SetEndInclusive(uint16_t begin, uint16_t end)
{
    _begin = (begin - 1);
    _end = end;
}

inline void AddressRange::SetNoneInclusive(uint16_t begin, uint16_t end)
{
    _begin = (begin - 1);
    _end = (end - 1);
}

}