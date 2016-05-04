#ifndef LSS_IO_BASIC_OUTPUT_H_
#define LSS_IO_BASIC_OUTPUT_H_

#include <string>

#include "gmock/gmock.h"
#include "pstreams/pstream.h"

namespace lss {
namespace io {

class Writer {
 public:
  virtual bool Assign(int machine_id, int job_id) = 0;
  virtual bool Unassign(int machine_id) = 0;
  virtual ~Writer() = default;
};

class BasicWriter : public Writer {
 public:
  explicit BasicWriter(const std::string &output_path);

  void SetOutputPath(const std::string &output_path);

  // Returns true on success. The assignment might fail if there is already
  // a pending assignment but that's not the only possibility.
  bool Assign(int machine_id, int job_id) override;

  // Attempts to cancel a pending assignment. Returns false upon failure
  // or if there is no pending assignment.
  bool Unassign(int machine_id) override;

 private:
  std::string output_path_;
};

class WriterMock : public Writer {
 public:
  MOCK_METHOD2(Assign, bool(int, int));
  MOCK_METHOD1(Unassign, bool(int));
};

void NotifyDriverIFinishedCompute();

}  // namespace io
}  // namespace lss


#endif  // LSS_IO_BASIC_OUTPUT_H_
