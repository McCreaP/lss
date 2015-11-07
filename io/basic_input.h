#ifndef LSS_IO_BASIC_INPUT_H_
#define LSS_IO_BASIC_INPUT_H_

#include <fstream>
#include <string>
#include <vector>

#include "raw_input_types.h"

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
  BasicReader(const std::string& input_path);

  void SetInputPath(const std::string& input_path);

  void Read(RawData& destination);

 private:
  template<class T>
  void ReadRecords(const std::string& file_name, std::vector<T>& destination) {
    std::ifstream input(input_path_ + '/' + file_name);

    T buf;
    while (input >> buf) destination.push_back(buf);
  }

  std::string input_path_;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_BASIC_INPUT_H_
