#include "basic_input.h"

#include <functional>
#include <sstream>

namespace lss {
namespace io {
namespace {

const char kMachinesFile[] = "machines";
const char kMachineSetsFile[] = "machine-sets";
const char kFairMachineSetsFile[] = "fair-service-machine-sets";
const char kJobsFile[] = "jobs";
const char kBatchesFile[] = "batches";
const char kAccountsFile[] = "accounts";
const char kContextChangesFile[] = "context-changes";

}  // namespace

BasicReader::BasicReader(const std::string& input_path)
    : input_path_(input_path) { }

void BasicReader::SetInputPath(const std::string& input_path) {
  input_path_ = input_path;
}

bool BasicReader::Read(RawData& destination) {
  bool ok = true;
  ok &= ReadRecords(kMachinesFile, destination.machines);
  ok &= ReadRecords(kMachineSetsFile, destination.machine_sets);
  ok &= ReadRecords(kFairMachineSetsFile, destination.fair_machine_sets);
  ok &= ReadRecords(kJobsFile, destination.jobs);
  ok &= ReadRecords(kBatchesFile, destination.batches);
  ok &= ReadRecords(kAccountsFile, destination.accounts);
  ok &= ReadRecords(kContextChangesFile, destination.context_changes);
  return ok;
}

std::istream& operator>>(std::istream& input, Job& job) {
  input >> job.id >> job.batch_id >> job.duration >> job.machineset_id;
  for (int i = 0; i < kContextN; ++i)
    input >> job.context[i];
  return input;
}

std::istream& operator>>(std::istream& input, Batch& batch) {
  input >> batch.id >> batch.account_id;
  input >> batch.timely_reward >> batch.reward >> batch.expected_time;
  input >> batch.due;
  return input;
}

std::istream& operator>>(std::istream& input, Machine& machine) {
  int state;
  input >> machine.id >> state;
  machine.state = static_cast<MachineState>(state);
  return input;
}

std::istream& operator>>(std::istream& input, MachineSet& set) {
  input >> set.id;

  std::string line;
  std::getline(input, line);
  std::istringstream line_stream(line);
  int machine_id;
  while (line_stream >> machine_id) set.machines.push_back(machine_id);

  return input;
}

std::istream& operator>>(std::istream& input, Account& account) {
  input >> account.id >> account.alloc;
  return input;
}

std::istream& operator>>(std::istream& input, ContextChange& change) {
  for (int i = 0; i < kContextN; ++i)
    input >> change.changed[i];
  input >> change.cost;
  return input;
}

}  // namespace io
}  // namespace lss
