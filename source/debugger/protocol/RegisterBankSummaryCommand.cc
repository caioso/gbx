#include "RegisterBankSummaryCommand.h"

using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxdb::protocol
{

RegisterBankSummaryCommand::RegisterBankSummaryCommand()
    : DebugCommand(CommandID::CommandRegisterBankSummary)
{}


void RegisterBankSummaryCommand::SetSummary(shared_ptr<Runtime> runtime)
{
    _registerValues[0] = get<uint8_t>(runtime->ReadRegister(Register::B));
    _registerValues[1] = get<uint8_t>(runtime->ReadRegister(Register::C));
    _registerValues[2] = get<uint8_t>(runtime->ReadRegister(Register::D));
    _registerValues[3] = get<uint8_t>(runtime->ReadRegister(Register::E));
    _registerValues[4] = get<uint8_t>(runtime->ReadRegister(Register::H));
    _registerValues[5] = get<uint8_t>(runtime->ReadRegister(Register::L));
    _registerValues[6] = get<uint8_t>(runtime->ReadRegister(Register::A));
    _registerValues[7] = get<uint8_t>(runtime->ReadRegister(Register::F));
    _registerValues[8] = (get<uint8_t>(runtime->ReadRegister(Register::IR)) & 0xFF); 
    _registerValues[9] = (get<uint8_t>(runtime->ReadRegister(Register::PIR)) & 0xFF); 
    _registerValues[10] = (get<uint16_t>(runtime->ReadRegister(Register::PC)) & 0xFF); 
    _registerValues[11] = ((get<uint16_t>(runtime->ReadRegister(Register::PC)) >> 0x08) & 0xFF);
    _registerValues[12] = (get<uint16_t>(runtime->ReadRegister(Register::SP)) & 0xFF); 
    _registerValues[13] = ((get<uint16_t>(runtime->ReadRegister(Register::SP)) >> 0x08) & 0xFF);
}

void RegisterBankSummaryCommand::DecodeResponseMessage(shared_ptr<DebugMessage> message)
{
    auto buffer = message->Buffer();
    if ((*buffer)[0] != (static_cast<uint16_t>(MessageID::MessageRegisterBankSummary) & 0xFF) ||
        (*buffer)[1] != (static_cast<uint16_t>(MessageID::MessageRegisterBankSummary >> 0x08) & 0xFF))
        throw GBXDebuggerExceptions("Requested message does not encode a 'RegisterBankSummary' debug message");

    _registerValues[0] = (*buffer)[2];
    _registerValues[1] = (*buffer)[3];
    _registerValues[2] = (*buffer)[4];
    _registerValues[3] = (*buffer)[5];
    _registerValues[4] = (*buffer)[6];
    _registerValues[5] = (*buffer)[7];
    _registerValues[6] = (*buffer)[8];
    _registerValues[7] = (*buffer)[9];
    _registerValues[8] = (*buffer)[10];
    _registerValues[9] = (*buffer)[11];
    _registerValues[10] = (*buffer)[12];
    _registerValues[11] = (*buffer)[13];
    _registerValues[12] = (*buffer)[14];
    _registerValues[13] = (*buffer)[15];
}


void RegisterBankSummaryCommand::DecodeRequestMessage([[maybe_unused]] shared_ptr<DebugMessage> message)
{}

shared_ptr<DebugMessage> RegisterBankSummaryCommand::EncodeCommandMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = static_cast<uint16_t>(MessageID::MessageRegisterBankSummary) & 0xFF;
    (*buffer)[1] = ((static_cast<uint16_t>(MessageID::MessageRegisterBankSummary)) >> 0x08) & 0xFF;
    
    copy(_registerValues.begin(), _registerValues.end(), (*buffer).begin() + 2);

    return make_shared<DebugMessage>(buffer);
}

}