#include <iostream>
#include <vector>
#include <numbers>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <functional>
#include <random>

void run(std::vector<std::vector<std::vector<double>>> data, std::function<double(std::vector<std::vector<std::vector<double>>>&)> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    double result = runFunction(data);
    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Sum: " << result << std::endl;

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() << "µs\n";
    std::cout << microseconds.count() / 1000 << "ms\n";
    std::cout << microseconds.count() / 1000000 << "s\n";
}

std::vector<std::vector<std::vector<double>>> generateData(const int matrixSize) {
    std::vector<std::vector<std::vector<double>>> data(matrixSize,
        std::vector<std::vector<double>>(matrixSize,
            std::vector<double>(matrixSize)));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < matrixSize; i++) {
        for (size_t j = 0; j < matrixSize; j++) {
            for (size_t k = 0; k < matrixSize; k++) {
                data[i][j][k] = dis(gen);
            }
        }
    }

    return data;
}

double nonContiguous3dSum(std::vector<std::vector<std::vector<double>>>& data) {
    double result = 0;

    for (size_t k = 0; k < data[0][0].size(); k++) {
        for (size_t j = 0; j < data[0].size(); j++) {
            for (size_t i = 0; i < data.size(); i++) {
                result += data[i][j][k];
            }
        }
    }

    return result;
}

double partiallyContiguous3dSum(std::vector<std::vector<std::vector<double>>>& data) {
    double result = 0;

    for (size_t k = 0; k < data[0][0].size(); k++) {
        for (size_t i = 0; i < data.size(); i++) {
            for (size_t j = 0; j < data[0].size(); j++) {
                result += data[i][j][k];
            }
        }
    }

    return result;
}

double contiguous3dSum(std::vector<std::vector<std::vector<double>>>& data) {
    double result = 0;

    for (size_t i = 0; i < data.size(); i++) {
        for (size_t j = 0; j < data[0].size(); j++) {
            for (size_t k = 0; k < data[0][0].size(); k++) {
                result += data[i][j][k];
            }
        }
    }

    return result;
}

int main() {
    std::cout << "Data Placement for Performance!\n";

    auto data = generateData(500);

    std::cout << "\n\Non-Contiguous 3D Sum:\n";
    run(data, nonContiguous3dSum);

    std::cout << "\n\Partially Contiguous 3D Sum:\n";
    run(data, partiallyContiguous3dSum);

    std::cout << "\n\Contiguous 3D Sum:\n";
    run(data, contiguous3dSum);
}