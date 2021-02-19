#include "DebugRequestConsumer.h"

using namespace std;

namespace gbx::interfaces
{

DebugRequestConsumer::DebugRequestConsumer(std::shared_ptr<DebugRequestProducer> producer)
    : _producer(producer)
{}

}