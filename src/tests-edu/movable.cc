#include <exception>
#include <iostream>
#include <type_traits>
// https://stackoverflow.com/questions/38287425/moved-objects-are-still-destructed

class Moveable {
public:
  Moveable() {
    std::cout << "Acquire odd resource\n";
  }

  ~Moveable() noexcept(false) {
    std::cout << "Release odd resource\n";
    // if (!std::uncaught_exception() && error_during_release) {
    //   throw std::exception("error");
    // }
  }

  Moveable(Moveable const &) = delete;
  Moveable &operator=(Moveable const &) = delete;

  Moveable(Moveable &&) = default;
  Moveable &operator=(Moveable &&) = default;
};

int main(int argc, char *argv[]) {
  static_assert(!std::is_copy_constructible<Moveable>::value,
    "is not copy constructible");
  static_assert(!std::is_copy_assignable<Moveable>::value, "is not copy assignable");
  static_assert(std::is_move_constructible<Moveable>::value, "is move constructible");
  static_assert(std::is_move_assignable<Moveable>::value, "is move assignable");

  Moveable moveable{};
  Moveable moved{std::move(moveable)};
  Moveable moved_again{std::move(moved)};
}
