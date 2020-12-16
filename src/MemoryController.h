#pragma once

#include <memory>

#include "AddressRange.h"
#include "Memory.h"

namespace gbx
{

class MemoryController
{
public:
    MemoryController() = default;
    ~MemoryController() = default;

    void RegisterMemoryResource(std::shared_ptr<Memory> resource, AddressRange range);
};

}