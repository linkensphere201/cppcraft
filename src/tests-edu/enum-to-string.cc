// keywords: enum-to-string
//

#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>
#include "log.h"


enum class MyType : int8_t {
    kUnknown = 1,
    kType1,
    kType2,
    kTypeX
};
template <typename ... Args>
std::string enumstringwrapper() {
    return __PRETTY_FUNCTION__;
}

#include <utility>

template <auto... vs>
constexpr auto get_enumerator_names_impl() {
  return __PRETTY_FUNCTION__;
}

template <typename Enum, auto...vs>
constexpr auto get_enumerator_names(std::index_sequence<vs...>) {
    return get_enumerator_names_impl<static_cast<Enum>(vs)...>();
}

TEST(enumtostring, better) {
    log_info("{}", get_enumerator_names<MyType>(std::make_index_sequence<int(MyType::kTypeX)>{}));
}

#include "magic_enum.hpp"
template <typename x>
std::string wrapper_enums (x&& s) {
    return std::string(magic_enum::enum_name(s));
}

TEST(enumtostring, testmagic_enum) {
    MyType s = MyType::kTypeX;
    log_info("{}", wrapper_enums(s));
}

void ShowTopic() {
  log_info("Topic: " "enum-to-string");
}

TEST(enumtostring, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
