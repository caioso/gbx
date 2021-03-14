#pragma once

namespace gbxcore
{

enum class AddressingMode
{
    Register,
    RegisterPair,
    Immediate,
    ImmediatePair,
    SingleImmediatePair, 
    ImmediateRegisterIndirect,
    RegisterIndirectSource,
    RegisterIndirectSourcePair,
    RegisterIndirectDestination,
    RegisterIndirectDestinationPair,
    RegisterIndirectSourceAndDestination,
    RegisterIndirectSourceIncrement,
    RegisterIndirectDestinationIncrement,
    RegisterIndirectSourceDecrement,
    RegisterIndirectDestinationDecrement,
    Implicit,
    RegisterIndexedSource,
    RegisterIndexedDestination,
    ExtendedSource,
    ExtendedDestination,
    ExtendedDestinationPair,
    RegisterImplicitSource,
    RegisterImplicitDestination,
    ImmediateImplicitSource,
    ImmediateImplicitDestination,
    SubRoutineCall,
};

}