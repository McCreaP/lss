#ifndef LSS_IO_READER_H_
#define LSS_IO_READER_H_

#include <string>

#include "base/raw_situation.h"

namespace lss {
namespace io {

class Reader {
 public:
  virtual void SetInputPath(const std::string& input_path) = 0;
  virtual bool Read(RawSituation* destination) = 0;

  virtual ~Reader() = default;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_READER_H_
