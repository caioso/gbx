#pragma once

namespace gbx
{

struct AddressingModeFormat
{
    bool acquireOperand1;
        bool acquireOperand1FromPc;
        bool acquireOperand1Directly;
            bool incrementSource;
            bool decrementSource;
            bool incrementDestination;
            bool decrementDestination;
        bool acquireOperand1Implicitly;
    bool acquireOperand2;
        bool acquireOperand2FromPc;
        bool acquireOperand2AtComposedAddress;
        bool acquireOperand2Implicitly;
        bool acquireOperand2Directly;
            bool incrementSourceOperand2;
    bool acquireOperand3;
    bool writeBack;
        bool writeBackAtOperandAddress;
        bool writeBackAtRegisterAddress;
        bool writeBackAtComposedOperandAddress;
        bool writeBackAtImplicitlyWithRegister;
        bool writeBackAtImplicitlyWithImmediateOperand;
        bool writeBackPairAtRegisterAddress;
};

class AddressingModeTemplate
{
public:
    static AddressingModeFormat NoAddressingMode;
    static AddressingModeFormat RegisterAddressingMode;
    static AddressingModeFormat ImmediateAddressingMode;
    static AddressingModeFormat RegisterIndexedSourceAddressingMode;
    static AddressingModeFormat RegisterIndexedDestinationAddressingMode;
    static AddressingModeFormat RegisterIndirectSourceAddressingMode;
    static AddressingModeFormat RegisterIndirectDestinationAddressingMode;
    static AddressingModeFormat RegisterExtendedSourceAddressingMode;
    static AddressingModeFormat RegisterExtendedDestinationAddressingMode;
    static AddressingModeFormat ImmediateRegisterIndirectAddressingMode;
    static AddressingModeFormat ImmediateRegisterIndirectSourceIncrementAddressingMode;
    static AddressingModeFormat ImmediateRegisterIndirectSourceDecrementAddressingMode;
    static AddressingModeFormat ImmediateRegisterIndirectDestinationIncrementAddressingMode;
    static AddressingModeFormat ImmediateRegisterIndirectDestinationDecrementAddressingMode;
    static AddressingModeFormat ImplicitRegisterSourceAddressingMode;
    static AddressingModeFormat ImplicitRegisterDestinationAddressingMode;
    static AddressingModeFormat ImplicitImmediateSourceAddressingMode;
    static AddressingModeFormat ImplicitImmediateDestinationAddressingMode;
    static AddressingModeFormat ImmediatePairAddressingMode;
    static AddressingModeFormat RegisterPairAddressingMode;
    static AddressingModeFormat RegisterIndirectSourceAndDestinationAddressingMode;
    static AddressingModeFormat RegisterIndirectDestinationPair;
    static AddressingModeFormat RegisterIndirectSourcePair;
};

}