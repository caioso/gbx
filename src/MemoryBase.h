#include <cstdint>
#include <stdlib.h>

namespace gbx
{
class MemoryBase
{
public:
    explicit MemoryBase(std::size_t size);
    std::size_t Size();

private:
    std::size_t _size;
};

}