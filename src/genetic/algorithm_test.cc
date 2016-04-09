#include "genetic/algorithm.h"

#include <memory>
#include <tuple>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "genetic/moves.h"
#include "genetic/selector_impl.h"

namespace lss {
namespace genetic {

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::InvokeWithoutArgs;

class CrosserFake : public Crosser<ChromosomeFake> {
 public:
  using CrossoverArgs = std::tuple<ChromosomeFake, ChromosomeFake>;
  void Crossover(ChromosomeFake *lhs, ChromosomeFake *rhs) const override {
    called_crossover_args_.push_back(std::make_tuple(*lhs, *rhs));
  }

  std::vector<CrossoverArgs> GetCalledCrossoverArgs() {
    return called_crossover_args_;
  }

 private:
  mutable std::vector<CrossoverArgs> called_crossover_args_;
};

class MutatorFake : public Mutator<ChromosomeFake> {
 public:
  void Mutate(__attribute__((unused)) const Situation &situation,
              ChromosomeFake *chromosome) const override {
    invoked_chromosomes_.push_back(*chromosome);
  }

  std::vector<ChromosomeFake> GetInvokedChromosomes() const { return invoked_chromosomes_; }

 private:
  mutable std::vector<ChromosomeFake> invoked_chromosomes_;
};

template<class T>
class Iterator {
 public:
  explicit Iterator(std::vector<T> v) : v_(std::move(v)) { }

  T Next() {
    return v_[counter_++];
  }

 private:
  int counter_ = 0;
  std::vector<T> v_;
};

class AlgorithmShould : public ::testing::Test {
 protected:
  using Chromosome = ChromosomeFake;

  void SetUp() {
    moves_ = std::make_shared<MovesMock<Chromosome>>();
    rand_ = std::make_shared<RandomMock>();
    for (int i = 0; i < population_size_; ++i) {
      population_.push_back(Chromosome(i));
    }
  }

  GeneticAlgorithm<Chromosome> BuildAlgorithm() {
    return BuildAlgorithm(moves_);
  }

  GeneticAlgorithm<Chromosome> BuildAlgorithm(std::shared_ptr<Moves<Chromosome>> moves) {
    return GeneticAlgorithm<Chromosome>(population_size_,
                                        number_of_generations_,
                                        crossover_probability_,
                                        moves,
                                        rand_);
  }

  void CrossoverTest(std::shared_ptr<CrosserFake> crosser, std::vector<double> randoms) {
    auto initializer = std::make_shared<InitializerMock<Chromosome>>();
    auto selector = std::make_shared<SelectorMock<Chromosome>>();
    auto mutator = std::make_shared<MutatorMock<Chromosome>>();
    auto moves = std::make_shared<ConfigurableMoves<Chromosome>>();
    moves->SetInitializer(initializer)
        .SetSelector(selector)
        .SetMutator(mutator)
        .SetCrosser(crosser);

    EXPECT_CALL(*initializer, InitPopulation(_, population_size_))
        .WillOnce(Return(population_));
    EXPECT_CALL(*selector, Select(population_, _))
        .WillOnce(Return(population_));
    EXPECT_CALL(*mutator, Mutate(_, _)).Times(population_size_);

    Iterator<double> it(randoms);
    EXPECT_CALL(*rand_, GetRealInRange(0., 1.))
        .WillRepeatedly(InvokeWithoutArgs(&it, &Iterator<double>::Next));

    GeneticAlgorithm<Chromosome> algorithm = BuildAlgorithm(moves);
    algorithm.Run(schedule_, situation_);
  }

  int population_size_ = 5;
  int number_of_generations_ = 1;
  double crossover_probability_ = 0.5;
  std::shared_ptr<MovesMock<Chromosome>> moves_;
  std::shared_ptr<RandomMock> rand_;
  Population<Chromosome> population_;
  Schedule schedule_;
  Situation situation_;
};

TEST_F(AlgorithmShould, call_InitPopulation_once_and_RandomShuffle_and_Select_in_each_generation) {
  number_of_generations_ = 42;
  EXPECT_CALL(*moves_, InitPopulation(_, population_size_))
      .WillOnce(Return(Population<Chromosome>()));
  EXPECT_CALL(*moves_, Select(_, _))
      .Times(number_of_generations_)
      .WillRepeatedly(Return(Population<Chromosome>()));
  EXPECT_CALL(*rand_, RandomShuffle(_)).Times(number_of_generations_);

  GeneticAlgorithm<Chromosome> algorithm = BuildAlgorithm();
  algorithm.Run(schedule_, situation_);
}

TEST_F(AlgorithmShould, take_chromosomes_to_crossover_according_to_generated_random_number) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto crosser = std::make_shared<CrosserFake>();

  CrossoverTest(crosser, {0.2, 0.6, 0.7, 0.1, 0.8});

  auto called_crossover_args = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(1, called_crossover_args.size());
  auto expected_crossover_args = std::make_tuple(population_[0], population_[3]);
  EXPECT_EQ(expected_crossover_args, called_crossover_args[0]);
}

TEST_F(AlgorithmShould, not_crossover_multiple_times_the_same_chromosome) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto crosser = std::make_shared<CrosserFake>();

  CrossoverTest(crosser, {0.2, 0.6, 0.3, 0.1, 0.2});

  auto called_crossover_args = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(2, called_crossover_args.size());
  auto expected_crossover_args1 = std::make_tuple(population_[0], population_[2]);
  EXPECT_EQ(expected_crossover_args1, called_crossover_args[0]);
  auto expected_crossover_args2 = std::make_tuple(population_[3], population_[4]);
  EXPECT_EQ(expected_crossover_args2, called_crossover_args[1]);
}

TEST_F(AlgorithmShould, not_crossover_one_chromosome) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto crosser = std::make_shared<CrosserFake>();

  CrossoverTest(crosser, {0.6, 0.6, 0.3, 0.8, 0.7});

  auto called_crossover_args = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(0, called_crossover_args.size());
}

TEST_F(AlgorithmShould, take_chromosome_to_crossover_in_right_order) {
  auto crosser = std::make_shared<CrosserFake>();

  std::vector<size_t> order = {2, 3, 1, 4, 0};
  EXPECT_CALL(*rand_, RandomShuffle(_)).WillOnce(SetArgPointee<0>(order));
  CrossoverTest(crosser, {0., 1., 0., 0., 0.});

  auto called_crossover_args = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(2, called_crossover_args.size());
  auto expected_crossover_args1 = std::make_tuple(ChromosomeFake(2), ChromosomeFake(1));
  EXPECT_EQ(expected_crossover_args1, called_crossover_args[0]);
  auto expected_crossover_args2 = std::make_tuple(ChromosomeFake(4), ChromosomeFake(0));
  EXPECT_EQ(expected_crossover_args2, called_crossover_args[1]);
}

TEST_F(AlgorithmShould, run_mutator_on_each_chromosome_in_one_generation) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto initializer = std::make_shared<InitializerMock<Chromosome>>();
  auto selector = std::make_shared<SelectorMock<Chromosome>>();
  auto mutator = std::make_shared<MutatorFake>();
  auto crosser = std::make_shared<CrosserMock<Chromosome>>();
  auto moves = std::make_shared<ConfigurableMoves<Chromosome>>();
  moves->SetInitializer(initializer)
      .SetSelector(selector)
      .SetMutator(mutator)
      .SetCrosser(crosser);

  EXPECT_CALL(*initializer, InitPopulation(_, population_size_))
      .WillOnce(Return(population_));
  EXPECT_CALL(*selector, Select(_, _))
      .Times(number_of_generations_)
      .WillRepeatedly(Return(population_));
  EXPECT_CALL(*rand_, GetRealInRange(0., 1.))
      .Times(population_size_)
      .WillRepeatedly(Return(1.));

  GeneticAlgorithm<Chromosome> algorithm = BuildAlgorithm(moves);
  algorithm.Run(schedule_, situation_);

  std::vector<Chromosome> muated = mutator->GetInvokedChromosomes();
  ASSERT_EQ(population_size_, muated.size());
  for (const Chromosome &chromosome : population_) {
    bool chromosome_was_mutated =
        std::find(muated.begin(), muated.end(), chromosome) != muated.end();
    ASSERT_TRUE(chromosome_was_mutated);
  }
}

}  // namespace genetic
}  // namespace lss
