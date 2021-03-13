#include "OutputStream.h"

using namespace std;
using namespace gbxasm;

namespace gbxasm::streams
{

OutputStream::OutputStream(std::optional<std::string> prefix, std::shared_ptr<interfaces::StreamSink> sink)
    : _sink(sink)
    , _prefix(prefix.value_or(""))
{}

void OutputStream::Write(std::string message, std::optional<std::string> file,  std::optional<size_t> line, std::optional<size_t> column)
{
    stringstream ss;

    if (file.has_value() && line.has_value() && column.has_value())
        ss << file.value() << ':' << line.value() << ':' << column.value() << ": ";

    if (!_prefix.empty())
        ss << _prefix << ": ";

    ss << message;

    _sink->Write(ss.str());
}

}