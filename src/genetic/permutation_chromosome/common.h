#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_

#include <vector>

#include "base/random.h"
#include "base/situation.h"

namespace lss {
namespace genetic {

Machine FindRandomMachineForJob(Job job, Random *rand);

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_
