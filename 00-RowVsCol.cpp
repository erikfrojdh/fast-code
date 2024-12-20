#include <chrono>
#include <fmt/core.h>

#include "aare/NDArray.hpp"
#include <hwy/highway.h>

#include "helper.hpp"
namespace hn = hwy::HWY_NAMESPACE;

namespace project {
void CallAddLoop(int *HWY_RESTRICT arr, const size_t size);
}

// General setup
using data_type = int;
using std::chrono::microseconds;
constexpr int n_rows = 1024 * 8;
constexpr int n_cols = 1024 * 8;
double frame_size_GB = n_rows * n_cols * sizeof(data_type) / 1e9;

auto increment_with_operator(aare::NDArray<data_type, 2> &data, bool print = true) {
    auto start = std::chrono::high_resolution_clock::now();
    data += data_type{1};
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<microseconds>(stop - start);
    std::chrono::duration<double> t = stop - start;
    if(print)
        fmt::print("Using operator {} microseconds {:.1f} GB/s\n", duration.count(),
                frame_size_GB / t.count());
    return t;
}

auto col_first(aare::NDArray<data_type, 2> &data) {
    auto start = std::chrono::high_resolution_clock::now();

    // Loop over the array with the column as the inner loop
    for (int row = 0; row < data.shape(0); row++) {
        for (int col = 0; col < data.shape(1); col++) {
            data(row, col) += data_type{1};
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<microseconds>(stop - start);
    auto t = std::chrono::duration<double>(stop - start).count();
    fmt::print("Col first: {} microseconds {:.1f} GB/s\n", duration.count(),
               frame_size_GB / t);
    return t;
}

auto row_first(aare::NDArray<data_type, 2> &data) {
    auto start = std::chrono::high_resolution_clock::now();


    for (int col = 0; col < data.shape(1); col++) {
        for (int row = 0; row < data.shape(0); row++) {
            data(row, col) += data_type{1};
        }
    }
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<microseconds>(stop - start);
    auto t = std::chrono::duration<double>(stop - start).count();
    fmt::print("Row first: {} microseconds {:.1f} GB/s\n", duration.count(),
               frame_size_GB / t);
    return t;
}

int main() {

    aare::NDArray<data_type, 2> data({n_rows, n_cols}, data_type{0});

    
    // touch the data a few times to warm up 
    for (size_t i = 0; i < 3; i++)
    {
        increment_with_operator(data, false);
    }
    
        
    size_t n = 5;
    std::vector<double> cf;
    std::vector<double> rf;
    
    for (size_t i = 0; i < n; i++) {
        cf.push_back(col_first(data));
        rf.push_back(row_first(data));
    }

    auto [m,s] = mean_std(cf);
    fmt::print("\n\nCol first: mean: {:.3f}s std: {:.4f}s\n", m, s);
    auto [m2,s2] = mean_std(rf);
    fmt::print("Row first: mean: {:.3f}s std: {:.4f}s\n", m2, s2);
    fmt::print("Speedup: {:.2f}x\n", m2/m);


    return 0;
}