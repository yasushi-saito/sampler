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

int main(int argc, char**argv) {
  return RUN_ALL_TESTS();
}