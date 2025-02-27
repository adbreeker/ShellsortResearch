#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <math.h>
#include <future>
#include <random>
#include <string>

struct Result
{
    double time;
    std::string name;
    std::vector<long> gapsUsed;
};

void shellSort(std::vector<int>& arr, std::vector<long>& gaps)
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

void measureShellSortTime(std::vector<int> data, std::vector<long> gaps, std::string name, std::promise<Result> output) 
{
    auto start = std::chrono::high_resolution_clock::now();
    shellSort(data, gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), name, gaps });
}

std::vector<long> getTokudaGaps(long sortingRange)
{
    std::vector<long> tokudaGaps;

    for (long k = 1; k < sortingRange; k++)
    {
        long gap = (long)std::ceil((std::pow(9.0 / 4.0, k) - 1) / ((9.0 / 4.0) - 1));

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

std::vector<long> getCiuraGaps(long sortingRange)
{
    std::vector<long> ciuraGaps;
    for (long gap : {1, 4, 10, 23, 57, 132, 301, 701 /*later extension:*/, 1750})
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

std::vector<long> getLeeGaps(long sortingRange)
{
    std::vector<long> leeGaps;

    double lambda = 2.24360906142;

    for (long k = 1; k < sortingRange; k++)
    {
        long gap = (long)std::ceil((std::pow(lambda, k) - 1) / (lambda - 1));

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

std::vector<long> getSkeanEhrenborgJaromczykGaps(long sortingRange)
{
    std::vector<long> sejGaps;

    for (long k = -1; k < sortingRange; k++)
    {
        long gap = (long)std::floor(4.0816 * std::pow(8.5714, k / 2.2449));

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

std::vector<long> getRandomizedGaps(long sortingRange)
{
    std::vector<long> randomizedGaps = {1};

    std::random_device rd;
    std::mt19937 gen(rd());

    if (rand() % 2 == 0)
    {
        std::uniform_real_distribution<float> dist(1.0001f, 5.0f);

        for (long k = 1; k < sortingRange; k++)
        {

            float x = dist(gen);
            long gap = (long)std::ceil(randomizedGaps[k-1] * x);

            if (gap < sortingRange)
            {
                randomizedGaps.push_back(gap);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        std::uniform_real_distribution<float> dist(2.0f, 5.0f);

        for (long k = 1; k < sortingRange; k++)
        {

            float x = dist(gen);
            long gap = (long)std::floor(randomizedGaps[k - 1] * x);

            if (gap < sortingRange)
            {
                randomizedGaps.push_back(gap);
            }
            else
            {
                break;
            }
        }
    }

    std::reverse(randomizedGaps.begin(), randomizedGaps.end());
    return randomizedGaps;
}

int main() 
{
    const long SORTING_RANGE = 1000;

    std::vector<long> tokudaGaps = getTokudaGaps(SORTING_RANGE);
    std::vector<long> ciuraGaps = getCiuraGaps(SORTING_RANGE);
    std::vector<long> leeGaps = getLeeGaps(SORTING_RANGE);
    std::vector<long> sejGaps = getSkeanEhrenborgJaromczykGaps(SORTING_RANGE);

    std::vector<long> randomizedGaps[95];
    for (std::vector<long>& rg : randomizedGaps) rg = getRandomizedGaps(SORTING_RANGE);

    for (std::vector<long> rg : randomizedGaps)
    {
        for (long gap : rg)
        {
            std::cout << gap << "  ";
        }
        std::cout << "\n";
    }

    int winners[6] = {};
    
    std::cout << "\n\n";

    for (int i = 0; i < 1; i++)
    {
        std::cout << "Iteration " << i << "\n";

        std::vector<Result> results;
        std::vector<int> data(SORTING_RANGE);
        for (int& num : data) num = rand() % 10000;

        // Use promises & futures for safe thread return value
        std::promise<Result> promises[100];
        std::future<Result> futures[100];
        for (int j = 0; j < 100; j++) futures[j] = promises[j].get_future();

        std::thread threads[100];

        threads[0] = std::thread(measureShellSortTime, data, tokudaGaps, "Tokuda", std::move(promises[0]));
        threads[1] = std::thread(measureShellSortTime, data, ciuraGaps, "Ciura", std::move(promises[1]));
        threads[2] = std::thread(measureShellSortTime, data, leeGaps, "Lee", std::move(promises[2]));
        threads[3] = std::thread(measureShellSortTime, data, sejGaps, "SEJ", std::move(promises[3]));
        threads[4] = std::thread(measureSortTime, [](std::vector<int>& d) {std::sort(d.begin(), d.end());}, data, "stdSort", std::move(promises[4]));

        for (int j = 5; j<100; j++) threads[j] = std::thread(measureShellSortTime, data, randomizedGaps[j-5], "Random" + std::to_string(j-4), std::move(promises[j]));

        for (int j = 0; j < 100; j++) threads[j].join();

        // Get results from threads
        for (int j = 0; j < 100; j++) results.push_back(futures[j].get());

        // Sort results to find the fastest
        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.time < b.time;
            });

        for (Result r : results)
        {
            std::cout << r.name + ": " << r.time << "ms\n";
            if (r.name != "stdSort")
            {
                std::cout << "Gaps: ";
                for (long gap : r.gapsUsed) std::cout << gap << " ";
            }
            std::cout << "\n\n";
        }
    }

    return 0;
}