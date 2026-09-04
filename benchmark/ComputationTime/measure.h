#pragma once

#include <chrono>
#include <vector>
#include <numeric>

template <typename Func> double measure(Func func, int iterations = 50) {
    
    // ウォームアップ
    func();

    std::vector<double> times;
    times.reserve(iterations);

    for (int i = 0; i < iterations; i++) {

        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());

    }

    // 平均値算出
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / (double)iterations;

}