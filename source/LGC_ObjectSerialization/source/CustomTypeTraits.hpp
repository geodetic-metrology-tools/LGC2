/*
© Copyright CERN 2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _CUSTOM_TYPE_TRAITS
#define _CUSTOM_TYPE_TRAITS

#include <string>

// clang-format off

/* ######### HELPERS ######### */
// It is used to silently reject the specialization if typename T::type is ill-formed
template<class...>
struct make_void { using type = void; };
template<typename... T> using void_t = typename make_void<T...>::type;

template<class, class T>
struct type_sink { typedef T type; };
template<class E, class T = void>
using type_sink_t = typename type_sink<E, T>::type;

// helper for value
template< class T, class U >
constexpr bool is_same_v = std::is_same<T, U>::value;

// get innermost type (recursive)
template<class T, typename = void>
struct inner_type {
	using type = T;
};
template<class T>
struct inner_type<T, void_t<typename T::value_type>>
	: inner_type<typename T::value_type> {};
template<class T>
using inner_type_t = typename inner_type<T>::type;


/* ######### CUSTOM TYPE TRAITS ######### */
// is pair
template<class T>
struct is_pair : std::false_type
{
};
template<class T, class U>
struct is_pair<std::pair<T, U>> : std::true_type
{
};
template<class T>
struct is_pair_t : is_pair<typename std::decay<T>::type>
{
};

// is map
template<class T, class = void>
struct has_mapped_type : std::false_type {};
template<class T>
struct has_mapped_type<T, type_sink_t<typename T::mapped_type>> : std::true_type {};

// is iterable
template<class T, class = void>
struct is_iterable_container : std::false_type {};
template<typename T>
// For some reason MSVC was misbehaving when using type_sink here compared to for example gcc
// Therefore, void_t was used, although type_sink should be sufficient
struct is_iterable_container<T, void_t<
	decltype(std::begin(std::declval<T>())),
	decltype(std::end(std::declval<T>()))
	>> : std::true_type {};

// is string (and char variants)
template<class T, class = void>
struct is_string : std::false_type{};
template<typename T>
struct is_string<T, typename std::enable_if<std::is_same_v<T, std::string> || 
	std::is_same_v<char const *, typename std::decay<T>::type> || 
	std::is_same_v<char *, typename std::decay<T>::type>
	>::type> : std::true_type {};

// clang-format on
#endif
