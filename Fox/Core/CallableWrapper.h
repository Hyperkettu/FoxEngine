#pragma once

#include <utility>

namespace Fox {

    template <class Signature>
    class CallableWrapper;

    template <class ReturnType, class... ParamTypes>
    class FOX_API CallableWrapper<ReturnType(ParamTypes...)>
    {
    public:
        virtual ~CallableWrapper() = default;

        virtual ReturnType Invoke(ParamTypes... args) = 0;
    protected:
        CallableWrapper() = default;
    };

    /* wrapper around a non-const member function */
    template <class T, class Signature>
    class MemberFunctionCallableWrapper;

    template <class T, class ReturnType, class... ParamTypes>
    class FOX_API MemberFunctionCallableWrapper<T, ReturnType(ParamTypes...)> : public CallableWrapper<ReturnType(ParamTypes...)>
    {
    private:
        using PtrToMemberFunc = ReturnType(T::*)(ParamTypes...);
    public:
        MemberFunctionCallableWrapper(T& instance, PtrToMemberFunc ptrToMemberFunction) : instance(instance),
            ptrToMemberFunction(ptrToMemberFunction) {}

        ReturnType Invoke(ParamTypes... args) override {
            return (instance.*ptrToMemberFunction)(std::forward<ParamTypes>(args)...);
        }

    private:
        T& instance;
        PtrToMemberFunc ptrToMemberFunction;
    };

    /***** wrapper around a const member function *****/
    template <class T, class Signature>
    class ConstMemberFunctionCallableWrapper;

    template <class T, class ReturnType, class... ParamTypes>
    class FOX_API ConstMemberFunctionCallableWrapper<T, ReturnType(ParamTypes...)> : public CallableWrapper<ReturnType(ParamTypes...)>
    {
    private:
        using PtrToConstMemberFunction = ReturnType(T::*)(ParamTypes...) const;
    public:
        ConstMemberFunctionCallableWrapper(T& instance, PtrToConstMemberFunction ptrToConstMemberFunction) :
            instance(instance), ptrToConstMemberFunction(ptrToConstMemberFunction) {}

        ReturnType Invoke(ParamTypes... args) override {
            return (instance.*ptrToConstMemberFunction)(std::forward<ParamTypes>(args)...);
        }

    private:
        T& instance;
        PtrToConstMemberFunction ptrToConstMemberFunction;
    };

    /* wrapper around a function object/lambda */
    template <class T, class Signature>
    class FunctionObjectCallableWrapper;

    template <class T, class ReturnType, class... ParamTypes>
    class FOX_API FunctionObjectCallableWrapper<T, ReturnType(ParamTypes...)> : public CallableWrapper<ReturnType(ParamTypes...)>
    {
    public:
        FunctionObjectCallableWrapper(T& functionObject) : functionObject(&functionObject), allocated(false) {}                        // lvalue (take address)
        FunctionObjectCallableWrapper(T&& functionObject) : functionObject(new T(std::move(functionObject))), allocated(true) {}       // rvalue (make copy on the heap)

        ~FunctionObjectCallableWrapper() {
            Destroy();
        }

        ReturnType Invoke(ParamTypes... args) {
            return (*functionObject)(std::forward<ParamTypes>(args)...);
        }

    private:
        template <class U = T, class = std::enable_if_t<std::is_function<U>::value>>
        void Destroy() {}

        template <class = T, class U = T, class = std::enable_if_t<!std::is_function<U>::value>>
        void Destroy() {                                                                          
            if (allocated)
                delete functionObject;
        }

        T* functionObject;
        bool allocated;
    };
}