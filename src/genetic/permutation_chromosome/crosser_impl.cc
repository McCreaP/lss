#include <set>
#include <cstdlib>

#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/moves_impl.h"

namespace lss {
namespace genetic {

void merge(Chromosome *lhs, const Chromosome &rhs, size_t minBound, size_t maxBound);

void CrosserImpl::crossover(Chromosome *lhs, Chromosome *rhs) const {
  size_t permutationSize = lhs->size();
  size_t firstBound = std::rand() % permutationSize;
  size_t secondBound = std::rand() % permutationSize;
  size_t minBound = std::min(firstBound, secondBound);
  size_t maxBound = std::max(firstBound, secondBound);

  Chromosome lhsCopy{*lhs};
  merge(lhs, *rhs, minBound, maxBound);
  merge(rhs, lhsCopy, minBound, maxBound);

}

namespace {

void merge(Chromosome *to, const Chromosome &from, size_t minBound, size_t maxBound) {
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
}  // namespace genetic
}  // namespace lss
