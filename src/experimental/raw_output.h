// It would probably be best to place all raw types in a single 'raw_types.h' header.
#ifndef LSS_EXPERIMENTAL_RAW_OUTPUT_H_
#define LSS_EXPERIMENTAL_RAW_OUTPUT_H_

#include "experimental/base.h"

#include <vector>

struct RawAssignment {
  IdType machine_id;
  IdType job_id;
};

struct RawOutput {
  Time timestamp;
  std::vector<RawAssignment> assignments;
};

#endif  // LSS_EXPERIMENTAL_RAW_OUTPUT_H_
