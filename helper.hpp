#include <array>
#include <vector>

template<typename T>
std::array<double, 2> mean_std(const std::vector<T> &data) {
    double sum = 0;
    double sum2 = 0;
    for (const auto &val : data) {
        sum += val;
        sum2 += val * val;
    }
    double mean = sum / data.size();
    double variance = sum2 / data.size() - mean * mean;
    return {mean, std::sqrt(variance)};
}