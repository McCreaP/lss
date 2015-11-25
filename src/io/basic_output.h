#ifndef LSS_IO_BASIC_OUTPUT_H_
#define LSS_IO_BASIC_OUTPUT_H_

#include <string>

namespace lss {
namespace io {

class BasicWriter {
 public:
  explicit BasicWriter(const std::string& output_path);

  void SetOutputPath(const std::string& output_path);

  // Returns true on success. The assignment might fail if there is already
  // a pending assignment but that's not the only possibility.
  bool Assign(int machine_id, int job_id);

  // Attempts to cancel a pending assignment. Returns false upon failure
  // or if there is no pending assignment.
  bool Unassign(int machine_id);

 private:
  std::string output_path_;
};

}  // namespace io
}  // namespace lss


#endif  // LSS_IO_BASIC_OUTPUT_H_
