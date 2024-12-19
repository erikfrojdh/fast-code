#include <chrono>
#include <fmt/core.h>

#include "aare/NDArray.hpp"

#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

namespace project{
    void CallAddLoop(int* HWY_RESTRICT arr,
                const size_t size);
}

int main() {
    using std::chrono::microseconds;
    // using std::chrono::literals;
    constexpr int n_rows = 1024*16;
    constexpr int n_cols = 1024*16;
    using data_type = int;
    double frame_size_GB = n_rows * n_cols * sizeof(data_type) / 1e9;

    aare::NDArray<data_type, 2> data({n_rows, n_cols}, data_type{0});

    // Measure time to iterate over all elements in the
    // array incrementing them by 1.
    // Warmup by running the loop N times
    // int64_t sum = 0;
     for(int i = 0; i < 30; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        // data += data_type{1};
        auto sum = std::accumulate(data.begin(), data.end(), data_type{0});
        
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        std::chrono::duration<double> t = stop - start;
        fmt::print("Sum: {}\n", sum);
        fmt::print("Using operator {} microseconds {:.1f} GB/s\n", duration.count(), frame_size_GB/t.count());
    }
    for(int i = 0; i < 3; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int row = 0; row < data.shape(0); row++) {
            for (int col = 0; col < data.shape(1); col++) {
                data(row, col) += data_type{1};
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        std::chrono::duration<double> t = stop - start;
        fmt::print("Col first: {} microseconds {:.1f} GB/s\n", duration.count(), frame_size_GB/t.count());
    }
    for(int i = 0; i < 3; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int col = 0; col < data.shape(1); col++) {
            for (int row = 0; row < data.shape(0); row++) {
                data(row, col) += data_type{1};
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        std::chrono::duration<double> t = stop - start;
        fmt::print("Row first: {} microseconds {:.1f} GB/s\n", duration.count(), frame_size_GB/t.count());
    }
    for(int i = 0; i < 3; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        memset(data.data(), 0, data.size() * sizeof(data_type));
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        std::chrono::duration<double> t = stop - start;
        fmt::print("Memset: {} microseconds {:.1f} GB/s\n", duration.count(), frame_size_GB/t.count());
    }
    for(int i = 0; i < 10; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();

        //TODO! deal with remaining elements
        project::CallAddLoop(data.data(), data.size());

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);

        std::chrono::duration<double> t = stop - start;
        fmt::print("Simd: {} microseconds {:.1f} GB/s\n", duration.count(), frame_size_GB/t.count());
    }
   

    return 0;
}