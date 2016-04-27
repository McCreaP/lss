#include <boost/program_options.hpp>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "base/schedule.h"
#include "genetic/algorithm.h"
#include "genetic/selector_impl.h"
#include "genetic/permutation_chromosome/moves_impl.h"
#include "io/basic_input.h"
#include "io/basic_output.h"

namespace program_opt = boost::program_options;

using std::cout;
using std::string;
using GeneticPermutationAlgorithm
    = lss::genetic::GeneticAlgorithm<lss::genetic::PermutationJobMachine>;


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

GeneticPermutationAlgorithm BuildGeneticAlgorithm() {
  using namespace lss::genetic;

  int population_size = 50;
  int number_of_generations = 1000;
  double crossover_probability = 0.1;
  double mutation_probability = 0.01;

  auto rand = std::make_shared<lss::Random>();
  auto initializer = std::make_shared<InitializerImpl>(rand);
  auto evaluator = std::make_shared<EvaluatorImpl>();
  auto selector = std::make_shared<SelectorImpl<PermutationJobMachine>>(evaluator, rand);
  auto crosser = std::make_shared<CrosserImpl>(rand);
  auto mutator = std::make_shared<MutatorImpl>(mutation_probability, rand);
  auto moves = std::make_shared<ConfigurableMoves<PermutationJobMachine>>();
  (*moves)
      .SetInitializer(initializer)
      .SetSelector(selector)
      .SetCrosser(crosser)
      .SetSelector(selector)
      .SetMutator(mutator);

  return GeneticAlgorithm<PermutationJobMachine>(population_size, number_of_generations,
                                                 crossover_probability, moves, rand);
}

int main(int argc, char **argv) {
  program_opt::variables_map config = ProcessCommandLine(argc, argv);
  ConfigLogger(argv, config["verbose"].as<int>());
  LOG(INFO) << "Scheduler start";

  lss::io::BasicReader reader(config["input"].as<string>());
  lss::io::BasicWriter writer(config["assignments"].as<string>());
  lss::AssignmentsHandler assignments_handler(&writer);
  lss::Situation situation;
  lss::Schedule schedule;

  auto algorithm = BuildGeneticAlgorithm();
  while (true) {
    lss::RawSituation raw;
    if (reader.Read(&raw))
      situation = lss::Situation(raw);

    VLOG(2) << "Run next iteration";
    schedule = algorithm.Run(schedule, situation);

    assignments_handler.AdjustAssignments(schedule, situation);
  }

  LOG(INFO) << "Scheduler stop";
  return 0;
}
