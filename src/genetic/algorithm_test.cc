#include "genetic/algorithm.h"

#include <memory>
#include <tuple>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "genetic/moves.h"
#include "genetic/mocks.h"
#include "genetic/selector_impl.h"

namespace lss {
namespace genetic {

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::InvokeWithoutArgs;

class CrosserFake: public Crosser<ChromosomeFake> {
 public:
  using CrossoverArgs = std::tuple<ChromosomeFake, ChromosomeFake>;
  void Crossover(ChromosomeFake *lhs, ChromosomeFake *rhs) const override {
    calledCrossoverArgs_.push_back(std::make_tuple(*lhs, *rhs));
  }

  std::vector<CrossoverArgs> GetCalledCrossoverArgs() {
    return calledCrossoverArgs_;
  }

 private:
  mutable std::vector<CrossoverArgs> calledCrossoverArgs_;
};

class MutatorFake: public Mutator<ChromosomeFake> {
 public:
  void Mutate(__attribute__((unused)) const Situation &situation, ChromosomeFake *chromosome) const override {
    invokedChromosomes_.push_back(*chromosome);
  }

  std::vector<ChromosomeFake> GetInvokedChromosomes() const {return invokedChromosomes_;}

 private:
  mutable std::vector<ChromosomeFake> invokedChromosomes_;
};

class AlgorithmShould : public ::testing::Test {
 protected:
  using Chromosome = ChromosomeFake;

  void SetUp() {
    moves_ = std::make_shared<MovesMock<Chromosome>>();
    rand_ = std::make_shared<RandomMock>();
    for (int i = 0; i < populationSize_; ++i) {
      population_.push_back(Chromosome(i));
    }
  }

  GeneticAlgorithm<Chromosome> BuildAlgorithm() {
    return BuildAlgorithm(moves_);
  }

  GeneticAlgorithm<Chromosome> BuildAlgorithm(std::shared_ptr<Moves<Chromosome>> moves) {
    return GeneticAlgorithm<Chromosome>(populationSize_, numberOfGenerations_, crossoverProbability_, moves, rand_);
  }

  void CrossoverTest(std::shared_ptr<CrosserFake> crosser, std::vector<double> randoms) {
    Situation situation(rawSituation_, false);
    auto initializer = std::make_shared<InitializerMock<Chromosome>>();
    auto selector = std::make_shared<SelectorMock<Chromosome>>();
    auto mutator = std::make_shared<MutatorMock<Chromosome>>();
    auto moves = std::make_shared<Moves<Chromosome>>();
    moves->SetInitializer(initializer)
        .SetSelector(selector)
        .SetMutator(mutator)
        .SetCrosser(crosser);

    EXPECT_CALL(*initializer, InitPopulation(_, populationSize_))
        .WillOnce(Return(population_));
    EXPECT_CALL(*selector, Select(population_, _))
        .WillOnce(Return(population_));
    EXPECT_CALL(*mutator, Mutate(_, _)).Times(populationSize_);

    Iterator<double> it(randoms);
    EXPECT_CALL(*rand_, GetRealInRange(0., 1.))
        .WillRepeatedly(InvokeWithoutArgs(&it, &Iterator<double>::Next));

    GeneticAlgorithm<Chromosome> algorithm = BuildAlgorithm(moves);
    algorithm.Run(schedule_, situation);
  }

  int populationSize_ = 5;
  int numberOfGenerations_ = 1;
  double crossoverProbability_ = 0.5;
  std::shared_ptr<MovesMock<Chromosome>> moves_;
  std::shared_ptr<RandomMock> rand_;
  Population<Chromosome> population_;
  Schedule schedule_;
  RawSituation rawSituation_;
};

TEST_F(AlgorithmShould, call_InitPopulation_once_and_RandomShuffle_and_Select_in_each_generation) {
  Situation situation(rawSituation_, false);
  numberOfGenerations_ = 42;
  EXPECT_CALL(*moves_, InitPopulation(_, populationSize_))
      .WillOnce(Return(Population<Chromosome>()));
  EXPECT_CALL(*moves_, Select(_, _))
      .Times(numberOfGenerations_)
      .WillRepeatedly(Return(Population<Chromosome>()));
  EXPECT_CALL(*rand_, RandomShuffle(_)).Times(numberOfGenerations_);

  GeneticAlgorithm<Chromosome> algorithm = BuildAlgorithm();
  algorithm.Run(schedule_, situation);
}

TEST_F(AlgorithmShould, take_chromosomes_to_crossover_according_to_generated_random_number) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto crosser = std::make_shared<CrosserFake>();

  CrossoverTest(crosser, {0.2, 0.6, 0.7, 0.1, 0.8});

  auto calledCrossoverArgs = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(1, calledCrossoverArgs.size());
  auto expectedCrossoverArgs = std::make_tuple(population_[0], population_[3]);
  EXPECT_EQ(expectedCrossoverArgs, calledCrossoverArgs[0]);
}

TEST_F(AlgorithmShould, not_crossover_multiple_times_the_same_chromosome) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto crosser = std::make_shared<CrosserFake>();

  CrossoverTest(crosser, {0.2, 0.6, 0.3, 0.1, 0.2});

  auto calledCrossoverArgs = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(2, calledCrossoverArgs.size());
  auto expectedCrossoverArgs1 = std::make_tuple(population_[0], population_[2]);
  EXPECT_EQ(expectedCrossoverArgs1, calledCrossoverArgs[0]);
  auto expectedCrossoverArgs2 = std::make_tuple(population_[3], population_[4]);
  EXPECT_EQ(expectedCrossoverArgs2, calledCrossoverArgs[1]);
}

TEST_F(AlgorithmShould, not_crossover_one_chromosome) {
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto crosser = std::make_shared<CrosserFake>();

  CrossoverTest(crosser, {0.6, 0.6, 0.3, 0.8, 0.7});

  auto calledCrossoverArgs = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(0, calledCrossoverArgs.size());
}

TEST_F(AlgorithmShould, take_chromosome_to_crossover_in_right_order) {
  auto crosser = std::make_shared<CrosserFake>();

  std::vector<size_t> order = {2, 3, 1, 4, 0};
  EXPECT_CALL(*rand_, RandomShuffle(_)).WillOnce(SetArgPointee<0>(order));
  CrossoverTest(crosser, {0., 1., 0., 0., 0.});

  auto calledCrossoverArgs = crosser->GetCalledCrossoverArgs();
  EXPECT_EQ(2, calledCrossoverArgs.size());
  auto expectedCrossoverArgs1 = std::make_tuple(ChromosomeFake(2), ChromosomeFake(1));
  EXPECT_EQ(expectedCrossoverArgs1, calledCrossoverArgs[0]);
  auto expectedCrossoverArgs2 = std::make_tuple(ChromosomeFake(4), ChromosomeFake(0));
  EXPECT_EQ(expectedCrossoverArgs2, calledCrossoverArgs[1]);
}

TEST_F(AlgorithmShould, run_mutator_on_each_chromosome_in_one_generation) {
  Situation situation(rawSituation_, false);
  EXPECT_CALL(*rand_, RandomShuffle(_));
  auto initializer = std::make_shared<InitializerMock<Chromosome>>();
  auto selector = std::make_shared<SelectorMock<Chromosome>>();
  auto mutator = std::make_shared<MutatorFake>();
  auto crosser = std::make_shared<CrosserMock<Chromosome>>();
  auto moves = std::make_shared<Moves<Chromosome>>();
  moves->SetInitializer(initializer)
      .SetSelector(selector)
      .SetMutator(mutator)
      .SetCrosser(crosser);

  EXPECT_CALL(*initializer, InitPopulation(_, populationSize_))
      .WillOnce(Return(population_));
  EXPECT_CALL(*selector, Select(_, _))
      .Times(numberOfGenerations_)
      .WillRepeatedly(Return(population_));
  EXPECT_CALL(*rand_, GetRealInRange(0., 1.))
      .Times(populationSize_)
      .WillRepeatedly(Return(1.));

  GeneticAlgorithm<Chromosome> algorithm = BuildAlgorithm(moves);
  algorithm.Run(schedule_, situation);

  std::vector<Chromosome> mutatedChromosomes = mutator->GetInvokedChromosomes();
  ASSERT_EQ(populationSize_, mutatedChromosomes.size());
  for (const Chromosome &chromosome : population_) {
    bool chromosomeWasMutated =
      std::find(mutatedChromosomes.begin(), mutatedChromosomes.end(), chromosome) != mutatedChromosomes.end();
    ASSERT_TRUE(chromosomeWasMutated);
  }
}

}  // namespace genetic
}  // namespace lss
