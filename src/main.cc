#include <boost/program_options.hpp>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include "greedy/greedy_scheduler.h"

namespace program_opt = boost::program_options;
using std::string;
using std::cout;

void ConfigLogger(char *argv[], int verbose) {
  FLAGS_v = verbose;
  FLAGS_log_dir = "logs";
  FLAGS_stderrthreshold = 0;
  FLAGS_logbufsecs = 0;
  FLAGS_colorlogtostderr = true;
  google::InitGoogleLogging(argv[0]);
}

program_opt::variables_map ProcessCommandLine(int argc, char **argv) {
  program_opt::variables_map variables_map;
  program_opt::options_description desc("Scheduler options");
  desc.add_options()
      ("help,h", "produce help message")
      ("input,i", program_opt::value<string>()->required(), "Set input file path")
      ("assignments,a", program_opt::value<string>()->required(), "Set assignments directory path")
      ("verbose,v", program_opt::value<int>(), "Set verbosity level");
  program_opt::store(program_opt::parse_command_line(argc, argv, desc), variables_map);

  if (variables_map.count("help")) {
    cout << desc << "\n";
    exit(0);
  }
  program_opt::notify(variables_map);
  return variables_map;
}

int main(int argc, char **argv) {
  program_opt::variables_map config = ProcessCommandLine(argc, argv);
  ConfigLogger(argv, config["verbose"].as<int>());
  LOG(INFO) << "Scheduler start";
  lss::greedy::GreedyScheduler scheduler(config["input"].as<string>(),
                                         config["assignments"].as<string>());
  scheduler.Schedule();
  LOG(INFO) << "Scheduler stop";
  return 0;
}
