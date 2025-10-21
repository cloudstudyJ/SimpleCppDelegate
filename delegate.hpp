#pragma once

#include "typeTraits.hpp"

#define NO_COPY_AND_MOVE(ClassName)                         \
    ClassName(const ClassName&) = delete;                   \
    ClassName(ClassName&&) noexcept = delete;               \
    ClassName& operator=(const ClassName&) = delete;        \
    ClassName& operator=(ClassName&&) noexcept = delete;

#define NO_DYNAMIC_ALLOCATION()                             \
    void* operator new(unsigned long long size) = delete;   \
    void* operator new[](unsigned long long size) = delete; \
    void operator delete(void* ptr) noexcept = delete;      \
    void operator delete[](void* ptr) noexcept = delete;

#define NO_INSTANTIATION(ClassName) \
    ClassName() = delete;           \
    ~ClassName() = delete;          \
    NO_COPY_AND_MOVE(ClassName)     \
    NO_DYNAMIC_ALLOCATION()

/**
 * TODO
 *   1. support lambda, functor (wip)
 *   2. add type-erasure idiom
 *   3. apply CTAD
 */

template <typename, typename = void>
class Delegate { NO_INSTANTIATION(Delegate); };

template <typename Func>
class Delegate<Func, EnableIF_T<isFunction_v<Func>>> {
    NO_COPY_AND_MOVE(Delegate);

    using Traits     = FunctionTrait<Func>;
    using ReturnType = Traits::ReturnType;
    using ParamList  = Traits::ParamList;
    using FuncType   = Traits::Type;

    public:
        Delegate() noexcept = default;
        ~Delegate() noexcept = default;

        template <typename ... Args>
        inline ReturnType operator()(Args&&... args) const {
            if constexpr (isVoid_v<ReturnType>)
                execute(std::forward<Args>(args)...);

            else
                return execute(std::forward<Args>(args)...);
        }

        void bind(Func func) noexcept { mFunction = func; }

        template <typename ... Args>
        inline ReturnType execute(Args&&... args) const {
            static_assert(
                Traits::paramsCount == sizeof...(Args),
                "Delegate::execute() arguments count mismatch"
            );
            static_assert(
                areCallCompatible_v<ParamList, TypeList<Args...>>,
                "Delegate::execute() incompatible argument types"
            );

            if constexpr (isVoid_v<ReturnType>) {
                if (mFunction)
                    mFunction(std::forward<Args>(args)...);
            }
            else {
                if (mFunction)
                    return mFunction(std::forward<Args>(args)...);

                return ReturnType{};
            }
        }

        inline constexpr unsigned int paramsCount() const noexcept { return Traits::paramsCount; }

    private:
        FuncType* mFunction{};
};

template <typename Func>
class Delegate<Func, EnableIF_T<isMemberFunction_v<Func>>> {
    NO_COPY_AND_MOVE(Delegate);

    using Traits     = MemberFunctionTrait<Func>;
    using ReturnType = Traits::ReturnType;
    using ClassType  = Traits::ClassType;
    using ParamList  = Traits::ParamList;
    using FuncType   = Traits::Type;

    public:
        Delegate() noexcept = default;
        ~Delegate() noexcept = default;

        template <typename ... Args>
        inline ReturnType operator()(Args&&... args) const {
            if constexpr (isVoid_v<ReturnType>)
                execute(std::forward<Args>(args)...);

            else
                return execute(std::forward<Args>(args)...);
        }

        void bind(ClassType* instance, Func method) noexcept {
            mInstance = instance;
            mFunction = method;
        }

        template <typename ... Args>
        inline ReturnType execute(Args&&... args) const  {
            static_assert(
                Traits::paramsCount == sizeof...(Args),
                "Delegate::execute() arguments count mismatch"
            );
            static_assert(
                areCallCompatible_v<ParamList, TypeList<Args...>>,
                "Delegate::execute() incompatible argument types"
            );

            if constexpr (isVoid_v<ReturnType>) {
                if (mInstance and mFunction)
                    (mInstance->*mFunction)(std::forward<Args>(args)...);
            }
            else {
                if (mInstance and mFunction)
                    return (mInstance->*mFunction)(std::forward<Args>(args)...);

                return ReturnType{};
            }
        }

        inline constexpr unsigned int paramsCount() const noexcept { return Traits::paramsCount; }

    private:
        ClassType* mInstance{};
        FuncType   mFunction{};
};