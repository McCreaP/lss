#include "io/basic_input.h"

#include <glog/logging.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>

namespace lss {
namespace io {
namespace {

template<class T, std::vector<T> RawData::* VEC>
void ReadOne(std::istream* input, RawData* destination) {
  (destination->*VEC).push_back(T());
  *input >> (destination->*VEC).back();
}

using SectionReader = void(*)(std::istream*, RawData*);
using HeaderReaderPair = std::tuple<const char*, SectionReader>;

constexpr std::array<HeaderReaderPair, 7> kReaders{
  HeaderReaderPair{
    "machines",
    &ReadOne<Machine, &RawData::machines>
  },
  HeaderReaderPair{
    "machine-sets",
    &ReadOne<MachineSet, &RawData::machine_sets>
  },
  HeaderReaderPair{
    "fair-service-machine-sets",
    &ReadOne<MachineSet, &RawData::fair_machine_sets>
  },
  HeaderReaderPair{
    "jobs",
    &ReadOne<Job, &RawData::jobs>
  },
  HeaderReaderPair{
    "batches",
    &ReadOne<Batch, &RawData::batches>
  },
  HeaderReaderPair{
    "accounts",
    &ReadOne<Account, &RawData::accounts>
  },
  HeaderReaderPair{
    "context-changes",
    &ReadOne<ContextChange, &RawData::context_changes>
  },
};

SectionReader GetSectionReader(const std::string& header) {
  for (size_t i = 0; i < kReaders.size(); ++i) {
    if (header == std::get<0>(kReaders[i]))
      return std::get<1>(kReaders[i]);
  }
  return nullptr;
}

}  // namespace

BasicReader::BasicReader(const std::string& input_path)
    : input_path_(input_path) { }

void BasicReader::SetInputPath(const std::string& input_path) {
  input_path_ = input_path;
}

bool BasicReader::Read(RawData* destination) {
  std::string new_path = input_path_ + ".read";
  if (std::rename(input_path_.c_str(), new_path.c_str()))
    return false;

  std::ifstream input(new_path);
  if (input.fail())
    return false;

  SectionReader reader = nullptr;
  std::string line_buf;
  std::istringstream line;
  while (getline(input, line_buf)) {
    if (line_buf.empty())
      continue;

    if (SectionReader new_reader = GetSectionReader(line_buf)) {
      reader = new_reader;
    } else if (reader) {
      line.str(line_buf);
      reader(&line, destination);
    } else {
      LOG(WARNING) << "Expected header in input line: '" << line_buf;
    }
  }

  input.close();
  if (std::remove(new_path.c_str())) {
    LOG(WARNING) << "Failed to remove input file: " << strerror(errno);
  }

  return true;
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
