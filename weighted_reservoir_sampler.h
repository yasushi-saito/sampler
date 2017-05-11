#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <queue>
#include <random>

// Weighted random sampling with a reservoir. Thread compatible.
//
// PS Efraimidis, PG Spirakis - Information Processing Letters, 2006
//
// http://utopia.duth.gr/~pefraimi/research/data/2007EncOfAlg.pdf

template <typename T, typename Compare = std::less<T>>
class WeightedReservoirSampler {
 public:
  WeightedReservoirSampler(int max_samples, Compare less = Compare())
      : max_samples_(max_samples),
        less_(std::move(less)),
        rand_(0),
        dist_(0, 1) {}

  // Add a new value. The same as WeightedAdd(value, 1).
  void Add(const T& value) { WeightedAdd(value, 1); }

  // Add a new value with the given weight. The weight defines the relative
  // probability that the value is sampled. The higher the weight, the more
  // likely the value is chosen.
  void WeightedAdd(const T& value, double weight) {
    if (state_ != State::HEAP) {
      std::make_heap(samples_.begin(), samples_.end(), SampleLessThan);
      state_ = State::HEAP;
    }
    const double u = dist_(rand_);
    Sample sample = {value, weight, std::pow(u, 1.0 / weight)};

    if (samples_.size() >= std::max(1, max_samples_) &&
        sample.key < samples_.front().key) {
      // Common case. The new sample smaller than the min key in samples_.
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
  const std::vector<Sample>& Samples() const { return samples_; }

  bool Empty() const { return samples_.empty(); }

  const T& Quantile(double fraction) {
    if (state_ != State::SORTED) {
      std::stable_sort(samples_.begin(), samples_.end(),
                       [this](const Sample& s0, const Sample& s1) -> bool {
                         return less_(s0.value, s1.value);
                       });
      state_ = State::SORTED;
    }

    assert(!samples_.empty());
    int index = fraction * samples_.size();
    if (index < 0) index = 0;
    if (index >= samples_.size()) index = samples_.size() - 1;
    return samples_[index].value;
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
};
