#pragma once

namespace __TypeTraitsBase {
    template <typename T, T Value>
    struct ConstantValue { static inline constexpr T value = Value; };

    using FalseType = ConstantValue<bool, false>;
    using TrueType  = ConstantValue<bool, true>;

    #pragma region(Remove__Traits)
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

    template <typename T> struct RemovePR { using Type = RemovePtr_T<RemoveRef_T<T>>; };
    template <typename T> using RemovePR_T = typename RemovePR<T>::Type;
    #pragma endregion

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

    #pragma region(Is__Traits)
    template <typename>   struct IsConst         : FalseType {};
    template <typename T> struct IsConst<T const>: TrueType  {};
    template <typename T> static inline constexpr bool isConst_v = IsConst<T>::value;

    template <typename>   struct IsLValueRef    : FalseType {};
    template <typename T> struct IsLValueRef<T&>: TrueType  {};
    template <typename T> static inline constexpr bool isLValueRef_v = IsLValueRef<T>::value;

    template <typename>   struct IsRValueRef     : FalseType {};
    template <typename T> struct IsRValueRef<T&&>: TrueType  {};
    template <typename T> static inline constexpr bool isRValueRef_v = IsRValueRef<T>::value;

    template <typename T>
    struct IsRef
        : ConstantValue<bool, isLValueRef_v<T> or isRValueRef_v<T>> {};
    template <typename T> static inline constexpr bool isRef_v = IsRef<T>::value;

    template <typename>   struct IsPtr          : FalseType {};
    template <typename T> struct IsPtr<T*>      : TrueType  {};
    template <typename T> struct IsPtr<T* const>: TrueType  {};
    template <typename T> static inline constexpr bool isPtr_v = IsPtr<T>::value;

    template <typename>
    struct IsFunction
        : FalseType {};
    template <typename Ret, typename ... Args>
    struct IsFunction<Ret (Args...)>
        : TrueType {};
    template <typename Ret, typename ... Args>
    struct IsFunction<Ret (*)(Args...)>
        : IsFunction<Ret (Args...)> {};
    template <typename T> static inline constexpr bool isFunction_v = IsFunction<T>::value;

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
    template <typename T> static inline constexpr bool isMemberFunction_v = IsMemberFunction<T>::value;

    template <typename, typename>
    struct IsSame
        : FalseType {};
    template <typename T>
    struct IsSame<T, T>
        : TrueType {};
    template <typename A, typename B>
    static inline constexpr bool isSame_v = IsSame<A, B>::value;

    template <typename, typename>
    struct AreSame
        : FalseType {};
    template <typename ... Types1, typename ... Types2>
    struct AreSame<TypeList<Types1...>, TypeList<Types2...>>
        : TypeIF_T<
            (sizeof...(Types1) == sizeof...(Types2)) and
            (isSame_v<Types1, Types2> and ...),
            TrueType,
            FalseType
          > {};
    template <typename TypeList1, typename TypeList2>
    static inline constexpr bool areSame_v = AreSame<TypeList1, TypeList2>::value;

    template <typename T>
    struct IsVoid
        : TypeIF_T<isSame_v<T, void>, TrueType, FalseType> {};
    template <typename T> static inline constexpr bool isVoid_v = IsVoid<T>::value;
    #pragma endregion

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

    template <typename Param, typename Arg>
    struct IsValueTypeCompatible
        : TypeIF_T<
            (!isPtr_v<Param> and !isRef_v<Param>) and
            !isPtr_v<RemoveRef_T<Arg>>,
            TrueType,
            FalseType
          > {};
    template <typename Param, typename Arg>
    static inline constexpr bool isValueTypeCompatible_v = IsValueTypeCompatible<Param, Arg>::value;

    template <typename Param, typename Arg>
    struct IsLValueRefTypeCompatible
        : TypeIF_T<
            ((!isConst_v<Param> and isLValueRef_v<Param>) and
            (!isPtr_v<RemoveRef_T<Arg>> and isLValueRef_v<Arg> and !isConst_v<Arg>))
            or
            ((isConst_v<Param> and isLValueRef_v<Param>) and
            !isPtr_v<RemoveRef_T<Arg>>),
            TrueType,
            FalseType
          > {};
    template <typename Param, typename Arg>
    static inline constexpr bool isLValueRefTypeCompatible_v = IsLValueRefTypeCompatible<Param, Arg>::value;

    template <typename Param, typename Arg>
    struct IsRValueRefTypeCompatible
        : TypeIF_T<
            isRValueRef_v<Param> and
            (!isPtr_v<RemoveRef_T<Arg>> and !isLValueRef_v<Arg>),
            TrueType,
            FalseType
          > {};
    template <typename Param, typename Arg>
    static inline constexpr bool isRValueRefTypeCompatible_v = IsRValueRefTypeCompatible<Param, Arg>::value;

    template <typename Param, typename Arg>
    struct IsRefTypeCompatible
        : TypeIF_T<
            IsLValueRefTypeCompatible<Param, Arg>::value or
            IsRValueRefTypeCompatible<Param, Arg>::value,
            TrueType,
            FalseType
          > {};
    template <typename Param, typename Arg>
    static inline constexpr bool isRefTypeCompatible_v = IsRefTypeCompatible<Param, Arg>::value;

    template <typename Param, typename Arg>
    struct IsPtrTypeCompatible
        : TypeIF_T<
            (isPtr_v<RemoveRef_T<Param>> and isPtr_v<RemoveRef_T<Arg>>) and
            (!isConst_v<RemovePR_T<Arg>> or isConst_v<RemovePR_T<Param>>),
            TrueType,
            FalseType
          > {};
    template <typename Param, typename Arg>
    static inline constexpr bool isPtrTypeCompatible_v = IsPtrTypeCompatible<Param, Arg>::value;

    template <typename Param, typename Arg>
    struct IsTypeCompatible
        : TypeIF_T<
            IsValueTypeCompatible<Param, Arg>::value or
            IsRefTypeCompatible<Param, Arg>::value or
            IsPtrTypeCompatible<Param, Arg>::value,
            TrueType,
            FalseType
          > {};
    template <typename Param, typename Arg>
    static inline constexpr bool isTypeCompatible_v = IsTypeCompatible<Param, Arg>::value;
}

template <typename T> using RemoveConst_T = __TypeTraitsBase::RemoveConst_T<T>;
template <typename T> using RemoveRef_T   = __TypeTraitsBase::RemoveRef_T<T>;
template <typename T> using RemovePtr_T   = __TypeTraitsBase::RemovePtr_T<T>;
template <typename T> using RemoveCR_T    = __TypeTraitsBase::RemoveCR_T<T>;
template <typename T> using RemovePR_T    = __TypeTraitsBase::RemovePR_T<T>;

template <typename ... Types>
using TypeList = __TypeTraitsBase::TypeList<Types...>;

template <bool Condition, typename A, typename B>
using TypeIF_T = __TypeTraitsBase::TypeIF_T<Condition, A, B>;

template <bool Condition, typename T = void>
using EnableIF_T = __TypeTraitsBase::EnableIF_T<Condition, T>;

template <typename T> using FuncTraits       = __TypeTraitsBase::FuncTraits<T>;
template <typename T> using MemberFuncTraits = __TypeTraitsBase::MemberFuncTraits<T>;

template <typename T> static inline constexpr bool isConst_v           = __TypeTraitsBase::isConst_v<T>;
template <typename T> static inline constexpr bool isLValueReference_v = __TypeTraitsBase::isLValueRef_v<T>;
template <typename T> static inline constexpr bool isRValueReference_v = __TypeTraitsBase::isRValueRef_v<T>;
template <typename T> static inline constexpr bool isReference_v       = __TypeTraitsBase::isRef_v<T>;
template <typename T> static inline constexpr bool isPointer_v         = __TypeTraitsBase::isPtr_v<T>;
template <typename T> static inline constexpr bool isFunction_v        = __TypeTraitsBase::isFunction_v<T>;
template <typename T> static inline constexpr bool isMemberFunction_v  = __TypeTraitsBase::isMemberFunction_v<T>;
template <typename T> static inline constexpr bool isVoid_v            = __TypeTraitsBase::isVoid_v<T>;

template <typename A, typename B>
static inline constexpr bool isSame_v = __TypeTraitsBase::isSame_v<A, B>;
template <typename TypeList1, typename TypeList2>
static inline constexpr bool areSame_v = __TypeTraitsBase::areSame_v<TypeList1, TypeList2>;