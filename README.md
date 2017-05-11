# WeightedReservoirSampler

Given a stream of values with weights, this class randomly and incrementally
samples K of them.  The cost is O(N log(K)), where N is the number of values.
