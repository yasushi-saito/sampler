// Copyright (c) 2017, Carbon3d Inc, All rights reserved.
// Authors: saito@carbon3d.com

#pragma once

#include <cmath>
#include <queue>
#include <random>

// Weighted random sampling with a reservoir
//
// PS Efraimidis, PG Spirakis - Information Processing Letters, 2006
//
// http://utopia.duth.gr/~pefraimi/research/data/2007EncOfAlg.pdf

template <typename T>
class WeightedReservoirSampler {
 public:
  WeightedReservoirSampler(int max_samples)
      : max_samples_(max_samples), rand_(0), dist_(0, 1) {}

  void Add(const T& value) { WeightedAdd(value, 1); }

  void WeightedAdd(const T& value, double weight) {
    const double u = dist_(rand_);
    Sample sample = {value, weight, std::pow(u, 1.0 / weight)};

    if (!samples_.empty() && sample.weight < samples_.back().weight) {
      // Common case.
      return;
    }
    samples_.push_back(sample);
    std::push_heap(samples_.begin(), samples_.end(), SampleLessThan);
    while (samples_.size() > max_samples_) {
      std::pop_heap(samples_.begin(), samples_.end(), SampleLessThan);
      samples_.pop_back();
    }
  }

  struct Sample {
    T value;
    double weight;
    double key;
  };
  const std::vector<Sample>& GetSamples() const { return samples_; }

 private:
  static bool SampleLessThan(const Sample& s1, const Sample& s2) {
    return s1.key > s2.key;  // smallest key is sorted last
  }

  const int max_samples_;
  std::mt19937 rand_;
  std::uniform_real_distribution<double> dist_;
  std::vector<Sample> samples_;
};
