#include "scheduler/greedy_scheduler.h"

#include <glog/logging.h>

void configLogger(char *argv[]) {
  FLAGS_log_dir = "logs";
  FLAGS_stderrthreshold = 0;
  google::InitGoogleLogging(argv[0]);
}

int main(int __attribute__((unused)) argc, char *argv[]) {
  configLogger(argv);
  LOG(INFO) << "Scheduler start";
  const std::string kRunPath = "/home/vagrant/lss/run/";
  lss::GreedyScheduler scheduler(kRunPath + "input", kRunPath + "assignments/");
  scheduler.Schedule();
  LOG(INFO) << "Scheduler stop";
  return 0;
}
