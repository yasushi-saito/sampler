#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>
#include <random>

// Weighted random sampling with a reservoir
//
// PS Efraimidis, PG Spirakis - Information Processing Letters, 2006
//
// http://utopia.duth.gr/~pefraimi/research/data/2007EncOfAlg.pdf

template <typename T, typename Compare = std::less<T>>
class WeightedReservoirSampler {
 public:
  static constexpr int kDefaultMaxSamples = 65536;
  WeightedReservoirSampler(int max_samples = kDefaultMaxSamples,
                           Compare less = Compare())
      : max_samples_(max_samples),
        less_(std::move(less)),
        rand_(0),
        dist_(0, 1) {}

  void Add(const T& value) { WeightedAdd(value, 1); }

  void WeightedAdd(const T& value, double weight) {
    if (state_ != State::HEAP) {
      std::make_heap(samples_.begin(), samples_.end(), SampleLessThan);
      state_ = State::HEAP;
    }
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

  bool Empty() const { return samples_.empty(); }

  const T& Quantile(double fraction) {
    if (state_ != State::SORTED) {
      std::stable_sort(samples_.begin(), samples_.end(), less_);
      total_weight_ = 0;
      for (const Sample& s : samples_) {
        total_weight_ += s.weight;
      }
      state_ = State::SORTED;
    }

    CHECK(!samples_.empty());
    double cumulative_weight = 0;
    for (const Sample& s : samples_) {
      cumulative_weight += s.weight;
      if (cumulative_weight >= fraction * total_weight_) {
        return s.value;
      }
    }
    return samples_.back().value();
  }

 private:
  static bool SampleLessThan(const Sample& s1, const Sample& s2) {
    return s1.key > s2.key;  // smallest key is sorted last
  }
  enum class State {
    UNKNOWN,  // initial state.
    HEAP,     // samples_ forms a heap.
    SORTED    // samples_ are sorted by value order.
  };
  const int max_samples_;
  const Compare less_;
  std::mt19937 rand_;
  std::uniform_real_distribution<double> dist_;

  State state_ = State::UNKNOWN;
  std::vector<Sample> samples_;

  double total_weight_ = 0;
};
