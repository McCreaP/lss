#ifndef LSS_IO_BASIC_INPUT_H_
#define LSS_IO_BASIC_INPUT_H_

#include <fstream>
#include <string>
#include <vector>

#include "io/raw_input_types.h"

namespace lss {
namespace io {

struct RawData {
  std::vector<Machine> machines;
  std::vector<MachineSet> machine_sets;
  std::vector<MachineSet> fair_machine_sets;
  std::vector<Job> jobs;
  std::vector<Batch> batches;
  std::vector<Account> accounts;
  std::vector<ContextChange> context_changes;
};

class BasicReader {
 public:
  // 'input_path' should name a file (not directory) with input data.
  explicit BasicReader(const std::string& input_path);

  void SetInputPath(const std::string& input_path);

  // The contents of the files are not validated and reading malformed
  // records will quietly result in corrupted data.
  bool Read(RawData* destination);

  std::string input_path_;
};

std::istream& operator>>(std::istream& input, Job& job);
std::istream& operator>>(std::istream& input, Batch& batch);
std::istream& operator>>(std::istream& input, Machine& machine);
std::istream& operator>>(std::istream& input, MachineSet& set);
std::istream& operator>>(std::istream& input, Account& account);
std::istream& operator>>(std::istream& input, ContextChange& change);

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_BASIC_INPUT_H_
