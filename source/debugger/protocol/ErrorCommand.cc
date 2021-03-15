#include "ErrorCommand.h"

using namespace gbxdb::interfaces;
using namespace std;

namespace gbxdb::protocol
{

ErrorCommand::ErrorCommand(ErrorID error)
    : DebugCommand(CommandID::CommandError)
    , _errorId(error)
{}

void ErrorCommand::DecodeRequestMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

shared_ptr<interfaces::DebugMessage> ErrorCommand::EncodeRequestMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = static_cast<uint16_t>(MessageID::MessageError) & 0xFF;
    (*buffer)[1] = ((static_cast<uint16_t>(MessageID::MessageError)) >> 0x08) & 0xFF;
    (*buffer)[2] = static_cast<uint16_t>(_errorId) & 0xff;
    (*buffer)[3] = (static_cast<uint16_t>(_errorId) >> 0x08) & 0xff;

    return make_shared<DebugMessage>(buffer);
}

}