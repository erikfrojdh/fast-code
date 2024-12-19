#include <benchmark/benchmark.h>
#include <random>
#include <vector>


static void BM_Sorted(benchmark::State& state) {
  // Perform setup here
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0,100); // distribution in range [1, 6]



  size_t len = 100000;
  std::vector<int> vec(len);
  for (size_t i = 0; i!=len; ++i){
    vec[i] = dist(rng);
  }

  std::sort(vec.begin(), vec.end());

  for (auto _ : state) {
    // This code gets timed
    int sum=0;
    for (size_t i = 0; i!=len; ++i)
      if (vec[i]<50)
        sum += vec[i];

    benchmark::DoNotOptimize(sum);
  }
    
}
static void BM_UnSorted(benchmark::State& state) {
  // Perform setup here
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0,100); // distribution in range [1, 6]



  size_t len = 100000;
  std::vector<int> vec(len);
  for (size_t i = 0; i!=len; ++i){
    vec[i] = dist(rng);
  }



  for (auto _ : state) {
    // This code gets timed
    int sum=0;
    for (size_t i = 0; i!=len; ++i)
      if (vec[i]<50)
        sum += vec[i];

    benchmark::DoNotOptimize(sum);
  }
    
}
// Register the function as a benchmark
BENCHMARK(BM_Sorted);
BENCHMARK(BM_UnSorted);
// Run the benchmark
BENCHMARK_MAIN();