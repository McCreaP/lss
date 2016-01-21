#include <glog/logging.h>

#include <io/basic_input.h>

using lss::io::BasicReader;
using lss::io::RawData;

void configLogger(char *argv[]) {
  FLAGS_log_dir = "logs";
  FLAGS_stderrthreshold = 0;
  google::InitGoogleLogging(argv[0]);
}

int main(int __attribute__((unused)) argc, char *argv[]) {
  configLogger(argv);
  LOG(INFO) << "Scheduler start";
  RawData rawData;
  BasicReader basicReader(".");
  basicReader.Read(&rawData);
  LOG(INFO) << "Scheduler stop";
  return 0;
}
