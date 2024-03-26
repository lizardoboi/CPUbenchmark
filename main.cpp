#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <numeric>
#include <cstdint>
#include <random>
#include <cmath>

template<class T>
std::vector<std::vector<T>> multiplyMatrix(const std::vector<std::vector<T>>& matrix1, const std::vector<std::vector<T>>& matrix2) {
    auto rows1 = matrix1.size();
    auto cols1 = matrix1[0].size();
    auto cols2 = matrix2[0].size();

    std::vector<std::vector<T>> result(rows1, std::vector<T>(cols2, 0));

    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            for (int k = 0; k < cols1; ++k) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

template<class T>
std::vector<std::vector<T>> generateRandomMatrix(const int rows, const int cols) {
    std::default_random_engine random(time(nullptr));
    std::uniform_int_distribution<int> uniform_dist(0, 1000);
    std::vector<std::vector<T>> matrix(rows, std::vector<T>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = uniform_dist(random);
        }
    }
    return matrix;
}

using namespace std;

template<class Collection>
void print(const Collection& collection) {
    for (auto el : collection) {
        cout << el << " + ";
    }
    cout << endl;
}

template<class T>
void runTypicalTask(const std::string PModel, const std::string Task, const std::string OpType, const std::string Opt, const int InsCount, const std::string Timer, const int LNum) {
    std::vector<int64_t> task_times;
    task_times.reserve(InsCount);
    for (int i = 0; i < InsCount; ++i) {
        const int matrix_size = 100;
        auto matrix1 = generateRandomMatrix<T>(matrix_size, matrix_size);
        auto matrix2 = generateRandomMatrix<T>(matrix_size, matrix_size);

        auto start_task = std::chrono::high_resolution_clock::now();
        auto result = multiplyMatrix<T>(matrix1, matrix2);
        auto end_task = std::chrono::high_resolution_clock::now();

        auto duration_task = std::chrono::duration_cast<std::chrono::nanoseconds>(end_task - start_task);
        int64_t task_time = duration_task.count();
        task_times.push_back(task_time);
    }

    int64_t Time = std::accumulate(task_times.begin(), task_times.end(), 0); // Используйте std::accumulate()
    long double AvTime = ((long double)Time) / (long double)task_times.size(); // Используйте общее время
    long double AbsError = 0.0;
    for (const auto time : task_times) {
        AbsError += std::fabs(time - AvTime);
    }
    AbsError /= task_times.size();
    long double RelError = AbsError / AvTime * 100.0;
    auto TaskPerf = InsCount / (AvTime / (long double)10e9);

    // Рассчет дисперсии
    long double variance = 0.0;
    for (const auto time : task_times) {
        variance += std::pow(time - AvTime, 2);
    }
    variance /= task_times.size();

    // Рассчет стандартного отклонения
    long double standard_deviation = std::sqrt(variance);

    cout << PModel << ";" << Task << ";" << OpType << ";" << Opt << ";" << InsCount << ";" << Timer << ";" << Time << ";" << LNum << ";";
    printf("%.2Lf;%.2Lf;%.2Lf;%.2Lf\n",AbsError, RelError,AvTime,TaskPerf);
}
int main() {
    const int InsCount = 100; // Фиксированное количество операций

    cout << "PModel;Task;OpType;Opt;InsCount;Timer;Time;LNum;AvTime;AbsError;RelError;TaskPerf" << "\n";
    auto PModel = "AMD Ryzen 5 3500x";
    auto Task = "Matrix-Vector Multiplication";
    std::string OpType = "int";
    std::string Opt = "None";
    std::string Timer = "chrono";
    int LNum = 1;
    runTypicalTask<int>(PModel, Task, OpType, Opt, InsCount, Timer, LNum);
    ++LNum;
    OpType = "float";
    runTypicalTask<float>(PModel, Task, OpType, Opt, InsCount, Timer, LNum);
    ++LNum;
    OpType = "double";
    runTypicalTask<double>(PModel, Task, OpType, Opt, InsCount, Timer, LNum);
    return 0;
}