#include <set>
#include <cstdlib>

#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/moves_impl.h"

namespace lss {
namespace genetic {
namespace {

void Merge(std::vector<JobMachine> *to, const std::vector<JobMachine> &from, size_t minBound, size_t maxBound) {
  std::set<JobMachine> alreadyTaken;
  for (size_t i = minBound; i < maxBound; ++i) {
    alreadyTaken.insert((*to)[i]);
  }

  auto it = from.begin();
  for (size_t i = 0; i < minBound; ++i) {
    while (it != from.end() && alreadyTaken.find(*it) != alreadyTaken.end()) ++it;
    (*to)[i] = *it;
  }
  for (size_t i = maxBound; i < to->size(); ++i) {
    while (it != from.end() && alreadyTaken.find(*it) != alreadyTaken.end()) ++it;
    (*to)[i] = *it;
  }
}

}  // namespace

void CrosserImpl::Crossover(PermutationJobMachine *lhs, PermutationJobMachine *rhs) const {
  size_t permutationSize = lhs->permutation().size();
  size_t firstBound = rand_->Rand(permutationSize);
  size_t secondBound = rand_->Rand(permutationSize);
  size_t minBound = std::min(firstBound, secondBound);
  size_t maxBound = std::max(firstBound, secondBound);

  PermutationJobMachine lhsCopy = *lhs;
  Merge(&lhs->permutation(), rhs->permutation(), minBound, maxBound);
  Merge(&rhs->permutation(), lhsCopy.permutation(), minBound, maxBound);
}

}  // namespace genetic
}  // namespace lss
