#pragma once

namespace gbx
{

enum class AddressingMode
{
    Register,
    RegisterPair,
    Immediate,
    ImmediatePair,
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
    ExtendedDestination,
    RegisterImplicitSource,
    RegisterImplicitDestination,
    ImmediateImplicitSource,
    ImmediateImplicitDestination
};

}