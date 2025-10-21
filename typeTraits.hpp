#pragma once

/**
 * TODO
 *   1. check array type compatible?
 *   2. if Param is class ptr -> check base
 *   3. check convertible to other types
 */

namespace __TypeTraitsBase {
    #define IN_DEFINE_VALUE_TRAIT_1_PARAM(aliasName, traitName)         \
        template <typename T>                                           \
        static inline constexpr bool aliasName = traitName<T>::value

    #define IN_DEFINE_VALUE_TRAIT_2_PARAMS(aliasName, traitName)        \
        template <typename A, typename B>                               \
        static inline constexpr bool aliasName = traitName<A, B>::value

    #define IN_GENERATE_IS_FUNCTION_SPECIALIZATION(qualifiers, ...)             \
        template <typename Ret, typename ... Params>                            \
        struct IsFunction<Ret (Params... __VA_OPT__(,) __VA_ARGS__) qualifiers> \
            : TrueType {}

    #define IN_GENERATE_IS_FUNCTION_PTR_SPECIALIZATION(qualifiers, ...)                 \
        template <typename Ret, typename ... Params>                                    \
        struct IsFunctionPtr<Ret (*)(Params... __VA_OPT__(,) __VA_ARGS__) qualifiers>   \
            : TrueType {}

    #define IN_GENERATE_IS_FUNCTION_REF_SPECIALIZATION(qualifiers, ...)                 \
        template <typename Ret, typename ... Params>                                    \
        struct IsFunctionRef<Ret (&)(Params... __VA_OPT__(,) __VA_ARGS__) qualifiers>   \
            : TrueType {}

    #define IN_GENERATE_IS_MEMBER_FUNCTION_SPECIALIZATION(qualifiers, ...)                      \
        template <typename Ret, typename Class, typename ... Params>                            \
        struct IsMemberFunction<Ret (Class::*)(Params... __VA_OPT__(,) __VA_ARGS__) qualifiers> \
            : TrueType {}

    #define IN_DEFINE_IS_FUNCTION_SPECIALIZATION(qualifiers)        \
        IN_GENERATE_IS_FUNCTION_SPECIALIZATION(qualifiers);         \
        IN_GENERATE_IS_FUNCTION_SPECIALIZATION(qualifiers, ...);

    #define IN_DEFINE_IS_FUNCTION_PTR_SPECIALIZATION(qualifiers)        \
        IN_GENERATE_IS_FUNCTION_PTR_SPECIALIZATION(qualifiers);         \
        IN_GENERATE_IS_FUNCTION_PTR_SPECIALIZATION(qualifiers, ...);

    #define IN_DEFINE_IS_FUNCTION_REF_SPECIALIZATION(qualifiers)        \
        IN_GENERATE_IS_FUNCTION_REF_SPECIALIZATION(qualifiers);         \
        IN_GENERATE_IS_FUNCTION_REF_SPECIALIZATION(qualifiers, ...);

    #define IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(qualifiers)     \
        IN_GENERATE_IS_MEMBER_FUNCTION_SPECIALIZATION(qualifiers);      \
        IN_GENERATE_IS_MEMBER_FUNCTION_SPECIALIZATION(qualifiers, ...);

    #define IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(funcType)           \
        template <typename Ret, typename Class, typename ... Params>    \
        struct MemberFunctionTrait<funcType>                            \
            : FunctionTraitBase<Ret, Params...> {                       \
            using ClassType = Class;                                    \
            using Type      = funcType;                                 \
        }

    template <typename T, T Value>
    struct ConstantValue { static inline constexpr T value = Value; };

    template <bool Value>
    using BoolConstant = ConstantValue<bool, Value>;

    using FalseType = BoolConstant<false>;
    using TrueType  = BoolConstant<true>;

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

    template <bool Condition, typename, typename B> struct TypeIF             { using Type = B; };
    template <typename A, typename B>               struct TypeIF<true, A, B> { using Type = A; };
    template <bool Condition, typename A, typename B>
    using TypeIF_T = typename TypeIF<Condition, A, B>::Type;

    template <bool, typename = void> struct EnableIF          {};
    template <typename T>            struct EnableIF<true, T> { using Type = T; };
    template <bool Condition, typename T = void>
    using EnableIF_T = typename EnableIF<Condition, T>::Type;

    #pragma region(Is__Traits)
    template <typename>   struct IsConst         : FalseType {};
    template <typename T> struct IsConst<T const>: TrueType  {};
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isConst_v, IsConst);

    template <typename>   struct IsLRef    : FalseType {};
    template <typename T> struct IsLRef<T&>: TrueType  {};
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isLRef_v, IsLRef);

    template <typename>   struct IsRRef     : FalseType {};
    template <typename T> struct IsRRef<T&&>: TrueType  {};
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isRRef_v, IsRRef);

    template <typename T> struct IsRef: BoolConstant<isLRef_v<T> or isRRef_v<T>> {};
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isRef_v, IsRef);

    template <typename>   struct IsPtr          : FalseType {};
    template <typename T> struct IsPtr<T*>      : TrueType  {};
    template <typename T> struct IsPtr<T* const>: TrueType  {};
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isPtr_v, IsPtr);

    template <typename> struct IsFunction: FalseType {};
    IN_DEFINE_IS_FUNCTION_SPECIALIZATION();
    IN_DEFINE_IS_FUNCTION_SPECIALIZATION(noexcept);
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isFunction_v, IsFunction);

    template <typename> struct IsFunctionPtr: FalseType {};
    IN_DEFINE_IS_FUNCTION_PTR_SPECIALIZATION();
    IN_DEFINE_IS_FUNCTION_PTR_SPECIALIZATION(noexcept);
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isFunctionPtr_v, IsFunctionPtr);

    template <typename> struct IsFunctionRef: FalseType {};
    IN_DEFINE_IS_FUNCTION_REF_SPECIALIZATION();
    IN_DEFINE_IS_FUNCTION_REF_SPECIALIZATION(noexcept);
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isFunctionRef_v, IsFunctionRef);

    template <typename> struct IsMemberFunction: FalseType {};
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION();
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(const);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(noexcept);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(const noexcept);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(&);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(const &);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(& noexcept);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(const & noexcept);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(&&);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(const &&);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(&& noexcept);
    IN_DEFINE_IS_MEMBER_FUNCTION_SPECIALIZATION(const && noexcept);
    IN_DEFINE_VALUE_TRAIT_1_PARAM(isMemberFunction_v, IsMemberFunction);

    template <typename, typename> struct IsSame      : FalseType {};
    template <typename T>         struct IsSame<T, T>: TrueType  {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isSame_v, IsSame);

    template <typename, typename>
    struct AreSame
        : FalseType {};
    template <typename ... Types1, typename ... Types2>
    struct AreSame<TypeList<Types1...>, TypeList<Types2...>>
        : BoolConstant<
            (sizeof...(Types1) == sizeof...(Types2)) and
            (isSame_v<Types1, Types2> and ...)
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(areSame_v, AreSame);
    #pragma endregion

    template <typename Ret, typename ... Params>
    struct FunctionTraitBase {
        using ReturnType = Ret;
        using ParamList  = TypeList<Params...>;

        static inline constexpr unsigned int paramsCount = sizeof...(Params);
    };

    template <typename> struct FunctionTrait {};
    template <typename Ret, typename ... Params>
    struct FunctionTrait<Ret (Params...)>
        : FunctionTraitBase<Ret, Params...> { using Type = Ret (Params...); };
    template <typename Ret, typename ... Params>
    struct FunctionTrait<Ret (*)(Params...)>
        : FunctionTraitBase<Ret, Params...> { using Type = Ret (*)(Params...); };
    template <typename Ret, typename ... Params>
    struct FunctionTrait<Ret (&)(Params...)>
        : FunctionTraitBase<Ret, Params...> { using Type = Ret (&)(Params...); };

    template <typename> struct MemberFunctionTrait {};
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...));
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) const);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) noexcept);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) const noexcept);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) &);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) const &);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) & noexcept);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) const & noexcept);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) &&);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) const &&);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) && noexcept);
    IN_MEMBER_FUNCTION_TRAIT_SPECIALIZATION(Ret (Class::*)(Params...) const && noexcept);

    template <typename Param, typename Arg>
    struct IsValueCallCompatible
        : BoolConstant<
            (!isPtr_v<Param> and !isRef_v<Param>) and
            !isPtr_v<RemoveRef_T<Arg>>
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isValueCallCompatible_v, IsValueCallCompatible);

    template <typename Param, typename Arg>
    struct IsLRefCallCompatible
        : BoolConstant<
            // check non-const l-value reference type of Param
            ((!isConst_v<Param> and isLRef_v<Param>) and
            (!isPtr_v<RemoveRef_T<Arg>> and isLRef_v<Arg> and !isConst_v<Arg>))
            or
            // check const l-value reference type of Param
            ((isConst_v<Param> and isLRef_v<Param>) and
            !isPtr_v<RemoveRef_T<Arg>>)
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isLRefCallCompatible_v, IsLRefCallCompatible);

    template <typename Param, typename Arg>
    struct IsRRefCallCompatible
        : BoolConstant<
            isRRef_v<Param> and
            (!isPtr_v<RemoveRef_T<Arg>> and !isLRef_v<Arg>)
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isRRefCallCompatible_v, IsRRefCallCompatible);

    template <typename Param, typename Arg>
    struct IsRefCallCompatible
        : BoolConstant<
            isLRefCallCompatible_v<Param, Arg> or
            isRRefCallCompatible_v<Param, Arg>
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isRefCallCompatible_v, IsRefCallCompatible);

    template <typename Param, typename Arg>
    struct IsPtrCallCompatible
        : BoolConstant<
            (isPtr_v<RemoveRef_T<Param>> and isPtr_v<RemoveRef_T<Arg>>) and
            (!isConst_v<RemovePR_T<Arg>> or isConst_v<RemovePR_T<Param>>)
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isPtrCallCompatible_v, IsPtrCallCompatible);

    template <typename Param, typename Arg>
    struct IsCallCompatible
        : BoolConstant<
            isValueCallCompatible_v<Param, Arg> or
            isRefCallCompatible_v<Param,Arg>    or
            isPtrCallCompatible_v<Param, Arg>
          > {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(isCallCompatible_v, IsCallCompatible);

    template <typename, typename>
    struct AreCallCompatible
        : FalseType {};
    template <typename ... Types1, typename ... Types2>
    struct AreCallCompatible<TypeList<Types1...>, TypeList<Types2...>>
        : BoolConstant<(isCallCompatible_v<Types1, Types2> and ...)> {};
    IN_DEFINE_VALUE_TRAIT_2_PARAMS(areCallCompatible_v, AreCallCompatible);
}

#define EX_DEFINE_VALUE_TRAIT_1_PARAM(aliasName, Param)                         \
    template <typename Param>                                                   \
    static inline constexpr bool aliasName = __TypeTraitsBase::aliasName<Param>

#define EX_DEFINE_VALUE_TRAIT_2_PARAMS(aliasName, Param1, Param2)                           \
    template <typename Param1, typename Param2>                                             \
    static inline constexpr bool aliasName = __TypeTraitsBase::aliasName<Param1, Param2>

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

template <typename T> using FunctionTrait       = __TypeTraitsBase::FunctionTrait<T>;
template <typename T> using MemberFunctionTrait = __TypeTraitsBase::MemberFunctionTrait<T>;

EX_DEFINE_VALUE_TRAIT_1_PARAM(isConst_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isLRef_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isRRef_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isRef_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isPtr_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isFunction_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isFunctionPtr_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isFunctionRef_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isMemberFunction_v, Type);
EX_DEFINE_VALUE_TRAIT_1_PARAM(isVoid_v, Type);

EX_DEFINE_VALUE_TRAIT_2_PARAMS(isSame_v, Type1, Type2);
EX_DEFINE_VALUE_TRAIT_2_PARAMS(areSame_v, TypeList1, TypeList2);
EX_DEFINE_VALUE_TRAIT_2_PARAMS(isCallCompatible_v, Param, Arg);
EX_DEFINE_VALUE_TRAIT_2_PARAMS(areCallCompatible_v, TypeList1, TypeList2);