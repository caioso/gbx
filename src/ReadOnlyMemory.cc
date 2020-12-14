#include "ReadOnlyMemory.h"
namespace gbx
{

ReadOnlyMemory::ReadOnlyMemory(size_t size)
    : MemoryBase(size)
{}

std::uint8_t ReadWord(std::size_t address)
{
    return address;
}

}
