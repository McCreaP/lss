#ifndef LSS_RAW_INPUT_READER_H
#define LSS_RAW_INPUT_READER_H

// This is basically the current RawData but a timestamp should be added.
class RawInput;

class RawInputReader {
 public:
  static constexpr kDefaultPath = ".";
  static constexpr kDefaultMinimumReadInterval = 1;

  RawInputReader();

  void SetPath(const string &path);
  void SetMinimumReadInterval(double interval);

  const RawInput &GetInput() const;

  bool Read();
};

#endif  // LSS_RAW_INPUT_READER_H
