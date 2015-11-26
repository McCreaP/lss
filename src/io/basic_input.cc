#include "io/basic_input.h"

#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include <functional>
#include <iostream>
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

FileLock::FileLock(const std::string& lock_name)
  : file_descriptor_(-1), lock_name_(lock_name) {}

FileLock::~FileLock() {
  Unlock();
}

bool FileLock::IsLocked() const {
  return file_descriptor_ != -1;
}

bool FileLock::TryLock() {
  if (file_descriptor_ == -1) {
    file_descriptor_ = open(lock_name_.c_str(), O_WRONLY | O_CREAT | O_EXCL);
    return file_descriptor_ != -1;
  }
  return false;
}

void FileLock::Unlock() {
  if (file_descriptor_ != -1) {
    if (close(file_descriptor_) == -1)
      std::cerr << "Closing lock file failed: " << strerror(errno) << '\n';
    if (remove(lock_name_.c_str()) == -1)
      std::cerr << "Removing lock file failed: " << strerror(errno) << '\n';
    file_descriptor_ = -1;
  }
}

BasicReader::BasicReader(const std::string& input_path)
    : input_path_(input_path) { }

void BasicReader::SetInputPath(const std::string& input_path) {
  input_path_ = input_path;
}

bool BasicReader::Read(RawData* destination) {
  FileLock lock(input_path_ + "/lock");
  if (!lock.TryLock())
    return false;

  bool ok = true;
  ok &= ReadRecords(kMachinesFile, &destination->machines);
  ok &= ReadRecords(kMachineSetsFile, &destination->machine_sets);
  ok &= ReadRecords(kFairMachineSetsFile, &destination->fair_machine_sets);
  ok &= ReadRecords(kJobsFile, &destination->jobs);
  ok &= ReadRecords(kBatchesFile, &destination->batches);
  ok &= ReadRecords(kAccountsFile, &destination->accounts);
  ok &= ReadRecords(kContextChangesFile, &destination->context_changes);
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
