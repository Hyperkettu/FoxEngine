#pragma once

#include "Fox/Core/Signal.h"
#include "Fox/Core/Delegate.h"

namespace Fox {

    template <typename Signature>
    class Signal;

    class FOX_API SignalBind
    {
    public:
        SignalBind() = default;
        template <class ReturnType, class... ParamTypes>
        SignalBind(Signal<ReturnType(ParamTypes...)>* signal, CallableWrapper<ReturnType(ParamTypes...)>* callableWrapper) : 
            signal(signal), callableWrapper(callableWrapper), disconnectFunction(&DisconnectFunction<ReturnType, ParamTypes...>) {}
        void Disconnect() { disconnectFunction(signal, callableWrapper); }
    private:
        void (*disconnectFunction)(void*, void*);
        void* signal;
        void* callableWrapper;

        template <class ReturnType, class... ParamTypes>
        static void DisconnectFunction(void* signal, void* callableWrapper)
        {
            static_cast<Signal<ReturnType(ParamTypes...)>*>(signal)->Disconnect(
                static_cast<CallableWrapper<ReturnType(ParamTypes...)>*>(callableWrapper));
        }
    };
}