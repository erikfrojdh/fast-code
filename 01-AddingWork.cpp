#include <chrono>
#include <fmt/core.h>

#include "aare/NDArray.hpp"
int main() {
    using std::chrono::microseconds;
    // using std::chrono::literals;
    constexpr int n_rows = 1024*4;
    constexpr int n_cols = 1024*4;
    using data_type = double;
    double frame_size_GB = n_rows * n_cols * sizeof(data_type) / 1e9;

    aare::NDArray<double, 2> A({n_rows, n_cols}, data_type{10});
    aare::NDArray<double, 2> B({n_rows, n_cols}, data_type{20});
    aare::NDArray<double, 2> C({n_rows, n_cols}, data_type{30});
    int N = 10;
    for(int i = 0; i < N; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        C = A + B;
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        fmt::print("A+B {} microseconds C(0,0) = {}\n", duration.count(),C(0,0));
    }
    
    for(int i = 0; i < N; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        C = A*B + B;
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        fmt::print("A*B+B {} microseconds C(0,0) = {}\n", duration.count(),C(0,0));
    }

    for(int i = 0; i < N; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        C = A*A + B*B;
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        fmt::print("A*A+B*B {} microseconds C(0,0) = {}\n", duration.count(),C(0,0));
    
    }

    for(int i = 0; i < N; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        C = A*A + B*B + A*B;
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<microseconds>(stop - start);
        fmt::print("A*A + B*B + A*B {} microseconds C(0,0) = {}\n", duration.count(),C(0,0));
    
    }

}