#include "Construction.h"

namespace gbxasm::interfaces
{

Construction::Construction(size_t line, size_t column)
    : _column(column)
    , _line(line)
{}

size_t Construction::Column()
{
    return _column;
}

size_t Construction::Line()
{
    return _line;
}

void Construction::SetColumn(size_t column)
{
    _column = column;
}

void Construction::SetLine(size_t line)
{
    _line = line;
}
}