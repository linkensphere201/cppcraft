#pragma once
#include <vector>

namespace cp1craft {
namespace io {

class Buffer {
  public:
    void PutUint32(uint32_t u) = 0;
    void PutBytes(void *p, uint32_t n) = 0;
    void PutString(const std::string &s) = 0;
    void PutExtent(void *p, uint32_t n) = 0;

    void PeakUint32(uint32_t &x) = 0;
    void PeakBytes(uint32_t n, void*&p) = 0;
    void PeakString(std::string &s) = 0;

};


}
}
