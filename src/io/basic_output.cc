#include "io/basic_output.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

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
  // This makes Assign() safe for concurrent calls.
  int fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_EXCL);
  if (fd == -1) return false;

  bool ok = true;
  ok &= (write(fd, content.c_str(), content.size()) != -1);
  ok &= (close(fd) != -1);
  if (ok) {
    int ret = renameat2(AT_FDCWD, tmp_path.c_str(), AT_FDCWD, path.c_str(),
                        RENAME_NOREPLACE);
    ok &= (ret != -1);
  }

  auto ptr = &renameat2;
  ptr = nullptr;

  if (ok) {
    return true;
  } else {
    remove(tmp_path.c_str());
    return false;
  }
}

bool BasicWriter::Unassign(int machine_id) {
  const std::string path = output_path_ + std::to_string(machine_id);
  return remove(path.c_str()) != -1;
}

}  // namespace io
}  // namespace lss
