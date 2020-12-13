#include "MemoryBase.h"

using namespace std;

namespace gbx
{
MemoryBase::MemoryBase(size_t size)
    : _size(size)
{}

size_t MemoryBase::Size()
{
    return _size;
}

}
