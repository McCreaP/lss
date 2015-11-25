#include "io/basic_output.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

namespace lss {
namespace io {

BasicWriter::BasicWriter(const std::string& output_path)
  : output_path_(output_path) {}

void BasicWriter::SetOutputPath(const std::string& output_path) {
  output_path_ = output_path;
}

bool BasicWriter::Assign(int machine_id, int job_id) {
  const std::string path = output_path_ + std::to_string(machine_id);
  const std::string tmp_path = path + "_tmp";
  const std::string content = std::to_string(job_id);

  // Use open with O_CREAT | O_EXCL to ensure the file is actually created.
  // This functions as a lock and makes Assign() safe for concurrent calls.
  int fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_EXCL);
  if (fd == -1) return false;

  // Poor man's way to check if file does not exist (i.e. there is not pending
  // assignment) - call to access() might fail for other reasons than
  // nonexistence of the file.
  bool ok = (access(path.c_str(), F_OK) == -1);

  if (ok) {
    ok &= (write(fd, content.c_str(), content.size()) != -1);
    ok &= (close(fd) != -1);
  }
  if (ok)
    ok &= (rename(tmp_path.c_str(), path.c_str()) != -1);
  if (ok)
    return true;

  remove(tmp_path.c_str());
  return false;
}

bool BasicWriter::Unassign(int machine_id) {
  const std::string path = output_path_ + std::to_string(machine_id);
  return remove(path.c_str()) != -1;
}

}  // namespace io
}  // namespace lss
