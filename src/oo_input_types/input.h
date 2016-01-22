#ifndef LSS_OO_INPUT_TYPES_INPUT_H_
#define LSS_OO_INPUT_TYPES_INPUT_H_

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include <unordered_set>

#include "io/reader.h"
#include "oo_input_types/batch.h"
#include "oo_input_types/machine.h"

namespace lss {

class Input {
 public:
  explicit Input(std::shared_ptr<io::Reader> reader);

  // Returns false if reading data from file failed.
  // If the data is erroneous behavior is undefined.
  bool Update();
  void Assign(const io::Job& raw_job, Machine* machine);
  bool IsJobAssigned(int job_id) const;
  std::vector<Batch> GetSortedBatches() const;
  // Returns const reference to the member variable.
  // This reference is valid as long as object exists
  // and non-const method hasn't been called.
  const std::vector<std::shared_ptr<Machine>>& GetMachinesFromSet(int set_id) const;

 private:
  void UpdateBatches(const std::vector<io::Batch>& raw_batches);
  void UpdateJobs(const std::vector<io::Job>& raw_jobs);
  void UpdateMachines(const std::vector<io::Machine>& raw_machines);
  void UpdateMachineSets(const std::vector<io::MachineSet>& raw_machine_sets);

  std::shared_ptr<io::Reader> reader_;
  std::map<int, Batch> batches_;
  std::unordered_set<int> assigned_jobs_ids_;
  std::unordered_map<int, std::shared_ptr<Machine>> machines_;
  std::unordered_map<int, std::vector<std::shared_ptr<Machine>>> machines_from_set_;
  std::shared_ptr<ContextChanges> context_changes_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_INPUT_H_
