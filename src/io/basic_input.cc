#include "io/basic_input.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>

namespace lss {
namespace io {
namespace {

template<class T, std::vector<T> RawSituation::* VEC>
void ReadOne(std::istream* input, RawSituation* destination) {
  (destination->*VEC).push_back(T());
  *input >> (destination->*VEC).back();
}

using SectionReader = void (*)(std::istream*, RawSituation*);
using HeaderReaderPair = std::tuple<const char*, SectionReader>;

constexpr std::array<HeaderReaderPair, 7> kReaders{
  HeaderReaderPair{
    "machines",
    &ReadOne<RawMachine, &RawSituation::machines_>
  },
  HeaderReaderPair{
    "machine-sets",
    &ReadOne<RawMachineSet, &RawSituation::machine_sets_>
  },
  HeaderReaderPair{
    "fair-service-machine-sets",
    &ReadOne<RawMachineSet, &RawSituation::fair_sets_>
  },
  HeaderReaderPair{
    "jobs",
    &ReadOne<RawJob, &RawSituation::jobs_>
  },
  HeaderReaderPair{
    "batches",
    &ReadOne<RawBatch, &RawSituation::batches_>
  },
  HeaderReaderPair{
    "accounts",
    &ReadOne<RawAccount, &RawSituation::accounts_>
  },
  HeaderReaderPair{
    "context-changes",
    &ReadOne<RawChangeCost, &RawSituation::change_costs_>
  },
};

SectionReader GetSectionReader(const std::string &header) {
  for (size_t i = 0; i < kReaders.size(); ++i) {
    if (header == std::get<0>(kReaders[i]))
      return std::get<1>(kReaders[i]);
  }
  return nullptr;
}

}  // namespace

BasicReader::BasicReader(const std::string &input_path)
  : input_path_(input_path) { }

void BasicReader::SetInputPath(const std::string &input_path) {
  input_path_ = input_path;
}

bool BasicReader::Read(RawSituation* destination) {
  std::string new_path = input_path_ + ".read";
  if (std::rename(input_path_.c_str(), new_path.c_str())) {
    return false;
  }

  std::ifstream input(new_path.c_str());
  if (input.fail()) {
    return false;
  }

  SectionReader reader = nullptr;
  std::string line;
  while (getline(input, line)) {
    if (line.empty())
      continue;

    if (SectionReader new_reader = GetSectionReader(line)) {
      reader = new_reader;
    } else if (reader) {
      std::istringstream line_stream(line);
      reader(&line_stream, destination);
    } else {
      LOG(WARNING) << "Expected header in input line: '" << line;
    }
  }

  input.close();
  if (std::remove(new_path.c_str())) {
    PLOG(WARNING) << "Failed to remove input file";
  }

  return true;
}

}  // namespace io
}  // namespace lss
