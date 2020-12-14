#include <cstdint>
#include <stdlib.h>
#include <array>

#include "MemoryBase.h"

namespace gbx
{

class ReadOnlyMemory : public MemoryBase
{
public:
    explicit ReadOnlyMemory(size_t size);
    std::uint8_t ReadWord(std::size_t address);
};

}