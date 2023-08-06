#pragma once

#include <functional>
#include <chrono>
#include <iostream>

double time_fn(std::function<void(void)> fn) {
    auto start = std::chrono::system_clock::now();
    fn();
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> t_sec = end - start;
    double sec = t_sec.count();
    std::cout << "TIME: " << sec << " s\n";

    return sec;
}