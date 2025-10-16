#pragma once

namespace __TypeTraitsBase {
    template <typename T, T Value>
    struct ConstantValue { static inline constexpr T value = Value; };

    using FalseType = ConstantValue<bool, false>;
    using TrueType  = ConstantValue<bool, true>;

    template <typename T> struct RemoveConst          { using Type = T; };
    template <typename T> struct RemoveConst<T const> { using Type = T; };
    template <typename T> using RemoveConst_T = typename RemoveConst<T>::Type;

    template <typename T> struct RemoveRef      { using Type = T; };
    template <typename T> struct RemoveRef<T&>  { using Type = T; };
    template <typename T> struct RemoveRef<T&&> { using Type = T; };
    template <typename T> using RemoveRef_T = typename RemoveRef<T>::Type;

    template <typename T> struct RemovePtr           { using Type = T; };
    template <typename T> struct RemovePtr<T*>       { using Type = T; };
    template <typename T> struct RemovePtr<T* const> { using Type = T; };
    template <typename T> using RemovePtr_T = typename RemovePtr<T>::Type;

    template <typename T> struct RemoveCR { using Type = RemoveConst_T<RemoveRef_T<T>>; };
    template <typename T> using RemoveCR_T = typename RemoveCR<T>::Type;

    template <typename ... Types>
    struct TypeList {};

    template <bool Condition, typename, typename B>
    struct TypeIF { using Type = B; };
    template <typename A, typename B>
    struct TypeIF<true, A, B> { using Type = A; };
    template <bool Condition, typename A, typename B>
    using TypeIF_T = typename TypeIF<Condition, A, B>::Type;

    template <bool, typename = void>
    struct EnableIF {};
    template <typename T>
    struct EnableIF<true, T> { using Type = T; };
    template <bool Condition, typename T = void>
    using EnableIF_T = typename EnableIF<Condition, T>::Type;

    template <typename>   struct IsConst         : FalseType {};
    template <typename T> struct IsConst<T const>: TrueType  {};

    template <typename>   struct IsLValueRef    : FalseType {};
    template <typename T> struct IsLValueRef<T&>: TrueType  {};
    template <typename>   struct IsRValueRef     : FalseType {};
    template <typename T> struct IsRValueRef<T&&>: TrueType  {};

    template <typename T>
    struct IsRef
        : ConstantValue<bool, IsLValueRef<T>::value or IsRValueRef<T>::value> {};

    template <typename>   struct IsPtr          : FalseType {};
    template <typename T> struct IsPtr<T*>      : TrueType  {};
    template <typename T> struct IsPtr<T* const>: TrueType  {};

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
        : TypeIF_T<
            (sizeof...(Types1) == sizeof...(Types2)) and
            (IsSame<Types1, Types2>::value and ...),
            TrueType,
            FalseType
          > {};

    template <typename T>
    struct IsVoid
        : TypeIF_T<IsSame<T, void>::value, TrueType, FalseType> {};

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
        EnableIF_T<
            (!IsPtr<Param>::value and !IsRef<Param>::value) and
            !IsPtr<RemoveRef_T<Arg>>::value
        >
    >: TrueType {};

    template <typename, typename, typename = void>
    struct IsLValueReferenceTypeCompatible
        : FalseType {};
    template <typename Param, typename Arg>
    struct IsLValueReferenceTypeCompatible<
        Param, Arg,
        EnableIF_T<
            (!IsConst<Param>::value and IsLValueRef<Param>::value) and
            (!IsPtr<RemoveRef_T<Arg>>::value and IsLValueRef<Arg>::value and !IsConst<Arg>::value)
        >
    >: TrueType {};
    template <typename Param, typename Arg>
    struct IsLValueReferenceTypeCompatible<
        Param, Arg,
        EnableIF_T<
            (IsConst<Param>::value and IsLValueRef<Param>::value) and
            !IsPtr<RemoveRef_T<Arg>>::value
        >
    >: TrueType {};

    template <typename, typename, typename = void>
    struct IsRValueReferenceTypeCompatible
        : FalseType {};
    template <typename Param, typename Arg>
    struct IsRValueReferenceTypeCompatible<
        Param, Arg,
        EnableIF_T<
            IsRValueRef<Param>::value and
            (!IsPtr<RemoveRef_T<Arg>>::value and !IsLValueRef<Arg>::value)
        >
    >: TrueType {};

    template <typename, typename, typename = void>
    struct IsPointerTypeCompatible
        : FalseType {};
    template <typename Param, typename Arg>
    struct IsPointerTypeCompatible<
        Param, Arg,
        EnableIF_T<
            IsPtr<RemoveRef_T<Param>>::value and
            (IsPtr<RemoveRef_T<Arg>>::value and !IsConst<RemovePtr_T<RemoveRef_T<Arg>>>::value or IsConst<RemovePtr_T<RemoveRef_T<Param>>>::value)
        >
    >: TrueType {};
}

template <typename T> using RemoveConst_T = __TypeTraitsBase::RemoveConst_T<T>;
template <typename T> using RemoveRef_T   = __TypeTraitsBase::RemoveRef_T<T>;
template <typename T> using RemovePtr_T   = __TypeTraitsBase::RemovePtr_T<T>;
template <typename T> using RemoveCR_T    = __TypeTraitsBase::RemoveCR_T<T>;

template <typename ... Types>
using TypeList = __TypeTraitsBase::TypeList<Types...>;

template <bool Condition, typename A, typename B>
using TypeIF_T = __TypeTraitsBase::TypeIF_T<Condition, A, B>;

template <bool Condition, typename T = void>
using EnableIF_T = __TypeTraitsBase::EnableIF_T<Condition, T>;

template <typename T> using FuncTraits       = __TypeTraitsBase::FuncTraits<T>;
template <typename T> using MemberFuncTraits = __TypeTraitsBase::MemberFuncTraits<T>;

template <typename T> static inline constexpr bool isConst_v           = __TypeTraitsBase::IsConst<T>::value;
template <typename T> static inline constexpr bool isLValueReference_v = __TypeTraitsBase::IsLValueRef<T>::value;
template <typename T> static inline constexpr bool isRValueReference_v = __TypeTraitsBase::IsRValueRef<T>::value;
template <typename T> static inline constexpr bool isReference_v       = __TypeTraitsBase::IsRef<T>::value;
template <typename T> static inline constexpr bool isPointer_v         = __TypeTraitsBase::IsPtr<T>::value;
template <typename T> static inline constexpr bool isFunction_v        = __TypeTraitsBase::IsFunction<T>::value;
template <typename T> static inline constexpr bool isMemberFunction_v  = __TypeTraitsBase::IsMemberFunction<T>::value;
template <typename T> static inline constexpr bool isVoid_v            = __TypeTraitsBase::IsVoid<T>::value;

template <typename A, typename B>
static inline constexpr bool isSame_v = __TypeTraitsBase::IsSame<A, B>::value;
template <typename TypeList1, typename TypeList2>
static inline constexpr bool areSame_v = __TypeTraitsBase::AreSame<TypeList1, TypeList2>::value;