// keywords: move
//

#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "move");
}

TEST(move, ShowTopic) {
  ShowTopic();
}
// - Is *move* really move something ?
//  https://stackoverflow.com/questions/39029145/how-does-a-move-constructor-work
// - what-the-fuck is move sementics ?
//  https://stackoverflow.com/questions/3106110/what-is-move-semantics
//
// - https://stackoverflow.com/questions/17642357/const-reference-vs-move-semantics

// https://stackoverflow.com/questions/38287425/moved-objects-are-still-destructed

// 
// for STL Containers, when to call move constructor?
//  
// for example, std::vector::emplace_back:
//   Inserts a new element at the end of the vector, right after its current last element. This new element is [constructed in place] using args as the arguments for its constructor.
//    

class App {
public:
  App(int l) :lx_(l) {
  }
  App(const App& x) {
    log_info("copy ctor");
  }
  App(App && rhs) {
    log_info("move ctor");
  }
private:
  int lx_;
};

//
// https://stackoverflow.com/questions/13125632/when-does-move-constructor-get-called
//
// https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html#C_002b_002b-Dialect-Options
// -fno-elide-constructors
// The C++ standard allows an implementation to omit creating a temporary that is only used to initialize another object of the same type. Specifying this option disables that optimization, and forces G++ to call the copy constructor in all cases. This option also causes G++ to call trivial member functions which otherwise would be expanded inline.

// In C++17, the compiler is required to omit these temporaries, but this option still affects trivial member functions.
TEST(container, ifmove) {
  std::vector<App> vec1, vec2;
  vec1.reserve(10);
  vec2.reserve(10);

  log_info("--emplace_back--");
  vec1.emplace_back(20);    // no move-ctor or copy-ctor is called, elem is construct in-place.
  log_info("--push_back--");
  vec2.push_back(App(21));  // move-ctor is called
}

// how does **emplace** is implemented:
//  keyword: `placement new`(much like make_shared)

#include <type_traits>

/*
 * https://stackoverflow.com/questions/36296425/how-to-determine-programmatically-if-an-expression-is-rvalue-or-lvalue-in-c/36296823
 * */
template <typename T>
constexpr bool is_lvalue(T&&) {
  return std::is_lvalue_reference<T>{};
}

TEST(rvalue, basic) {
  using Widget = int;
  Widget someWidget;

  Widget&& var1 = Widget();
  auto&& var2 = var1;
  
  log_info("{}, {}, {}", std::is_rvalue_reference<decltype(var1)>::value, is_lvalue(var1), (void*)&var1);
  
  log_info("before: {}", var1);
  var1 = 20;
  log_info("after : {}", var1);
}


#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <string>
#include <memory>
#include <cstdlib>
#include <cxxabi.h>

/*
 * from: https://stackoverflow.com/questions/18369128/how-can-i-see-the-type-deduced-for-a-template-type-parameter
 * */
template <typename T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
        (
            abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
            std::free
        );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

template <typename T>
void print_type(T&& x) {
  log_info("type: {}", type_name<decltype(x)>());
}

TEST(rvalue, universal_reference) {
  int x;
  log_info("int x;");
  print_type(x);
  
  const int & p = x;
  log_info("const int & x;");
  print_type(p);
  
  int && y = 1;
  log_info("int &&x = 1;");
  print_type(y);

  log_info("1;");
  print_type(1);

  log_info("int x; std::move(x);");
  print_type(std::move(x));
}

void func1(int x) {
  log_info("f1(int x)");
}
void func1(int& x) {
  log_info("f1(int& x)");
}

// only accept one parameter binder
template<class A>
void mybinder1st(A&& a) {
  func1(a);                   // bad
  func1(static_cast<A&&>(a)); // good
}



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
