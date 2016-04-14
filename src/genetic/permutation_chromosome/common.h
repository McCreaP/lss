#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_

#include <vector>

#include "base/random.h"
#include "base/situation.h"
#include "genetic/permutation_chromosome/chromosome.h"

namespace lss {
namespace genetic {

Machine FindRandomMachineForJob(Job job, Random *rand);

std::vector<JobMachine> GetPermutation(std::vector<int> job_permutation,
                                       std::vector<int> machine_permutation,
                                       const Situation &situation);

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_
