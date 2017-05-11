// Copyright (c) 2017, Carbon3d Inc, All rights reserved.
// Authors: saito@carbon3d.com

#include <gtest/gtest.h>
#include "weighted_reservoir_sampler.h"

TEST(WeightedReservoir, Empty) {
  WeightedReservoirSampler<double> s(0);
  s.Add(1);
  s.Add(2);
  EXPECT_TRUE(s.GetSamples().empty());
}

TEST(WeightedReservoir, Simple) {
  WeightedReservoirSampler<double> s(1000);
  for (int i = 0; i < 10000; ++i) {
    s.Add(i % 10);
  }
  EXPECT_EQ(1000, s.GetSamples().size());
}

int main(int argc, char**argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
