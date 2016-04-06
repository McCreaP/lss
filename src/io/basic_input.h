#ifndef LSS_IO_BASIC_INPUT_H_
#define LSS_IO_BASIC_INPUT_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "base/raw_situation.h"
#include "io/reader.h"

namespace lss {
namespace io {

class BasicReader: public Reader {
 public:
  // 'input_path' should name a file (not directory) with input data.
  explicit BasicReader(const std::string &input_path);

  void SetInputPath(const std::string &input_path) override;

  // The contents of the files are not validated and reading malformed
  // records will quietly result in corrupted data.
  bool Read(RawSituation* destination) override;

 private:
  std::string input_path_;
};

std::istream &operator>>(std::istream &input, ::lss::RawJob &job);
std::istream &operator>>(std::istream &input, ::lss::RawBatch &batch);
std::istream &operator>>(std::istream &input, ::lss::RawMachine &machine);
std::istream &operator>>(std::istream &input, ::lss::RawMachineSet &set);
std::istream &operator>>(std::istream &input, ::lss::RawFairSet &set);
std::istream &operator>>(std::istream &input, ::lss::RawAccount &account);
std::istream &operator>>(std::istream &input, ::lss::RawChangeCost &change);

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_BASIC_INPUT_H_
