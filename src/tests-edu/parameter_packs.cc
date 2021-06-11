// keywords: parameter_packs
//

#include "log.h"
#include <gtest/gtest.h>
#include <iostream>

//
// tests for parameter packs
//

#include <iostream>
#include <string>
#include <tuple>
void doPrint(std::ostream &) {}
template <typename T, typename... Args>
void doPrint(std::ostream &out, T t, Args... args) {
  out << t << ", ";
  doPrint(out, args...);
}
template <int... args> class index {};

template <typename T, int... args> void tstuple(T &&t, index<args...> &&) {
  doPrint(std::cout, std::get<args>(t)...);
}

TEST(parameter_packs, printtuple) {
  auto t = std::make_tuple(1, "Foo", 3.14);
  tstuple(t, index<0, 1>{});
}

void ShowTopic() {
  log_info("Topic: "
           "parameter_packs");
}

TEST(parameter_packs, ShowTopic) { ShowTopic(); }

#include <cstdio>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace myexp {
// for c++11
template <int... Is> struct index {};

template <int N, int... Is> struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <int... Is> struct gen_seq<0, Is...> { typedef index<Is...> type; };

template <int st, typename t> struct s;

template <int st, int... is> struct s<st, index<is...>> {
  using type = index<is + st...>;
};

template <int st, int ed,
          typename t =
              typename std::enable_if<st<ed, void>::type> struct gen_seq_range {
  using type = typename s<st, typename gen_seq<ed - st + 1>::type>::type;
};

template <typename vs> constexpr auto test_template_deduction() {
  return __PRETTY_FUNCTION__;
}
} // namespace myexp

TEST(parameter_packs, eq_range) {
  std::cout << myexp::test_template_deduction<myexp::gen_seq<5>::type>()
            << std::endl;
  std::cout
      << myexp::test_template_deduction<myexp::gen_seq_range<3, 5>::type>()
      << std::endl;
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
