/*
© Copyright CERN 2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _CUSTOM_TYPE_TRAITS
#define _CUSTOM_TYPE_TRAITS

#include <string>

// clang-format off
template<class...>
struct make_void { using type = void; };
template<typename... T> using void_t = typename make_void<T...>::type;

template<class, class T>
struct type_sink { typedef T type; };
template<class E, class T = void>
using type_sink_t = typename type_sink<E, T>::type;


template<class T>
struct is_pair : std::false_type
{
};
template<class T, class U>
struct is_pair<std::pair<T, U>> : std::true_type
{
};
template<class T>
struct is_pair_d : is_pair<typename std::decay<T>::type>
{
};

template<class T, class = void>
struct has_mapped_type : std::false_type {};
template<class T>
struct has_mapped_type<T, type_sink_t<typename T::mapped_type>> : std::true_type {};

template<class T, class = void>
struct is_iterable_container : std::false_type {};
template<typename T>
// For some reason MSVC was misbehaving when using type_sink here compared to for example gcc
// Therefore, void_t was used, although type_sink should be sufficient
struct is_iterable_container<T, void_t<
	decltype(std::begin(std::declval<T>())),
	decltype(std::end(std::declval<T>()))
	>> : std::true_type {};
	
template<class T, class = void>
struct is_string : std::false_type{};
template<typename T>
struct is_string<T, typename std::enable_if<std::is_same<T, std::string>::value || 
	std::is_same<char const *, typename std::decay<T>::type>::value || 
	std::is_same<char *, typename std::decay<T>::type>::value
	>::type> : std::true_type {};
// clang-format on

#endif
