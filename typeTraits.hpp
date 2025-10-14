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

    template <typename T> struct RemovePointer           { using Type = T; };
    template <typename T> struct RemovePointer<T*>       { using Type = T; };
    template <typename T> struct RemovePointer<T* const> { using Type = T; };

    template <typename T>
    struct RemoveCR { using Type = RemoveConst<typename RemoveReference<T>::Type>::Type; };

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

    template <typename>   struct IsConst         : FalseType {};
    template <typename T> struct IsConst<T const>: TrueType  {};

    template <typename>   struct IsLValueReference    : FalseType {};
    template <typename T> struct IsLValueReference<T&>: TrueType  {};
    template <typename>   struct IsRValueReference     : FalseType {};
    template <typename T> struct IsRValueReference<T&&>: TrueType  {};

    template <typename T>
    struct IsReference
        : ConstantValue<bool, IsLValueReference<T>::value or IsRValueReference<T>::value> {};

    template <typename>   struct IsPointer          : FalseType {};
    template <typename T> struct IsPointer<T*>      : TrueType  {};
    template <typename T> struct IsPointer<T* const>: TrueType  {};

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
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) const>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) noexcept>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) const noexcept>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) &>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) const &>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) & noexcept>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) const & noexcept>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) &&>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) const &&>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) && noexcept>
        : TrueType {};
    template <typename Ret, typename Class, typename ... Args>
    struct IsMemberFunction<Ret (Class::*)(Args...) const && noexcept>
        : TrueType {};

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

    template <typename, typename, typename = void>
    struct IsValueTypeCompatible
        : FalseType {};
    template <typename Param, typename Arg>
    struct IsValueTypeCompatible<
        Param, Arg,
        typename EnableIF<
            (!IsPointer<Param>::value and !IsReference<Param>::value) and
            !IsPointer<typename RemoveReference<Arg>::Type>::value
        >::Type
    >: TrueType {};

    template <typename, typename, typename = void>
    struct IsRValueReferenceTypeCompatible
        : FalseType {};
    template <typename Param, typename Arg>
    struct IsRValueReferenceTypeCompatible<
        Param, Arg,
        typename EnableIF<
            IsRValueReference<Param>::value and
            (!IsPointer<typename RemoveReference<Arg>::Type>::value and !IsLValueReference<Arg>::value)
        >::Type
    >: TrueType {};

    // TODO: add l-value reference type compatible

    template <typename, typename, typename = void>
    struct IsPointerTypeCompatible
        : FalseType {};
    template <typename Param, typename Arg>
    struct IsPointerTypeCompatible<
        Param, Arg,
        typename EnableIF<
            (IsPointer<typename RemoveReference<Param>::Type>::value and IsPointer<typename RemoveReference<Arg>::Type>::value) and
            (!IsConst<typename RemovePointer<typename RemoveReference<Arg>::Type>::Type>::value or IsConst<typename RemovePointer<typename RemoveReference<Param>::Type>::Type>::value)
        >::Type
    >: TrueType {};
}

template <typename T> using RemoveConst     = typename __TypeTraitsBase::RemoveConst<T>::Type;
template <typename T> using RemoveReference = typename __TypeTraitsBase::RemoveReference<T>::Type;
template <typename T> using RemovePointer   = typename __TypeTraitsBase::RemovePointer<T>::Type;
template <typename T> using RemoveCR        = typename __TypeTraitsBase::RemoveCR<T>::Type;

template <typename ... Types>
using TypeList = __TypeTraitsBase::TypeList<Types...>;

template <bool Condition, typename A, typename B>
using TypeIF = typename __TypeTraitsBase::TypeIF<Condition, A, B>::Type;

template <bool Condition, typename T = void>
using EnableIF = typename __TypeTraitsBase::EnableIF<Condition, T>::Type;

template <typename T> using FuncTraits       = __TypeTraitsBase::FuncTraits<T>;
template <typename T> using MemberFuncTraits = __TypeTraitsBase::MemberFuncTraits<T>;

template <typename T> static inline constexpr bool isConst           = __TypeTraitsBase::IsConst<T>::value;
template <typename T> static inline constexpr bool isLValueReference = __TypeTraitsBase::IsLValueReference<T>::value;
template <typename T> static inline constexpr bool isRValueReference = __TypeTraitsBase::IsRValueReference<T>::value;
template <typename T> static inline constexpr bool isReference       = __TypeTraitsBase::IsReference<T>::value;
template <typename T> static inline constexpr bool isPointer         = __TypeTraitsBase::IsPointer<T>::value;
template <typename T> static inline constexpr bool isFunction        = __TypeTraitsBase::IsFunction<T>::value;
template <typename T> static inline constexpr bool isMemberFunction  = __TypeTraitsBase::IsMemberFunction<T>::value;

template <typename A, typename B>
static inline constexpr bool isSame = __TypeTraitsBase::IsSame<A, B>::value;
template <typename TypeList1, typename TypeList2>
static inline constexpr bool areSame = __TypeTraitsBase::AreSame<TypeList1, TypeList2>::value;