#include "buffer.h"
#include <vector>


namespace cp1craft {
namespace io {

class Buffer::BufferImpl : public Buffer {
   

private:
  std::vector<Extent::uchar_t> rep_;
};

}  // namespace io
}  // namespace cp1craft

