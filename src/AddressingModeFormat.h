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
    bool acquireOperand2;
        bool acquireOperand2FromPc;
        bool acquireOperand2AtComposedAddress;
    bool acquireOperand3;
    bool writeBack;
        bool writeBackAtOperandAddress;
        bool writeBackAtRegisterAddress;
        bool writeBackAtComposedOperandAddress;
};

class AddressingModeTemplate
{
public:
    static const AddressingModeFormat NoAddressingMode;
    static const AddressingModeFormat RegisterAddressingMode;
    static const AddressingModeFormat ImmediateAddressingMode;
    static const AddressingModeFormat RegisterIndexedSourceAddressingMode;
    static const AddressingModeFormat RegisterIndexedDestinationAddressingMode;
    static const AddressingModeFormat RegisterIndirectSourceAddressingMode;
    static const AddressingModeFormat RegisterIndirectDestinationAddressingMode;
    static const AddressingModeFormat RegisterExtendedSourceAddressingMode;
    static const AddressingModeFormat RegisterExtendedDestinationAddressingMode;
    static const AddressingModeFormat ImmediateRegisterIndirectAddressingMode;
    static const AddressingModeFormat ImmediateRegisterIndirectSourceIncrementAddressingMode;
    static const AddressingModeFormat ImmediateRegisterIndirectSourceDecrementAddressingMode;
    static const AddressingModeFormat ImmediateRegisterIndirectDestinationIncrementAddressingMode;
    static const AddressingModeFormat ImmediateRegisterIndirectDestinationDecrementAddressingMode;
};

}