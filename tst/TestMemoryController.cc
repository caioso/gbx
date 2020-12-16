#include <gtest/gtest.h>

#include "../src/AddressRange.h"
#include "../src/CPU.h"
#include "../src/MemoryController.h"
#include "../src/ROM.h"

using namespace std;
using namespace gbx;

TEST(MemoryControllerTests, ROMRegistration) 
{
    MemoryController memController;
    shared_ptr<Memory> rom(new ROM(0x100));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0000, 0x0100, RangeType::AllInclusive)
    );
}
