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
  using UChar = unsigned char;
  // without a deleter
  Extent(UChar *p, uint32_t l) : ptr_(p), len_(l), del_(nullptr) {}

  // with a deleter whose type should be able to convert to `Extent::deleter`
  template <class Del>
  Extent(UChar *p, uint32_t l, Del d) : ptr_(p), len_(l), del_(d) {}

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
  using deleter = std::function<void(UChar *)>;
  UChar *ptr_;
  uint32_t len_;
  deleter del_;
};

// Buffer: a place to contain a frame of message.
// in FIFO fashion the operation of a Buffer is to `put` and `peek`:
//   - `put` is to write something in at the end of this "buffer"
//   - `peek` is to read something out from the front of this "buffer"
//
//   1) ops: put a, put b, put c, finish put
//   2) content should be like: | a | b | c |
//   3) 1st peek --> a (caller must know which type of data is needed.)
//      2ed peek --> b
//      3rd peek --> c
//      4th peek --> failed_because_peek_is_finished
//
class StreamOp;
class Buffer {
 public:
  using UChar = Extent::UChar;
  
  virtual void PutUint32(uint32_t u) = 0;
  virtual void PutString(const std::string &s) = 0;
  virtual void PutBytes(UChar *p, uint32_t n) = 0;

  virtual bool PeekUint32(uint32_t &x) = 0;
  virtual bool PeekString(uint32_t n, std::string &s) = 0;
  virtual bool PeekBytes(uint32_t n, UChar *p) = 0;

  static std::shared_ptr<Buffer> NewBuffer(cp1craft::utils::LoggerPtr logger = cp1craft::utils::g_console);
  
  static constexpr const int kPreAllocSize = 64;

 private:
  friend class  StreamOp;
  const UChar*  oBuf();
  uint32_t      oLimit();

  bool eof_put_;
};

class StreamOp {
public:
  virtual void Bind(Buffer *) = 0;
  virtual bool Out(const Buffer::UChar*, uint32_t) = 0;
  virtual bool In(Buffer::UChar*, uint32_t) = 0;
};

}  // namespace io
}  // namespace cp1craft
