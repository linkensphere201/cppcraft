#include "buffer.h"
#include "log.h"

#include <string.h>
#include <vector>

namespace cp1craft {
namespace io {

// NOTE -
//  Buffer Implementation:
//    - not thread-safe, external synchronization is needed
//
class BufferImpl : public Buffer {
  using logger = cp1craft::utils::LoggerPtr;
  const size_t kAlignment = 32;

public:
  BufferImpl(logger log = nullptr);
  virtual void PutUint32(uint32_t u) override;
  virtual void PutString(const std::string &s) override;
  virtual void PutBytes(UChar *p, uint32_t n) override;

  virtual bool PeekUint32(uint32_t &x) override;
  virtual bool PeekString(uint32_t n, std::string &s) override;
  virtual bool PeekBytes(uint32_t n, UChar *p) override;

private:
  void acquire_put_nbytes(int nbytes); // in bytes
  void append(const UChar *d, int nbytes);
  UChar *head();

  bool acquire_peek_nbytes(uint32_t nbytes);
  void peek(uint32_t nbytes) { has_peek_ += nbytes; }
  void internal_info();

  std::vector<UChar> rep_;
  size_t has_put_;
  size_t has_peek_;

  logger logger_;
};

void BufferImpl::internal_info() {
  if (logger_) {
    logger_->info("{} {}", has_put_, has_peek_);
  }
}

BufferImpl::BufferImpl(logger log) : rep_(Buffer::kPreAllocSize), has_put_(0), has_peek_(0), logger_(log) {
}

void BufferImpl::acquire_put_nbytes(int nbytes) {
  if (rep_.size() + nbytes > rep_.capacity()) {
    size_t tgtsz = rep_.size() + nbytes;
    tgtsz &= (~(kAlignment - 1));
    tgtsz += kAlignment;
    assert(tgtsz > rep_.capacity());
    rep_.reserve(tgtsz);
  }
}
void BufferImpl::append(const UChar *d, int nbytes) {
  acquire_put_nbytes(nbytes);
  assert(rep_.capacity() - has_put_ >= nbytes);
  memcpy(head() + has_put_, d, nbytes);
  has_put_ += nbytes;
}

Buffer::UChar *BufferImpl::head() { return &rep_[0]; }

bool BufferImpl::acquire_peek_nbytes(uint32_t n) {
  size_t sz = has_put_;
  sz = sz > has_peek_ ? (sz - has_peek_) : 0;
  return sz >= n;
}

#include <arpa/inet.h> // for linux
void BufferImpl::PutUint32(uint32_t u) {
  uint32_t nw = htonl(u);
  // if (logger_) { logger_->info("put: {}", u); }
  // internal_info();
  append(reinterpret_cast<UChar *>(&nw), sizeof(uint32_t));
}

void BufferImpl::PutString(const std::string &s) {
  append(reinterpret_cast<const UChar *>(s.data()), s.length());
}

void BufferImpl::PutBytes(UChar *p, uint32_t n) { append(p, n); }

bool BufferImpl::PeekUint32(uint32_t &x) {
  if (acquire_peek_nbytes(sizeof(uint32_t))) {
    UChar *topeek = head() + has_peek_;
    uint32_t n;
    memcpy(reinterpret_cast<UChar *>(&n), topeek, sizeof(uint32_t));
    x = ntohl(n);
    peek(sizeof(uint32_t));
    // if (logger_) { logger_->info("peek: {}", x); }
    // internal_info();
    return true;
  }
  return false;
}
bool BufferImpl::PeekString(uint32_t n, std::string &s) {
  if (acquire_peek_nbytes(n)) {
    UChar *topeek = head() + has_peek_;
    s = std::string(reinterpret_cast<const char*>(topeek), n);
    peek(n);
    return true;
  }
  return false;
}
bool BufferImpl::PeekBytes(uint32_t n, UChar *p) {
  if (acquire_peek_nbytes(n)) {
    UChar *topeek = head() + has_peek_;
    memcpy(p, topeek, n);
    peek(n);
    return true;
  }
  return false;
}

std::shared_ptr<Buffer> Buffer::NewBuffer(cp1craft::utils::LoggerPtr logger) {
  return std::make_shared<BufferImpl>(logger);
}

} // namespace io
} // namespace cp1craft

