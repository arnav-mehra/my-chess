#pragma once

#include <string>
#include <functional>
#include <iostream>

void success(std::string name) {
    std::cout << "\033[1;32m" << name << "\033[0m" << "\n";
}

void failure(std::string name) {
    std::cout << "\033[1;31m" << name << "\033[0m" << "\n";
}

void assert(std::string name, bool condition) {
    if (!condition) {
        failure("ASSERT FAILED: " + name + ".");
        std::abort();
    }
}

void assert(std::function<void(void)> fn, bool condition) {
    if (!condition) {
        failure("ASSERT FAILED.");
        fn();
        std::abort();
    }
}