#include <glog/logging.h>

#include <io/basic_input.h>

using lss::io::BasicReader;
using lss::io::RawData;

void configLogger(char *argv[]) {
  FLAGS_log_dir = "logs";
  FLAGS_stderrthreshold = 0;
  google::InitGoogleLogging(argv[0]);
}

int main(int argc, char *argv[]) {
  (void) argc;
  configLogger(argv);
  LOG(INFO) << "Aplication start";
  RawData rawData;
  BasicReader basicReader(".");
  basicReader.Read(&rawData);
  LOG(INFO) << "Aplication stop";
  return 0;
}
