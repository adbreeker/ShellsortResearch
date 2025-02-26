#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

struct Result
{
    double time;
    std::string name;
};

std::vector<Result> results;

void shellSort(std::vector<int>& arr, std::vector<int>& gaps)
{
    for(int gap : gaps)
    {
        for (int i = gap; i < arr.size(); i++)
        {
            int temp = arr[i];
            int j;
            for (j = i; (j >= gap) && (arr[j - gap] > temp); j -= gap)
            {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

void measureSortTime(void(*sortFunc)(std::vector<int>&), std::vector<int> data, const std::string& name) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(data);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    results.push_back(Result{ elapsed.count(), name });
}

void measureShellSortTime(std::vector<int> data, std::vector<int> gaps) {
    auto start = std::chrono::high_resolution_clock::now();
    shellSort(data, gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    results.push_back(Result{ elapsed.count(), "Shell Sort" });
}

int main() {
    const int SIZE = 10000;
    
    for (int i = 0; i < 100; i++)
    {
        results = std::vector<Result>();
        std::vector<int> data(SIZE);
        for (int& num : data) num = rand() % 10000;

        std::vector<int> data1 = data;
        std::vector<int> data2 = data;

        std::vector<int> gaps = { 701, 301, 132, 57, 23, 10, 4, 1 };

        std::thread t1(measureSortTime, [](std::vector<int>& d) { std::sort(d.begin(), d.end()); }, data1, "std::sort");
        std::thread t2(measureShellSortTime, data2, gaps);

        t1.join();
        t2.join();

        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.time < b.time;
            });

        std::cout << "\n\n   Iteration " << i;
        if (results[0].name == "Shell Sort") { std::cout << " ---------------- SHELL WINNER !!! ------------------"; }
        std::cout << "\n";
        for (int j = 0; j < results.size(); j++)
        {
            std::cout << results[j].name << ": " << results[j].time << " ms\n";
        }
    }

    return 0;
}
