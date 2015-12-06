#ifndef LSS_IO_INPUT_H_
#define LSS_IO_INPUT_H_

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <vector>
#include <set>

#include "io/reader.h"
#include "oo_input_types/batch.h"
#include "oo_input_types/machine.h"
#include "oo_input_types/machine_set.h"

namespace lss {

class Input {
 public:
  explicit Input(std::shared_ptr<io::Reader> reader);

  bool Update();
  std::vector<Batch> GetSortedBatches() const;
  MachineSet GetMachineSet(int id) const;

 private:
  void UpdateBatches(const std::vector<io::Batch>& raw_batches,
                     const std::vector<io::Job>& raw_jobs);
  void UpdateMachines(const std::vector<io::Machine>& raw_machines,
      const std::vector<io::ContextChange>& raw_context_changes);
  void UpdateMachineSets(const std::vector<io::MachineSet>& raw_machine_sets);

  std::shared_ptr<io::Reader> reader_;
  std::map<int, Batch> batches_;
  std::unordered_map<int, Machine> machines_;
  std::unordered_map<int, MachineSet> machine_sets_;
};

}  // namespace lss

#endif  // LSS_IO_INPUT_H_
