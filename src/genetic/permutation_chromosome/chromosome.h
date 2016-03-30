#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H

#include "base/situation.h"

namespace lss {
namespace genetic {

using JobMachine = std::tuple<const lss::Job *, const lss::Machine *>;
using Chromosome = std::vector<JobMachine>;

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H
