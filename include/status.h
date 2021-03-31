#pragma once

#include <string>
#include <sstream>
#include <fmt/core.h>


namespace cp1craft {
namespace utils {

// TODO - this class may cause performance impact
class status {
public:
  status(){}

  template<typename T>
  status& operator<< (T&& arg) {
    ss << fmt::format("{}", std::forward<T>(arg));
    return *this;
  }
  
  std::string value() {
    auto r = ss.str();
    ss.str("");
    return r;
  }
private:
  std::stringstream ss;
};

} // namespace utils
} // namespace cp1craft
