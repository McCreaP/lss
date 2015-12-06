#ifndef LSS_IO_READER_H_
#define LSS_IO_READER_H_

#include <string>

#include "io/raw_input_types.h"

namespace lss {
namespace io {

class Reader {
 public:
  virtual void SetInputPath(const std::string& input_path) = 0;
  virtual bool Read(RawData* destination) = 0;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_READER_H_
