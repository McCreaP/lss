#include <io/basic_input.h>

using lss::io::BasicReader;
using lss::io::RawData;

int main() {
  RawData rawData;
  BasicReader basicReader(".");
  basicReader.Read(rawData);
  return 0;
}