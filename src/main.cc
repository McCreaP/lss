#include <boost/program_options.hpp>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include "scheduler/greedy_scheduler.h"

namespace po = boost::program_options;
using std::string;
using std::cout;

void ConfigLogger(char *argv[]) {
  FLAGS_log_dir = "logs";
  FLAGS_stderrthreshold = 0;
  google::InitGoogleLogging(argv[0]);
}

po::variables_map ProcessCommandLine(int argc, char **argv) {
  po::variables_map vm;
  po::options_description desc("Scheduler options");
  desc.add_options()
      ("help", "produce help message")
      ("input", po::value<string>()->required(), "set input path")
      ("assignments", po::value<string>()->required(), "set assignments path");
  po::store(po::parse_command_line(argc, argv, desc), vm);
  if (vm.count("help")) {
    cout << desc << "\n";
    exit(1);
  }
  try {
    po::notify(vm);
  } catch (boost::exception_detail::clone_impl <
           boost::exception_detail::error_info_injector<
               boost::program_options::required_option>> exception) {
    cout << exception.what() << "\n";
    exit(1);
  }
  return vm;
}

int main(int argc, char **argv) {
  po::variables_map config = ProcessCommandLine(argc, argv);
  ConfigLogger(argv);
  LOG(INFO) << "Scheduler start";
  lss::GreedyScheduler scheduler(config["input"].as<string>(),
                                 config["assignments"].as<string>());
  scheduler.Schedule();
  LOG(INFO) << "Scheduler stop";
  return 0;
}
