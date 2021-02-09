#pragma once

#include <cstdlib>

namespace gbxasm::interfaces
{

class Construction
{
public:
    Construction(size_t line, size_t column);
    ~Construction() = default;

    size_t Column();
    size_t Line();

protected:
    void SetColumn(size_t column);
    void SetLine(size_t line);

    size_t _column{};
    size_t _line{};
};

}