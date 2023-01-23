/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2023-01-04
 *===============================*/

#ifndef DS_TYPE_TRAITS_HPP
#define DS_TYPE_TRAITS_HPP

#include "./string.hpp"
#include "error.hpp"
#include <tuple>
#include <type_traits>

namespace ds {

template <typename Base, typename T>
using same_type =
    typename std::is_same<Base, typename std::common_type<Base, T>::type>::type;

/**
 * Check if all passed arguments are the same type as T
 **/
template <typename T, typename... Args>
using all_same_type = typename std::enable_if<
    std::conjunction<same_type<T, Args>...>::value>::type;

/**
 * Check if type T is a string type
 **/
template <typename T>
using is_string_type = typename std::disjunction<
    std::is_convertible<T, const char*>,      //
    std::is_convertible<T, ds::string>,       //
    std::is_convertible<T, const ds::string&> //
    >::type;

/**
 * Check if all passed arguments are string type
 **/
template <typename... Args>
using all_string_type = typename std::enable_if<
    std::conjunction<is_string_type<Args>...>::value>::type;

template <typename T>
using is_class_type = typename std::enable_if<std::is_class<T>::value>::type;

template <typename T>
using is_not_class_type =
    typename std::enable_if<!std::is_class<T>::value>::type;

template <typename T>
using is_int = typename std::enable_if<std::is_integral<T>::value>::type;

template <typename T> struct has_copy_method {
private:
  template <typename U>
  static auto test(opt_err)
      -> decltype(std::declval<U>().copy() == 1, std::__true_type());
  template <typename> static std::__false_type test(...);

public:
  static constexpr bool value =
      std::is_same<decltype(test<T>(0)), std::__true_type>::value;
};

} // namespace ds

#endif
