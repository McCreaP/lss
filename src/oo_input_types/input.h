#ifndef LSS_OO_INPUT_TYPES_INPUT_H_
#define LSS_OO_INPUT_TYPES_INPUT_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "io/reader.h"
#include "oo_input_types/batch_wrapper.h"
#include "oo_input_types/machine_wrapper.h"

namespace lss {

class Input {
 public:
  explicit Input(std::unique_ptr<io::Reader> reader);

  // Returns false if reading data from file failed.
  // If the data is erroneous behavior is undefined.
  bool Update();
  void Assign(const io::RawJob& raw_job, MachineWrapper* machine);
  bool IsJobAssigned(int job_id) const;
  std::vector<BatchWrapper> GetBatches() const;
  // The returned reference is valid till a non-const method of Input is called
  const std::vector<std::shared_ptr<MachineWrapper>>& GetMachinesFromSet(int set_id) const;

 private:
  void UpdateBatches(const std::vector<io::RawBatch>& raw_batches);
  void UpdateJobs(const std::vector<io::RawJob>& raw_jobs);
  void UpdateMachines(const std::vector<io::RawMachine>& raw_machines);
  void UpdateMachineSets(const std::vector<io::RawMachineSet>& raw_machine_sets);

  std::unique_ptr<io::Reader> reader_;
  std::map<int, BatchWrapper> batches_;
  std::unordered_set<int> assigned_jobs_ids_;
  std::unordered_map<int, std::shared_ptr<MachineWrapper>> machines_;
  std::unordered_map<int, std::vector<std::shared_ptr<MachineWrapper>>> machines_from_set_;
  std::shared_ptr<ContextChanges> context_changes_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_INPUT_H_
