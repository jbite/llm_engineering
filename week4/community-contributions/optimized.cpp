#include <iostream>
#include <iomanip>
#include <chrono>

double calculate(long long iterations, int param1, int param2) {
    double result = 1.0;
    for (long long i = 1; i <= iterations; ++i) {
        long long j = i * param1 - param2;
        result -= (1.0 / j);
        j = i * param1 + param2;
        result += (1.0 / j);
    }
    return result;
}

int main() {
    long long iterations = 100000000;
    int param1 = 4;
    int param2 = 1;

    auto start_time = std::chrono::high_resolution_clock::now();
    double result = calculate(iterations, param1, param2) * 4.0;
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end_time - start_time;

    std::cout << std::fixed << std::setprecision(12) << "Result: " << result << std::endl;
    std::cout << std::fixed << std::setprecision(6) << "Execution Time: " << elapsed_seconds.count() << " seconds" << std::endl;

    return 0;
}

