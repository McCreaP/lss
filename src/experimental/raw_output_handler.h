#ifndef LSS_EXPERIMENTAL_RAW_OUTPUT_HANDLER_H_
#define LSS_EXPERIMENTAL_RAW_OUTPUT_HANDLER_H_

#include "raw_output.h"

class RawOutputHandler {
 public:
  static constexpr kDefaultPath = ".";
  static constexpr kDefaultMinimumReadInterval = 1;

  RawOutputHandler();

  void SetPath(const std::string &path);
  void SetMinimumReadInterval(double interval);

  const RawOutput &GetOutput() const;

  bool Read();
  bool Write(const RawOutput &output);
};

#endif // LSS_EXPERIMENTAL_RAW_OUTPUT_HANDLER_H_
