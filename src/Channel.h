#pragma once

#include <functional>
#include <memory>

#include "GBXExceptions.h"

namespace gbx
{

template<typename T>
class Channel
{
public:
    constexpr inline void Bind(std::shared_ptr<Channel<T>> binding)
    {
        _binding = binding;
    }

    constexpr inline void OnReceived(const std::function<void(T)>& callback)
    {
        _callback = callback;
    }

    constexpr inline void Send(T message)
    {
        _binding->_callback(message);
    }

protected:
    std::shared_ptr<Channel<T>> _binding;
    std::function<void(T)> _callback;
};

}