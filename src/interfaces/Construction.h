#pragma once

#include <cstdlib>

namespace gbxasm::interfaces
{

class Construction
{
public:
    Construction(size_t line, size_t column);
    virtual ~Construction() = default;

    Construction(const Construction&) = default;
    Construction(Construction&&) = default;
    Construction& operator=(const Construction&) = default;
    Construction& operator=(Construction&&) = default;

    size_t Column();
    size_t Line();

protected:
    void SetColumn(size_t column);
    void SetLine(size_t line);

    size_t _column{};
    size_t _line{};
};

}