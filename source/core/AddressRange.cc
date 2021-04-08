#include "AddressRange.h"

namespace gbxcore
{

AddressRange::AddressRange(size_t begin, size_t end, std::optional<RangeType> range)
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

size_t AddressRange::Begin() const
{
    return _begin;
}

size_t AddressRange::End() const
{
    return _end;
}

inline void AddressRange::SetAllInclusive(size_t begin, size_t end)
{
    _begin = begin;
    _end = end;
}

inline void AddressRange::SetBeginInclusive(size_t begin, size_t end)
{
    _begin = begin;
    _end = (end - 1);
}

inline void AddressRange::SetEndInclusive(size_t begin, size_t end)
{
    _begin = (begin - 1);
    _end = end;
}

inline void AddressRange::SetNoneInclusive(size_t begin, size_t end)
{
    _begin = (begin - 1);
    _end = (end - 1);
}

}