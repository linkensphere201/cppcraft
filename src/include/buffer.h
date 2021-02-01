#pragma once
#include <vector>

namespace cp1craft {
namespace io {


// an Extent is a wrapper of a piece of memory.(which maybe is 
//  part of e.g. rocksdb::MemFile)
//
class Extent {
  public:
    using uchar_t = unsigned char;
    // without a deleter
    Extent(uchar_t *p, uint32_t l) : ptr_(p), len_(l), del_(nullptr) {}

    // with a deleter whose type should be able to convert to `Extent::deleter`
    template<class Del>
    Extent(uchar_t *p, uint32_t l, Del d) : ptr_(p), len_(l), del_(d) {}

    ~Extent() {
      if (del_ != nullptr) {
        del_(ptr_);
      }
    }

    Extent(Extent && rhs) : ptr_(rhs.ptr_), len_(rhs.len_), del_(rhs.del_) {
      rhs.ptr_ = nullptr;
      rhs.len_ = 0;
    }

    Extent(const Extent &)            = delete;
    Extent& operator=(const Extent &) = delete;

  private:
    using deleter = std::function<void(uchar_t*)>;
    uchar_t *ptr_;
    uint32_t len_;
    deleter  del_;
};

class Buffer {
  public:
    virtual void PutUint32(uint32_t u) = 0;
    virtual void PutBytes(Extent::uchar_t *p, uint32_t n) = 0;
    virtual void PutString(const std::string &s) = 0;
    virtual void PutExtent(Extent &&ext) = 0;

    virtual void PeakUint32(uint32_t &x) = 0;
    virtual void PeakBytes(uint32_t n, void*&p) = 0;
    virtual void PeakString(std::string &s) = 0;
};


}
}
