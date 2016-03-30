#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H

#include "base/situation.h"

namespace lss {
namespace genetic {

const Machine *findRandomMachineForJob(const Situation &situation, const Job &job);

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H
