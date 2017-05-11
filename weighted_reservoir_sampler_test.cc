// Copyright (c) 2017, Carbon3d Inc, All rights reserved.
// Authors: saito@carbon3d.com

#include "weighted_reservoir_sampler.h"
#include <gtest/gtest.h>

TEST(WeightedReservoir, Empty) {
  WeightedReservoirSampler<double> s(0);
  s.Add(1);
  s.Add(2);
  EXPECT_TRUE(s.Samples().empty());
}

TEST(WeightedReservoir, Uniform) {
  WeightedReservoirSampler<double> s(1000);
  for (int i = 0; i < 10000; ++i) {
    s.Add(i % 10);
  }
  EXPECT_EQ(1000, s.Samples().size());
  for (double f = 0.1; f < 1.2; f += 0.1) {
    std::cout << "Frac " << f << ": " << s.Quantile(f) << "\n";
  }
  EXPECT_EQ(0, s.Quantile(0.05));
  EXPECT_EQ(1, s.Quantile(0.15));
  EXPECT_EQ(2, s.Quantile(0.25));
  EXPECT_EQ(3, s.Quantile(0.35));
  EXPECT_EQ(4, s.Quantile(0.45));
  EXPECT_EQ(5, s.Quantile(0.55));
  EXPECT_EQ(6, s.Quantile(0.65));
  EXPECT_EQ(7, s.Quantile(0.75));
  EXPECT_EQ(8, s.Quantile(0.85));
  EXPECT_EQ(9, s.Quantile(0.95));
}

void CheckWeightedSamples(WeightedReservoirSampler<double>* s) {
  EXPECT_EQ(1000, s->Samples().size());
  int counts[3] = {0};
  for (double f = 0; f < 1.0; f += 0.01) {
    double q = s->Quantile(f);
    EXPECT_TRUE(q == 1 || q == 2 || q == 0);
    counts[static_cast<int>(q)]++;
  }
  // 0 should appear 1/6
  EXPECT_GT(counts[0], 0);
  EXPECT_LT(counts[0], 20);
  // 1 should appear 2/6
  EXPECT_GT(counts[1], 20);
  EXPECT_LT(counts[1], 40);
  // 2 should appear 3/6
  EXPECT_GT(counts[2], 40);
  EXPECT_LT(counts[2], 60);
}

TEST(WeightedReservoir, Weighted_Alternating) {
  WeightedReservoirSampler<double> s(1000);
  for (int i = 0; i < 50000; ++i) {
    s.WeightedAdd(i % 3, (i % 3) + 1);
  }
  CheckWeightedSamples(&s);
}

TEST(WeightedReservoir, Weighted_Sequential) {
  WeightedReservoirSampler<double> s(1000);
  for (int i = 0; i < 10000; ++i) {
    s.WeightedAdd(0, 1);
  }
  for (int i = 0; i < 10000; ++i) {
    s.WeightedAdd(1, 2);
  }
  for (int i = 0; i < 10000; ++i) {
    s.WeightedAdd(2, 3);
  }
  CheckWeightedSamples(&s);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
