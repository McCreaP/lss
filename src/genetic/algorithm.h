#ifndef LSS_GENETIC_ALGORITHM_H_
#define LSS_GENETIC_ALGORITHM_H_

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "base/algorithm.h"
#include "base/random.h"
#include "base/schedule_mock.h"
#include "base/situation.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

template<class T>
class GeneticAlgorithm : public Algorithm {
 public:
  GeneticAlgorithm(int population_size,
                   int number_of_generations,
                   double crossover_probability,
                   std::shared_ptr<Moves<T>> moves,
                   std::shared_ptr<Random> rand)
      : population_size_(population_size),
        number_of_generations_(number_of_generations),
        crossover_probability_(crossover_probability),
        moves_(moves),
        rand_(rand) { }

  Schedule Run(const Schedule &prev_schedule, Situation new_situation) override;

 private:
  void Crossover(Population<T> *population);
  void Mutate(Situation situation, Population<T> *population);

  int population_size_;
  int number_of_generations_;
  double crossover_probability_;
  std::shared_ptr<Moves<T>> moves_;
  std::shared_ptr<Random> rand_;
};

template<class T>
Schedule GeneticAlgorithm<T>::Run(__attribute__((unused)) const Schedule &prev_schedule,
                                  Situation new_situation) {
  ChromosomeImprover<T> improver;
  Population<T> population = moves_->InitPopulation(new_situation, population_size_);
  for (int generation = 0; generation < number_of_generations_; ++generation) {
    population = moves_->Select(population, &improver);
    Crossover(&population);
    Mutate(new_situation, &population);
  }
  return improver.GetBestChromosome().ToSchedule();
}

template<class T>
void GeneticAlgorithm<T>::Crossover(Population<T> *population) {
  std::vector<size_t> indexes(population->size());
  std::iota(std::begin(indexes), std::end(indexes), 0);
  rand_->RandomShuffle(&indexes);
  T *chromosome_waiting_for_crossover = nullptr;
  for (size_t i = 0; i < population->size(); ++i) {
    bool take_to_crossover = rand_->GetRealInRange(0., 1.) < crossover_probability_;
    if (take_to_crossover) {
      if (chromosome_waiting_for_crossover) {
        moves_->Crossover(chromosome_waiting_for_crossover, &(*population)[indexes[i]]);
        chromosome_waiting_for_crossover = nullptr;
      } else {
        chromosome_waiting_for_crossover = &(*population)[indexes[i]];
      }
    }
  }
}

template<class T>
void GeneticAlgorithm<T>::Mutate(Situation situation, Population<T> *population) {
  for (T &chromosome : *population) {
    moves_->Mutate(situation, &chromosome);
  }
}

class Chromosome {
 public:
  virtual Schedule ToSchedule() const = 0;
  virtual ~Chromosome() = default;
};

class ChromosomeFake : public Chromosome {
 public:
  ChromosomeFake() : id_(-1) {}
  explicit ChromosomeFake(int id) : id_(id) {}

  friend bool operator==(const ChromosomeFake &lhs, const ChromosomeFake &rhs) {
    return lhs.id_ == rhs.id_;
  }

  friend std::ostream &operator<<(std::ostream &os, const ChromosomeFake &chromosome) {
    os << "Chromosome: " << chromosome.id_;
    return os;
  }

  Schedule ToSchedule() const override {return Schedule();}

 private:
  int id_;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_ALGORITHM_H_
