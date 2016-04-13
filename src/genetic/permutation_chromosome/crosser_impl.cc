#include <cstdlib>
#include <unordered_set>

#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/moves_impl.h"

namespace lss {
namespace genetic {
namespace {

void Merge(std::vector<JobMachine> *to, const std::vector<JobMachine> &from, size_t min_bound, size_t max_bound) {
  std::unordered_set<JobMachine> already_taken;
  for (size_t i = min_bound; i < max_bound; ++i) {
    already_taken.insert((*to)[i]);
  }

  auto it = from.begin();
  for (size_t i = 0; i < min_bound; ++i) {
    while (it != from.end() && already_taken.count(*it) > 0)
      ++it;
    (*to)[i] = *it;
  }
  for (size_t i = max_bound; i < to->size(); ++i) {
    while (it != from.end() && already_taken.count(*it) > 0)
      ++it;
    (*to)[i] = *it;
  }
}

}  // namespace

void CrosserImpl::Crossover(PermutationJobMachine *lhs, PermutationJobMachine *rhs) const {
  size_t permutation_size = lhs->permutation().size();
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
