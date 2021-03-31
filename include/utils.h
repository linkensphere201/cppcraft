#pragma once

#include <tuple>

namespace cp1craft {
namespace utils {
namespace tuple_unpack_helper {

// for c++11
template<int... Is>
struct index {};

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template<int... Is>
struct gen_seq<0, Is...> {
  typedef index<Is...> type;
};

template<typename Fn, typename... Args>
struct callable {
  callable(Fn &&f, Args &&... a)
      : fn(std::forward<Fn>(f)), params(std::forward<Args>(a)...) {}
  std::function<void(Args...)> fn;
  std::tuple<Args...> params;
  void call() {
    return callFunc(
        typename gen_seq<sizeof...(Args)>::
            type{}); // sizeof... to fetch parameter size of a parameter pack
  }
  template<int... S>
  void callFunc(index<S...>) {
    fn(std::get<S>(params)...); // parameter unpack --> fn(std::get<0>(params),
                                // std::get<1>(params), ...);
  }
};

template<typename Fn, typename... Args>
callable<Fn, Args...> make_callable(Fn &&callback, Args &&... params) {
  return callable<Fn, Args...>(std::forward<Fn>(callback),
                               std::forward<Args>(params)...);
}
// for c++14, we can use std::integer_sequence instead
// for c++17, we can use std::apply or std::invoke instead

} // namespace tuple_unpack_helper
} // namespace utils
} // namespace cp1craft
