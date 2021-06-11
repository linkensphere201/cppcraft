// keywords: index_sequence
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

// c++14
//
// integer_sequence/index_sequence/make_integer_sequence/make_index_sequence/index_sequence_for

// PART I 定义
// - std::integer_sequence: 初始定义
// ```
// template< class T, T... Ints >
// class integer_sequence;
// ```
//
// - std::index_sequence<0, 1, ...>: 类型T为size_t的std::integer_sequence
// ```
// template<std::size_t... Ints>
// using index_sequence = std::integer_sequence<std::size_t, Ints...>;
// ```
//
// - std::make_integer_sequence<T, T n>: 如下，太难表述
// ```
// template<class T, T N>
// using make_integer_sequence = std::integer_sequence<T, /* a sequence 0, 1, 2, ..., N-1 */ >;
// ```
//
// - std::make_index_sequence<n> : 类型T为size_t的make_integer_sequence
// ```
// template<std::size_t N>
// using make_index_sequence = std::make_integer_sequence<std::size_t, N>;
// ```
//
// - index_sequence_for: 用于parameter pack
// template<class... T>
// using index_sequence_for = std::make_index_sequence<sizeof...(T)>;

// `using` keyword: 模版别名
//  template <typename x>
//  using A = SomeTemplateClass<x>;  <-- call this by A<T1> ... blah blah ...

// `parameter pack`: 变长模版参数
// `sizeof...(Args)`: 变长模版参数的参数个数
//

// std::get for tuple
//


// PART II - 实现
// 之前已经实现过一次了

// 用途： 1. tuple自动展开  2. enum better printing

void ShowTopic() {
  log_info("Topic: " "index_sequence");
}

TEST(index_sequence, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
