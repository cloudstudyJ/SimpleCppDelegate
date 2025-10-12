#pragma once

namespace __TypeTraitsBase {
    template <typename T, T Value>
    struct ConstantValue { static inline constexpr T value = Value; };

    using FalseType = ConstantValue<bool, false>;
    using TrueType  = ConstantValue<bool, true>;

    template <typename T> struct RemoveConst          { using Type = T; };
    template <typename T> struct RemoveConst<T const> { using Type = T; };

    template <typename T> struct RemoveReference      { using Type = T; };
    template <typename T> struct RemoveReference<T&>  { using Type = T; };
    template <typename T> struct RemoveReference<T&&> { using Type = T; };

    template <typename ... Types>
    struct TypeList {};

    template <bool Condition, typename, typename B>
    struct TypeIF { using Type = B; };
    template <typename A, typename B>
    struct TypeIF<true, A, B> { using Type = A; };

    template <bool, typename = void>
    struct EnableIF {};
    template <typename T>
    struct EnableIF<true, T> { using Type = T; };

    template <typename Ret, typename ... Args>
    struct FuncTraitsBase {
        using ReturnType = Ret;
        using ArgsList   = TypeList<Args...>;

        static inline constexpr unsigned int argsCount = sizeof...(Args);
    };

    template <typename>
    struct FuncTraits {};
    template <typename Ret, typename ... Args>
    struct FuncTraits<Ret (Args...)>
        : FuncTraitsBase<Ret, Args...> { using Type = Ret (Args...); };
    template <typename Ret, typename ... Args>
    struct FuncTraits<Ret (*)(Args...)>
        : FuncTraits<Ret (Args...)> {};

    template <typename>
    struct MemberFuncTraits {};
    template <typename Ret, typename Class, typename ... Args>
    struct MemberFuncTraits<Ret (Class::*)(Args...)>
        : FuncTraitsBase<Ret, Args...> {
        using ClassType = Class;
        using Type      = Ret (ClassType::*)(Args...);
    };
    // add specializations (const, &, &&, noexcept)

    template <typename, typename>
    struct IsSame
        : FalseType {};
    template <typename T>
    struct IsSame<T, T>
        : TrueType {};

    template <typename, typename>
    struct AreSame
        : FalseType {};
    template <typename ... Types1, typename ... Types2>
    struct AreSame<TypeList<Types1...>, TypeList<Types2...>>
        : TypeIF<
            (sizeof...(Types1) == sizeof...(Types2)) and
            (IsSame<Types1, Types2>::value and ...),
            TrueType,
            FalseType
          >::Type {};

    template <typename>
    struct IsFunction
        : FalseType {};
    template <typename Ret, typename ... Args>
    struct IsFunction<Ret (Args...)>
        : TrueType {};
    template <typename Ret, typename ... Args>
    struct IsFunction<Ret (*)(Args...)>
        : IsFunction<Ret (Args...)> {};

    template <typename>
    struct IsMemberFunction
        : FalseType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...)>
        : TrueType {};
    // add specializations (const, &, &&, noexcept)
}

template <typename T> using RemoveConst     = typename __TypeTraitsBase::RemoveConst<T>::Type;
template <typename T> using RemoveReference = typename __TypeTraitsBase::RemoveReference<T>::Type;
template <typename T> using RemoveCR        = RemoveConst<RemoveReference<T>>;

template <typename ... Types>
using TypeList = __TypeTraitsBase::TypeList<Types...>;

template <bool Condition, typename A, typename B>
using TypeIF = typename __TypeTraitsBase::TypeIF<Condition, A, B>::Type;

template <bool Condition, typename T = void>
using EnableIF = typename __TypeTraitsBase::EnableIF<Condition, T>::Type;

template <typename T>
using FuncTraits = __TypeTraitsBase::FuncTraits<T>;
template <typename T>
using MemberFuncTraits = __TypeTraitsBase::MemberFuncTraits<T>;

template <typename A, typename B>
static inline constexpr bool isSame = __TypeTraitsBase::IsSame<A, B>::value;
template <typename TypeList1, typename TypeList2>
static inline constexpr bool areSame = __TypeTraitsBase::AreSame<TypeList1, TypeList2>::value;

template <typename T>
static inline constexpr bool isFunction = __TypeTraitsBase::IsFunction<T>::value;
template <typename T>
static inline constexpr bool isMemberFunction = __TypeTraitsBase::IsMemberFunction<T>::value;