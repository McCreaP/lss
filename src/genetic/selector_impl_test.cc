#include "genetic/selector_impl.h"

#include <limits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "base/random.h"
#include "genetic/moves.h"
#include "genetic/algorithm.h"
#include "genetic/mocks.h"

namespace lss {
namespace genetic {

using ::testing::_;
using ::testing::Return;

class SelectorShould : public ::testing::Test {
 protected:
  void SetUp() {
    evaluator_ = std::make_shared<EvaluatorMock<ChromosomeMock>>();
    rand_ = std::make_shared<RandomMock>();
    for (int i = 0; i < kPopulationSize; ++i) {
      population_.push_back(ChromosomeMock(i));
    }
  }

  const int kPopulationSize = 4;
  Population<ChromosomeMock> population_;
  ImproverMock<ChromosomeMock> improver_;
  std::shared_ptr<EvaluatorMock<ChromosomeMock>> evaluator_;
  std::shared_ptr<RandomMock> rand_;
};

TEST_F(SelectorShould, evaluate_each_chromosome) {
  EXPECT_CALL(*rand_, GetRealInRange(_, _)).Times(kPopulationSize);
  EXPECT_CALL(improver_, TryImprove(_));
  for (const auto &chromosome : population_) {
    EXPECT_CALL(*evaluator_, Evaluate(chromosome));
  }

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  selector.Select(population_, &improver_);
}

TEST_F(SelectorShould, return_population_with_the_same_size) {
  EXPECT_CALL(*rand_, GetRealInRange(_, _)).Times(kPopulationSize);
  EXPECT_CALL(improver_, TryImprove(_));
  EXPECT_CALL(*evaluator_, Evaluate(_))
      .Times(kPopulationSize);

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver_);

  EXPECT_EQ(kPopulationSize, newPopulation.size());
}

TEST_F(SelectorShould, select_chromosomes_to_new_population_according_to_generated_random_numbers) {
  EXPECT_CALL(improver_, TryImprove(_));
  // Cumulative Fitness: 20, 30, 55, 70
  EXPECT_CALL(*evaluator_, Evaluate(_))
      .WillOnce(Return(20))
      .WillOnce(Return(10))
      .WillOnce(Return(25))
      .WillOnce(Return(15));
  EXPECT_CALL(*rand_, GetRealInRange(0., 70.))
      .WillOnce(Return(40))
      .WillOnce(Return(60))
      .WillOnce(Return(5))
      .WillOnce(Return(30));

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver_);

  Population<ChromosomeMock> expectedPopulation = {
      ChromosomeMock(2),
      ChromosomeMock(3),
      ChromosomeMock(0),
      ChromosomeMock(1)
  };
  EXPECT_EQ(expectedPopulation, newPopulation);
}

TEST_F(SelectorShould, take_multiple_times_the_same_chromosome_if_random_number_says_so) {
  EXPECT_CALL(improver_, TryImprove(_));
  // Cumulative Fitness: 20, 30, 55, 70
  EXPECT_CALL(*evaluator_, Evaluate(_))
      .WillOnce(Return(20))
      .WillOnce(Return(10))
      .WillOnce(Return(25))
      .WillOnce(Return(15));
  EXPECT_CALL(*rand_, GetRealInRange(0., 70.))
      .WillOnce(Return(40))
      .WillOnce(Return(50))
      .WillOnce(Return(5))
      .WillOnce(Return(45));

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver_);

  Population<ChromosomeMock> expectedPopulation = {
      ChromosomeMock(2),
      ChromosomeMock(2),
      ChromosomeMock(0),
      ChromosomeMock(2)
  };
  EXPECT_EQ(expectedPopulation, newPopulation);
}

TEST_F(SelectorShould, change_improver_to_return_best_chromosome_from_first_population) {
  EXPECT_CALL(*rand_, GetRealInRange(_, _)).Times(kPopulationSize);
  ChromosomeImprover<ChromosomeMock> improver;
  std::vector<double> evaluations = {40, 30, 120, 70};
  for (size_t i = 0; i < population_.size(); ++i) {
    EXPECT_CALL(*evaluator_, Evaluate(population_[i]))
        .WillOnce(Return(evaluations[i]));
  }

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver);

  EXPECT_EQ(population_[2], improver.GetBestChromosome());
  EXPECT_EQ(120, improver.GetBestFitness());
}

TEST_F(SelectorShould, change_improver_if_has_found_better_chromosome) {
  EXPECT_CALL(*rand_, GetRealInRange(_, _)).Times(kPopulationSize);
  ChromosomeImprover<ChromosomeMock> improver;
  static const ChromosomeMock kPrevBestChromosome;
  static const int kPrevBestFitness = 100;
  improver.TryImprove(kPrevBestChromosome, kPrevBestFitness);
  ASSERT_EQ(improver.GetBestChromosome(), kPrevBestChromosome);
  ASSERT_EQ(improver.GetBestFitness(), kPrevBestFitness);

  std::vector<double> evaluations = {40, 30, 120, 70};
  for (size_t i = 0; i < population_.size(); ++i) {
    EXPECT_CALL(*evaluator_, Evaluate(population_[i]))
        .WillOnce(Return(evaluations[i]));
  }

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver);

  EXPECT_EQ(population_[2], improver.GetBestChromosome());
  EXPECT_EQ(120, improver.GetBestFitness());
}

TEST_F(SelectorShould, change_improver_to_return_best_chromosome_if_many_in_population_are_better) {
  EXPECT_CALL(*rand_, GetRealInRange(_, _)).Times(kPopulationSize);
  ChromosomeImprover<ChromosomeMock> improver;
  static const ChromosomeMock kPrevBestChromosome;
  static const int kPrevBestFitness = 100;
  improver.TryImprove(kPrevBestChromosome, kPrevBestFitness);
  ASSERT_EQ(improver.GetBestChromosome(), kPrevBestChromosome);
  ASSERT_EQ(improver.GetBestFitness(), kPrevBestFitness);

  std::vector<double> evaluations = {90, 120, 170, 130};
  for (size_t i = 0; i < population_.size(); ++i) {
    EXPECT_CALL(*evaluator_, Evaluate(population_[i]))
        .WillOnce(Return(evaluations[i]));
  }

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver);

  EXPECT_EQ(population_[2], improver.GetBestChromosome());
  EXPECT_EQ(170, improver.GetBestFitness());
}

TEST_F(SelectorShould, not_change_improver_if_has_not_found_better_chromosome) {
  EXPECT_CALL(*rand_, GetRealInRange(_, _)).Times(kPopulationSize);
  ChromosomeImprover<ChromosomeMock> improver;
  static const ChromosomeMock kPrevBestChromosome;
  static const int kPrevBestFitness = 100;
  improver.TryImprove(kPrevBestChromosome, kPrevBestFitness);
  ASSERT_EQ(improver.GetBestChromosome(), kPrevBestChromosome);
  ASSERT_EQ(improver.GetBestFitness(), kPrevBestFitness);

  std::vector<double> evaluations = {40, 30, 90, 70};
  for (size_t i = 0; i < population_.size(); ++i) {
    EXPECT_CALL(*evaluator_, Evaluate(population_[i]))
        .WillOnce(Return(evaluations[i]));
  }

  SelectorImpl<ChromosomeMock> selector(evaluator_, rand_);
  Population<ChromosomeMock> newPopulation = selector.Select(population_, &improver);

  EXPECT_EQ(kPrevBestChromosome, improver.GetBestChromosome());
  EXPECT_EQ(kPrevBestFitness, improver.GetBestFitness());
}

TEST(ChromosomeImproverShould, return_default_chromosome_and_min_fitness_before_improve) {
  ChromosomeImprover<ChromosomeMock> improver;
  ASSERT_EQ(ChromosomeMock(), improver.GetBestChromosome());
  ASSERT_EQ(std::numeric_limits<double>::min(), improver.GetBestFitness());
}

TEST(ChromosomeImproverShould, take_better_chromosome) {
  ChromosomeImprover<ChromosomeMock> improver1, improver2;
  static const ChromosomeMock kChromosome1(1);
  static const double kFitness1 = 10;
  static const ChromosomeMock kChromosome2(2);
  static const double kFitness2 = 20;
  improver1.TryImprove(kChromosome1, kFitness1);
  ASSERT_EQ(kChromosome1, improver1.GetBestChromosome());
  ASSERT_EQ(kFitness1, improver1.GetBestFitness());
  improver2.TryImprove(improver1);
  ASSERT_EQ(kChromosome1, improver2.GetBestChromosome());
  ASSERT_EQ(kFitness1, improver2.GetBestFitness());

  improver1.TryImprove(kChromosome2, kFitness2);
  ASSERT_EQ(kChromosome2, improver1.GetBestChromosome());
  ASSERT_EQ(kFitness2, improver1.GetBestFitness());
  improver2.TryImprove(improver1);
  ASSERT_EQ(kChromosome2, improver2.GetBestChromosome());
  ASSERT_EQ(kFitness2, improver2.GetBestFitness());
}

TEST(ChromosomeImproverShould, not_take_worse_chromosome) {
  ChromosomeImprover<ChromosomeMock> improver1, improver2, worseImprover;
  static const ChromosomeMock kChromosome1(1);
  static const double kFitness1 = 20;
  static const ChromosomeMock kChromosome2(2);
  static const double kFitness2 = 10;
  worseImprover.TryImprove(kChromosome2, kFitness2);

  improver1.TryImprove(kChromosome1, kFitness1);
  ASSERT_EQ(kChromosome1, improver1.GetBestChromosome());
  ASSERT_EQ(kFitness1, improver1.GetBestFitness());
  improver2.TryImprove(improver1);
  ASSERT_EQ(kChromosome1, improver2.GetBestChromosome());
  ASSERT_EQ(kFitness1, improver2.GetBestFitness());

  improver1.TryImprove(kChromosome2, kFitness2);
  ASSERT_EQ(kChromosome1, improver1.GetBestChromosome());
  ASSERT_EQ(kFitness1, improver1.GetBestFitness());
  improver2.TryImprove(worseImprover);
  ASSERT_EQ(kChromosome1, improver2.GetBestChromosome());
  ASSERT_EQ(kFitness1, improver2.GetBestFitness());
}

}  // namespace genetic
}  // namespace lss