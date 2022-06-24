#pragma once

#include <vector>
#include <type_traits>

#include "Fox/Core/Delegate.h"
#include "Fox/Core/SignalBind.h"

namespace Fox {

    /* signal primary class template (not defined)*/
    template <class Signature>
    class Signal;

    /* signal partial class template for function types */
    template <class ReturnType, class... ParamTypes>
    class FOX_API Signal<ReturnType(ParamTypes...)> {
        friend class SignalBind;
    public:
        template <class T>
        SignalBind Connect(T& instance, ReturnType(T::* ptrToMemberFunction)(ParamTypes...)) {
            Delegate<ReturnType(ParamTypes...)> delegate;
            delegates.push_back(std::move(delegate));
            delegates.back().Connect(instance, ptrToMemberFunction);

            return SignalBind(this, delegates.back().callableWrapper);
        }

        template <class T>
        SignalBind Connect(T& instance, ReturnType(T::* ptrToConstMemberFunction)(ParamTypes...) const)
        {
            Delegate<ReturnType(ParamTypes...)> delegate;
            delegates.push_back(std::move(delegate));
            delegates.back().Connect(instance, ptrToConstMemberFunction);

            return SignalBind(this, delegates.back().callableWrapper);
        }

        template <class T>
        SignalBind Connect(T&& functionObject)
        {
            Delegate<ReturnType(ParamTypes...)> delegate;
            delegates.push_back(std::move(delegate));
            delegates.back().Connect(std::forward<T>(functionObject));

            return SignalBind(this, delegates.back().callableWrapper);
        }

        explicit operator bool() const
        {
            return !delegates.empty();
        }

        void operator()(ParamTypes... args)
        {
            for (auto& delegate : delegates)
                delegate(std::forward<ParamTypes>(args)...);
        }

        void Invoke(ParamTypes... args)
        {
            for (auto& delegate : delegates)
                delegate.Invoke(std::forward<ParamTypes>(args)...);
        }
    private:
        void Disconnect(CallableWrapper<ReturnType(ParamTypes...)>* callableWrapper)
        {
            for (auto it = delegates.begin(), end = delegates.end(); it != end; ++it)
                if (it->callableWrapper == callableWrapper)
                {
                    delegates.erase(it);
                    return;
                }
        }

        std::vector<Delegate<ReturnType(ParamTypes...)>> delegates;
    };
}