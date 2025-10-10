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

    template <typename, typename>
    struct IsSame
        : FalseType {};
    template <typename T>
    struct IsSame<T, T>
        : TrueType {};

    template <typename TypeList1, typename TypeList2>
    struct AreSame {};
    template <typename Head1, typename Head2, typename ... Types1, typename ... Types2>
    struct AreSame<TypeList<Head1, Types1...>, TypeList<Head2, Types2...>>
        : TypeIF<IsSame<Head1, Head2>::value, AreSame<TypeList<Types1...>, TypeList<Types2...>>, FalseType>::Type {};
    template <>
    struct AreSame<TypeList<>, TypeList<>>
        : TrueType {};
}

template <typename T> using RemoveConst     = typename __TypeTraitsBase::RemoveConst<T>::Type;
template <typename T> using RemoveReference = typename __TypeTraitsBase::RemoveReference<T>::Type;
template <typename T> using RemoveCR        = RemoveConst<RemoveReference<T>>;

template <typename ... Types>
using TypeList = __TypeTraitsBase::TypeList<Types...>;

template <bool Condition, typename A, typename B>
using TypeIF = typename __TypeTraitsBase::TypeIF<Condition, A, B>::Type;

template <typename A, typename B>
static inline constexpr bool isSame = __TypeTraitsBase::IsSame<A, B>::value;

template <typename TypeList1, typename TypeList2>
static inline constexpr bool areSame = __TypeTraitsBase::AreSame<TypeList1, TypeList2>::value;