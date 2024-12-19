#include <chrono>
#include <fmt/core.h>

#include "aare/NDArray.hpp"

#include <hwy/highway.h>
namespace hn = hwy::HWY_NAMESPACE;

// using T = float;

// void AddLoop(const T* HWY_RESTRICT array,
//                 const T& value) {
//   const hn::ScalableTag<T> d;
//   for (size_t i = 0; i < size; i += hn::Lanes(d)) {
//     const auto mul = hn::Load(d, mul_array + i);
//     const auto add = hn::Load(d, add_array + i);
//     auto x = hn::Load(d, array + i);
//     x = hn::Add(mul, x, add);
//     hn::Store(x, d, x_array + i);
//   }
// }


int main() {
    using std::chrono::microseconds;
    // using std::chrono::literals;
    constexpr int n_rows = 1024*4;
    constexpr int n_cols = 1024*4;
    using data_type = float;
    double frame_size_GB = n_rows * n_cols * sizeof(data_type) / 1e9;

    aare::NDArray<data_type, 2> data({n_rows, n_cols}, data_type{0});

    // Measure time to iterate over all elements in the
    // array incrementing them by 1.
    // Warmup by running the loop N times
     for(int i = 0; i < 30; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        data += data_type{1};
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        std::chrono::duration<double> t = stop - start;
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
    for(int i = 0; i < 3; i++)
    {
        data = 0;
        auto start = std::chrono::high_resolution_clock::now();
        const hn::ScalableTag<data_type> d;
        fmt::print("Lanes: {}\n", hn::Lanes(d));
        auto ptr = data.begin();
        for (size_t i = 0; i < data.size(); i += hn::Lanes(d)) {
            auto x = hn::Load(d, ptr + i);
            x = hn::Add(x, hn::Set(d, 1.0));
            hn::Store(x, d, ptr + i);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        std::chrono::duration<double> t = stop - start;
        fmt::print("Simd: {} microseconds {:.1f} GB/s\n", duration.count(), frame_size_GB/t.count());
        fmt::print("First element: {}\n", data(0, 0));
    }
   

    return 0;
}