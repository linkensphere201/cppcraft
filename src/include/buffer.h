#pragma once
#include "log.h"

namespace cp1craft {
namespace io {
//
// an Extent is a wrapper of a piece of memory.(which maybe is
//  part of e.g. rocksdb::MemFile)
//
class Extent {
 public:
  using uchar_t = unsigned char;
  // without a deleter
  Extent(uchar_t *p, uint32_t l) : ptr_(p), len_(l), del_(nullptr) {}

  // with a deleter whose type should be able to convert to `Extent::deleter`
  template <class Del>
  Extent(uchar_t *p, uint32_t l, Del d) : ptr_(p), len_(l), del_(d) {}

  ~Extent() {
    if (ptr_ != nullptr && del_ != nullptr) {
      del_(ptr_);
    }
  }

  Extent(Extent &&rhs) : ptr_(rhs.ptr_), len_(rhs.len_), del_(rhs.del_) {
    rhs.ptr_ = nullptr;
    rhs.len_ = 0;
  }

  Extent(const Extent &) = delete;
  Extent &operator=(const Extent &) = delete;

 private:
  using deleter = std::function<void(uchar_t *)>;
  uchar_t *ptr_;
  uint32_t len_;
  deleter del_;
};

// Buffer: a place to contain a frame of message.
// a little similar to a "queue", the operation of a buffer is `put` and `peak`:
//   - `put` is to write something in at the end of this "buffer"
//   - `peak` is to read something out from the front of this "buffer"
//
//   1) ops: put a, put b, put c, finish put
//   2) content should be like: | a | b | c |
//   3) 1st peak --> a (caller must know which type of data is needed.)
//      2ed peak --> b
//      3rd peak --> c
//      4th peak --> failed_because_peak_is_finished
//
class Buffer {
 public:
  using uchar_t = Extent::uchar_t;
  virtual void PutUint32(uint32_t u);
  virtual void PutString(const std::string &s);
  virtual void PutBytes(uchar_t *p, uint32_t n);

  void FinishPut() { eof_put_ = true; }
  bool IsPutFinished() { return eof_put_; }

  virtual void PeekUint32(uint32_t &x);
  virtual void PeekBytes(uint32_t n, void *&p);
  virtual void PeekString(std::string &s);
  virtual bool IsPeakFinished();

  static std::shared_ptr<Buffer> NewBuffer();
  static std::shared_ptr<Buffer> NewBuffer(uint32_t sz, cp1craft::utils::LoggerPtr logger);

 private:
  bool eof_put_;
};

}  // namespace io
}  // namespace cp1craft
