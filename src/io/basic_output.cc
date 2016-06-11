#include "io/basic_output.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <stdio.h>

#include <iostream>

namespace lss {
namespace io {

BasicWriter::BasicWriter(const std::string &output_path)
  : output_path_(output_path + "/") { }

void BasicWriter::SetOutputPath(const std::string &output_path) {
  output_path_ = output_path;
}

bool BasicWriter::Assign(IdType machine_id, IdType job_id) {
  VLOG(2) << "An attempt to assign job: " << job_id
    << " to the machine: " << machine_id << " is being made" << std::endl;
  const std::string path = output_path_ + std::to_string(machine_id);
  const std::string tmp_path = path + "_tmp";
  const std::string content = std::to_string(job_id);

  // Use open with O_CREAT | O_EXCL to ensure the file is actually created.
  // This functions as a lock and makes Assign() safe for concurrent calls.
  // Creates file with READ and WRITE permissions for the user.
  int fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    PLOG(WARNING) << "Creating temporary file failed";
    return false;
  }

  // Poor man's way to check if file does not exist (i.e. there is not pending
  // assignment) - call to access() might fail for other reasons than
  // nonexistence of the file.
  bool ok = (access(path.c_str(), F_OK) == -1);
  PLOG_IF(WARNING, !ok) << "There is a pending assignment";

  if (ok) {
    ok &= (write(fd, content.c_str(), content.size()) != -1);
    PLOG_IF(WARNING, !ok) << "Write failed";
    ok &= (close(fd) != -1);
    PLOG_IF(WARNING, !ok) << "Close failed";
  }
  if (ok) {
    ok &= (rename(tmp_path.c_str(), path.c_str()) != -1);
    PLOG_IF(WARNING, !ok) << "Rename failed";
  }
  if (ok) {
    VLOG(2) << "Job: " << job_id << " assigned to machine: " << machine_id;
    return true;
  }

  remove(tmp_path.c_str());
  PLOG_IF(WARNING, remove(tmp_path.c_str())) << "Remove temporary file failed";
  return false;
}

bool BasicWriter::Unassign(IdType machine_id) {
  const std::string path = output_path_ + std::to_string(machine_id);
  int result;
  PLOG_IF(INFO, result = remove(path.c_str())) << "Unassign failed";
  if (result == 0) {
    VLOG(2) << "Job unassigned from machine: " << machine_id;
    return true;
  }
  return false;
}

bool BasicWriter::DoesAssignmentExist(IdType machine_id) {
  const std::string path = output_path_ + std::to_string(machine_id);
  int fd = open(path.c_str(), O_RDONLY, S_IRUSR);
  if (fd == -1) {
    return false;
  }
  PLOG_IF(WARNING, close(fd) == -1) << "Close failed";
  return true;
}

void NotifyDriverIFinishedCompute() {
  redi::ipstream proc("curl localhost:8000 > /dev/null", redi::pstreams::pstderr);
}

}  // namespace io
}  // namespace lss
