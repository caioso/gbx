#include "CommandLineOutputDriver.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;

namespace gbxdb::output
{

void CommandLineOutputDriver::DisplayRegisterbank(array<uint8_t, RegisterBankSizeInBytes> registerBankValues)
{
    stringstream ss;
    
    for (auto i = 0llu; i < registerBankValues.size() - 2; ++i)
    {
        auto reg = ConvertIndexToRegister(i);
        auto name = ConvertRegisterToRegisterName(reg);
        ss << name << ": 0x" << hex << std::uppercase << static_cast<size_t>(registerBankValues[i]) << '\n';
    }
    
    cout << ss.str() << '\n';
}

Register CommandLineOutputDriver::ConvertIndexToRegister(size_t index)
{
    cout << "Unknown register index" << index << '\n';
    switch (index)
    {
        case static_cast<size_t>(Register::B): return Register::B;
        case static_cast<size_t>(Register::C): return Register::C;
        case static_cast<size_t>(Register::D): return Register::D;
        case static_cast<size_t>(Register::E): return Register::E;
        case static_cast<size_t>(Register::H): return Register::H;
        case static_cast<size_t>(Register::L): return Register::L;
        case static_cast<size_t>(Register::A): return Register::A;
        case static_cast<size_t>(Register::F): return Register::F;
        case static_cast<size_t>(Register::IR): return Register::IR;
        case static_cast<size_t>(Register::PIR): return Register::PIR;
        case static_cast<size_t>(Register::R): return Register::R;
        case static_cast<size_t>(Register::PC): return Register::PC;
        case static_cast<size_t>(Register::PC) + 1: return Register::PC;
        case static_cast<size_t>(Register::SP): return Register::SP;
        case static_cast<size_t>(Register::SP) + 1: return Register::SP;
        case static_cast<size_t>(Register::IX): return Register::IX;
        case static_cast<size_t>(Register::IX) + 1: return Register::IX;
        case static_cast<size_t>(Register::IY): return Register::IY;
        case static_cast<size_t>(Register::IY) + 1: return Register::IY;
        default:
            throw CommandLineOutputDriverException ("Unknown register index");
    }
}

string CommandLineOutputDriver::ConvertRegisterToRegisterName(Register reg)
{
    switch (reg)
    {
        case Register::B: return "B";
        case Register::C: return "C";
        case Register::D: return "D";
        case Register::E: return "E";
        case Register::H: return "H";
        case Register::L: return "L";
        case Register::A: return "A";
        case Register::F: return "F";
        case Register::IR: return "IR";
        case Register::PIR: return "PIR";
        case Register::R: return "R";
        case Register::PC: return "PC";
        case Register::SP: return "SP";
        case Register::IX: return "IX";
        case Register::IY: return "IY";
        case Register::HL: return "HL";
        case Register::BC: return "BC";
        case Register::DE: return "DE";
        case Register::AF: return "AF";
    }
}

}