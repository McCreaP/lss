#include <cstdlib>
#include <unordered_set>

#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/moves_impl.h"

namespace lss {
namespace genetic {
namespace {

void Merge(std::vector<JobMachine> *to,
           const std::vector<JobMachine> &from,
           size_t min_bound,
           size_t max_bound) {
  std::unordered_set<Job> already_taken;
  for (size_t i = min_bound; i < max_bound; ++i) {
    already_taken.insert(std::get<0>((*to)[i]));
  }

  auto it = from.begin();
  for (size_t i = 0; i < min_bound; ++i) {
    // I assume vectors '*to' and 'from' have the same size
    // and are permutations by jobs, thus I don't need to check
    // that the iterator is valid.
    while (already_taken.count(std::get<0>(*it)) > 0)
      ++it;
    (*to)[i] = *it;
    ++it;
  }
  for (size_t i = max_bound; i < to->size(); ++i) {
    while (already_taken.count(std::get<0>(*it)) > 0)
      ++it;
    (*to)[i] = *it;
    ++it;
  }
}

}  // namespace

void CrosserImpl::Crossover(PermutationJobMachine *lhs, PermutationJobMachine *rhs) const {
  size_t permutation_size = lhs->permutation().size();
  if (permutation_size == 0) {
    return;
  }
  size_t first_bound = rand_->Rand(permutation_size);
  size_t second_bound = rand_->Rand(permutation_size);
  size_t min_bound = std::min(first_bound, second_bound);
  size_t max_bound = std::max(first_bound, second_bound);

  PermutationJobMachine lhs_copy = *lhs;
  Merge(&lhs->permutation(), rhs->permutation(), min_bound, max_bound);
  Merge(&rhs->permutation(), lhs_copy.permutation(), min_bound, max_bound);
}

}  // namespace genetic
}  // namespace lss
