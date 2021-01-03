#pragma once

namespace gbx
{

enum class AddressingMode
{
    Register,
    Immediate,
    ImmediateRegisterIndirect,
    RegisterIndirectSource,
    RegisterIndirectDestination,
    RegisterIndirectSourceIncrement,
    RegisterIndirectDestinationIncrement,
    RegisterIndirectSourceDecrement,
    RegisterIndirectDestinationDecrement,
    Implicit,
    RegisterIndexedSource,
    RegisterIndexedDestination,
    ExtendedSource,
    ExtendedDestination
};

}