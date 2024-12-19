#include "aare/File.hpp"
#include "aare/NDArray.hpp"
#include "aare/RawFile.hpp"

#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <semaphore>
#include <thread>

double frame_size_GB = 400 * 400 * 2 / 1e9;

void read_compute(const std::filesystem::path &fname, ssize_t n_frames) {

    fmt::print("\nRead compute\n");
    auto t0 = std::chrono::high_resolution_clock::now();
    aare::NDArray<uint16_t, 3> data({n_frames, 400, 400});
    aare::File f(fname);
    auto start = std::chrono::high_resolution_clock::now();
    f.read_into(data.buffer(), n_frames);
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t = stop - start;
    fmt::print("Reading {} frames took {:.3f} seconds\n", n_frames, t.count());

    start = std::chrono::high_resolution_clock::now();
    aare::NDArray<uint64_t, 2> total({400, 400}, uint64_t{0});
    for (size_t frame = 0; frame < n_frames; frame++) {
        for (size_t row = 0; row < 400; row++) {
            for (size_t col = 0; col < 400; col++) {
                total(row, col) += data(frame, row, col);
            }
        }
    }
    aare::NDArray<float, 2> pd({400, 400});
    for (size_t row = 0; row < 400; row++) {
        for (size_t col = 0; col < 400; col++) {
            pd(row, col) = static_cast<float>(total(row, col)) / n_frames;
        }
    }

    stop = std::chrono::high_resolution_clock::now();
    t = stop - start;
    fmt::print("Calculating pd from {} frames took {:.3f} seconds\n", n_frames,
               t.count());
    fmt::print("Pixel (110,100): {}, {:.3f}\n", total(110, 100), pd(110, 100));
    auto tt = std::chrono::duration<double>(stop - t0).count();
    fmt::print("Total time: {:.3f} seconds. Processed: {:.2f} GB/s\n", tt,
               frame_size_GB * n_frames / tt);
}

void integrated(const std::filesystem::path &fname, ssize_t n_frames) {
    fmt::print("\nIntegrated approach\n");
    auto t0 = std::chrono::high_resolution_clock::now();
    aare::RawFile f(fname);

    auto start = std::chrono::high_resolution_clock::now();

    aare::NDArray<uint64_t, 2> total({400, 400}, uint64_t{0});
    aare::NDArray<uint16_t, 2> tmp({400, 400});
    for (size_t frame = 0; frame < n_frames; frame++) {
        f.read_into(tmp.buffer());
        for (size_t row = 0; row < 400; row++) {
            for (size_t col = 0; col < 400; col++) {
                total(row, col) += tmp(row, col);
            }
        }
    }
    aare::NDArray<float, 2> pd({400, 400});
    for (size_t row = 0; row < 400; row++) {
        for (size_t col = 0; col < 400; col++) {
            pd(row, col) = static_cast<float>(total(row, col)) / n_frames;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto t = stop - start;
    fmt::print("Pixel (110,100): {}, {:.3f}\n", total(110, 100), pd(110, 100));
    auto tt = std::chrono::duration<double>(stop - t0).count();
    fmt::print("Total time: {:.3f} seconds. Processed: {:.2f} GB/s\n", tt,
               frame_size_GB * n_frames / tt);
}

void reordered(const std::filesystem::path &fname, ssize_t n_frames) {

    fmt::print("\nRead reorder\n");
    auto t0 = std::chrono::high_resolution_clock::now();
    aare::NDArray<uint16_t, 3> data({400, 400, n_frames});
    aare::NDArray<uint16_t, 2> tmp({400, 400});
    aare::RawFile f(fname);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t frame = 0; frame < n_frames; frame++) {
        f.read_into(tmp.buffer());
        for (size_t row = 0; row < 400; row++) {
            for (size_t col = 0; col < 400; col++) {
                data(row, col, frame) = tmp(row, col);
            }
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t = stop - start;
    fmt::print("Reading {} frames took {:.3f} seconds\n", n_frames, t.count());

    start = std::chrono::high_resolution_clock::now();
    aare::NDArray<uint64_t, 2> total({400, 400}, uint64_t{0});

    for (size_t row = 0; row < 400; row++) {
        for (size_t col = 0; col < 400; col++) {
            for (size_t frame = 0; frame < n_frames; frame++) {
                total(row, col) += data(row, col, frame);
            }
        }
    }
    aare::NDArray<float, 2> pd({400, 400});
    for (size_t row = 0; row < 400; row++) {
        for (size_t col = 0; col < 400; col++) {
            pd(row, col) = static_cast<float>(total(row, col)) / n_frames;
        }
    }

    stop = std::chrono::high_resolution_clock::now();
    t = stop - start;
    fmt::print("Calculating pd from {} frames took {:.3f} seconds\n", n_frames,
               t.count());
    fmt::print("Pixel (110,100): {}, {:.3f}\n", total(110, 100), pd(110, 100));
    auto tt = std::chrono::duration<double>(stop - t0).count();
    fmt::print("Total time: {:.3f} seconds. Processed: {:.2f} GB/s\n", tt,
               frame_size_GB * n_frames / tt);
}

aare::NDArray<uint16_t, 2> tmp0({400, 400});
aare::NDArray<uint16_t, 2> tmp1({400, 400});
std::binary_semaphore smphSignalMainToThread{0}, smphSignalThreadToMain{0};

void reader(const std::filesystem::path &fname, ssize_t n_frames) {
    // fmt::print("Reader\n");
    aare::RawFile f(fname);

    for (size_t frame = 0; frame < n_frames; frame++) {
        smphSignalMainToThread.acquire();
        f.read_into(tmp0.buffer());
        // fmt::print("Read frame: {}\n", frame);
        smphSignalThreadToMain.release();
    }
}

auto two_threads(const std::filesystem::path &fname, ssize_t n_frames) {
    fmt::print("\nRead with two threads using semaphores\n");
    auto t0 = std::chrono::high_resolution_clock::now();
    std::thread t1(reader, fname, n_frames);
    size_t i = 0;
    aare::NDArray<uint64_t, 2> total({400, 400}, uint64_t{0});
    smphSignalMainToThread.release(); // t1 reads the first frame
    while (i < n_frames) {
        smphSignalThreadToMain.acquire(); // wait for t1 to read the frame
        std::copy(tmp0.begin(), tmp0.end(), tmp1.begin());
        // fmt::print("Processing frame: {}\n", i);
        smphSignalMainToThread
            .release(); // now t1 is free to do read the next frame
        // now t1 is free to do read the next frame
        for (size_t row = 0; row < 400; row++) {
            for (size_t col = 0; col < 400; col++) {
                total(row, col) += tmp1(row, col);
            }
        }
        i++;
    }
    t1.join();
    aare::NDArray<float, 2> pd({400, 400});
    for (size_t row = 0; row < 400; row++) {
        for (size_t col = 0; col < 400; col++) {
            pd(row, col) = static_cast<float>(total(row, col)) / n_frames;
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    fmt::print("Pixel (110,100): {}, {:.3f}\n", total(110, 100), pd(110, 100));
    auto tt = std::chrono::duration<double>(stop - t0).count();
    fmt::print("Total time: {:.3f} seconds. Processed: {:.2f} GB/s\n", tt,
               frame_size_GB * n_frames / tt);
    return tt;
}

auto only_read(const std::filesystem::path &fname, ssize_t n_frames) {
    fmt::print("\nRead the data into the same buffer\n");
    auto t0 = std::chrono::high_resolution_clock::now();
    std::ifstream f(fname, std::ios::binary);
    constexpr size_t frame_size = 400 * 400 * 2+112;
    char* buf = new char[frame_size];

    for (size_t frame = 0; frame < n_frames; frame++) {
        f.read(buf, frame_size);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto tt = std::chrono::duration<double>(stop - t0).count();
    fmt::print("Total time: {:.3f} seconds. Processed: {:.2f} GB/s\n", tt,
               frame_size_GB * n_frames / tt);
    delete[] buf;
    return tt;
}

int main() {

    constexpr ssize_t n_frames = 10000;
    std::filesystem::path fname =
        "/Users/erik/data/Moench03new/cu_half_speed_master_4.json";
    
    //Used for benchmarking read speed
    std::filesystem::path raw =
        "/Users/erik/data/Moench03new/cu_half_speed_d0_f0_4.raw";

    read_compute(fname, n_frames);
    integrated(fname, n_frames);
    // reordered(fname, n_frames);
    // two_threads(fname, n_frames);
    // only_read(raw, n_frames);


    // std::vector<double> v;
    // std::vector<double> v2;
    // for(size_t i = 0; i < 3; i++)
    // {
    //     v.push_back(only_read(raw, n_frames));
    //     v2.push_back(two_threads(fname, n_frames));
        
    // }
    // auto mean = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    // auto sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    // auto stdev = std::sqrt(sq_sum / v.size() - mean * mean);
    // fmt::print("Reading data: {:.3f}s std: {:.3f}s\n", mean, stdev);

    // auto mean2 = std::accumulate(v2.begin(), v2.end(), 0.0) / v2.size();
    // auto sq_sum2 = std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0);
    // auto stdev2 = std::sqrt(sq_sum / v2.size() - mean * mean);
    // fmt::print("Read and calculate: {:.3f}s std: {:.3f}s\n", mean2, stdev2);
    // fmt::print("Difference: {:.3f}s, relative: {:.3f}\n", mean2 - mean, mean2 / mean);

    
    // reordered(fname, n_frames);
}