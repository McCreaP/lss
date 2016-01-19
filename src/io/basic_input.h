#ifndef LSS_IO_BASIC_INPUT_H_
#define LSS_IO_BASIC_INPUT_H_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "io/raw_input_types.h"
#include "io/reader.h"

namespace lss {
namespace io {

// Represents a simple cooperative file lock.
class FileLock {
 public:
  // Constructs a FileLock that will be represented by file 'lock_name'.
  // You have to explicitly call TryLock() to acquire it.
  explicit FileLock(const std::string& lock_name);

  // Calls Unlock().
  ~FileLock();

  bool IsLocked() const;

  // Does nothing and returns false if lock is already acquired.
  // Otherwise attempts to create lock file and returns true on success.
  bool TryLock();

  // Attempts to remove lock file and returns FileLock to fresh state.
  // Does nothing if FileLock was not acquired.
  void Unlock();

 private:
  int file_descriptor_;
  const std::string lock_name_;
};

class BasicReader: public Reader {
 public:
  explicit BasicReader(const std::string& input_path);

  void SetInputPath(const std::string& input_path) override;

  // Returns true iff file 'lock' is successfully created in 'input_dir'
  // and subsequently all input files are opened. File 'lock' is deleted
  // before Read() returns.
  //
  // The contents of the files are not validated and reading malformed
  // records will quietly result in corrupted data.
  bool Read(RawData* destination) override;

 private:
  // Returns true iff opening the file succeeds.
  template<class T>
  bool ReadRecords(const std::string& file_name, std::vector<T>* destination) {
    std::ifstream input(input_path_ + '/' + file_name);
    if (input.fail()) {
      std::cerr << "Opening file: '" << file_name << "' failed in ReadRecords." << std::endl;
      return false;
    }


    T buf;
    while (input >> buf) destination->push_back(buf);
    return true;
  }

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
