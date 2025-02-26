#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <math.h>
#include <future>

struct Result
{
    double time;
    std::string name;
    std::vector<int> gapsUsed;
};

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

void measureSortTime(void(*sortFunc)(std::vector<int>&), std::vector<int> data, std::string name, std::promise<Result> output) 
{
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(data);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), name });
}

void measureShellSortTime(std::vector<int> data, std::vector<int> gaps, std::string name, std::promise<Result> output) 
{
    auto start = std::chrono::high_resolution_clock::now();
    shellSort(data, gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), name, gaps });
}

std::vector<int> getTokudaGaps(int sortingRange)
{
    std::vector<int> tokudaGaps;

    for (int k = 1; k < sortingRange; k++)
    {
        int gap = (int)std::ceil((std::pow(9.0 / 4.0, k) - 1) / ((9.0 / 4.0) - 1));

        if (gap < sortingRange)
        {
            tokudaGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(tokudaGaps.begin(), tokudaGaps.end());
    return tokudaGaps;
}

std::vector<int> getCiuraGaps(int sortingRange)
{
    std::vector<int> ciuraGaps;
    for (int gap : {1, 4, 10, 23, 57, 132, 301, 701 /*later extension:*/, 1750})
    {
        if (gap < sortingRange)
        {
            ciuraGaps.push_back(gap);
        }
        else break;
    }

    std::reverse(ciuraGaps.begin(), ciuraGaps.end());
    return ciuraGaps;
}

std::vector<int> getLeeGaps(int sortingRange)
{
    std::vector<int> leeGaps;

    double lambda = 2.24360906142;

    for (int k = 1; k < sortingRange; k++)
    {
        int gap = (int)std::ceil((std::pow(lambda, k) - 1) / (lambda - 1));

        if (gap < sortingRange)
        {
            leeGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(leeGaps.begin(), leeGaps.end());
    return leeGaps;
}

std::vector<int> getSkeanEhrenborgJaromczykGaps(int sortingRange)
{
    std::vector<int> sejGaps;

    for (int k = -1; k < sortingRange; k++)
    {
        int gap = (int)std::floor(4.0816 * std::pow(8.5714, k / 2.2449));

        if (gap < sortingRange)
        {
            sejGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(sejGaps.begin(), sejGaps.end());
    return sejGaps;
}

int main() 
{
    const int SORTING_RANGE = 10000;

    std::vector<int> tokudaGaps = getTokudaGaps(SORTING_RANGE);
    std::vector<int> ciuraGaps = getCiuraGaps(SORTING_RANGE);
    std::vector<int> leeGaps = getLeeGaps(SORTING_RANGE);
    std::vector<int> sejGaps = getSkeanEhrenborgJaromczykGaps(SORTING_RANGE);

    int winners[4] = { 0, 0, 0, 0 };

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Iteration " << i << "\n";

        std::vector<Result> results;
        std::vector<int> data(SORTING_RANGE);
        for (int& num : data) num = rand() % 10000;

        // Use promises & futures for safe thread return values
        std::promise<Result> p1, p2, p3, p4;
        std::future<Result> f1 = p1.get_future();
        std::future<Result> f2 = p2.get_future();
        std::future<Result> f3 = p3.get_future();
        std::future<Result> f4 = p4.get_future();

        std::thread t1(measureShellSortTime, data, tokudaGaps, "Tokuda", std::move(p1));
        std::thread t2(measureShellSortTime, data, ciuraGaps, "Ciura", std::move(p2));
        std::thread t3(measureShellSortTime, data, leeGaps, "Lee", std::move(p3));
        std::thread t4(measureShellSortTime, data, sejGaps, "SEJ", std::move(p4));

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        // Get results from threads
        results.push_back(f1.get());
        results.push_back(f2.get());
        results.push_back(f3.get());
        results.push_back(f4.get());

        // Sort results to find the fastest
        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.time < b.time;
            });

        if (results[0].name == "Tokuda") { winners[0]++; }
        if (results[0].name == "Ciura") { winners[1]++; }
        if (results[0].name == "Lee") { winners[2]++; }
        if (results[0].name == "SEJ") { winners[3]++; }
    }

    std::cout << "Tokuda: " << winners[0] << "\n";
    std::cout << "Ciura: " << winners[1] << "\n";
    std::cout << "Lee: " << winners[2] << "\n";
    std::cout << "SEJ: " << winners[3] << "\n";

    return 0;
}