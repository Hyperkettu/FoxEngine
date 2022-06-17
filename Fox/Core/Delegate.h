#pragma once

#include <type_traits>

namespace Fox {

    template <class Signature>
    class Delegate;

    template <class Signature>
    class Signal;

    template <class ReturnType, class... ParamTypes>
    class FOX_API Delegate<ReturnType(ParamTypes...)> {
        friend class Signal<ReturnType(ParamTypes...)>;
    public:
        Delegate() : callableWrapper(nullptr) {}
        Delegate(const Delegate& other) = delete;

        Delegate(Delegate&& other) : callableWrapper(other.callableWrapper)
        {
            other.callableWrapper = nullptr;
        }

        ~Delegate()
        {
            delete callableWrapper;
            callableWrapper = nullptr;
        }

        Delegate& operator=(Delegate const& other) = delete;

        Delegate& operator=(Delegate&& other)
        {
            Delegate temp(std::move(other));
            Swap(temp);

            return *this;
        }

        template <class T>
        void Connect(T& instance, ReturnType(T::*ptrToMemberFunction)(ParamTypes...))
        {
            callableWrapper = new MemberFunctionCallableWrapper<T, ReturnType(ParamTypes...)>(instance, ptrToMemberFunction);
        }

        template <class T>
        void Connect(T& instance, ReturnType(T::* ptrToConstMemberFunction)(ParamTypes...) const)
        {
            callableWrapper = new ConstMemberFunctionCallableWrapper<T, ReturnType(ParamTypes...)>(instance, ptrToConstMemberFunction);
        }

        template <class T>
        void Connect(T&& functionObject)
        {
           callableWrapper = new FunctionObjectCallableWrapper<std::remove_reference_t<T>, ReturnType(ParamTypes...)>(std::forward<T>(functionObject));
        }

        void Swap(Delegate& other)
        {
            CallableWrapper<ReturnType(ParamTypes...)>* temp = callableWrapper;
            callableWrapper = other.callableWrapper;
            other.callableWrapper = temp;
        }

        explicit operator bool() const { 
            return callableWrapper != nullptr; 
        }

        ReturnType operator()(ParamTypes... args) {
            return callableWrapper->Invoke(std::forward<ParamTypes>(args)...);
        }

        ReturnType Invoke(ParamTypes... args) {
            return callableWrapper->Invoke(std::forward<ParamTypes>(args)...);
        }
    private:
        CallableWrapper<ReturnType(ParamTypes...)>* callableWrapper;
    };

}