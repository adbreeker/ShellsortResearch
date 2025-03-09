#ifndef SHELLSORT_COMPARISIONS_HPP
#define SHELLSORT_COMPARISIONS_HPP


#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <thread>
#include <random>
#include "Shellsort.hpp"

struct Result
{
    double time = 0.0;
    double operations = 0;
    GapsSequence gapsSequence;
    int wins = 0;
};

std::vector<int> getRandomData(unsigned long sortingRange)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(-10000, 10000);

    std::vector<int> data(sortingRange);
    for (int& num : data) num = dist(gen);

    return data;
}

void measureShellSort(std::vector<int> data, GapsSequence gapsSequence, std::promise<Result> output)
{
    unsigned long operations;

    auto start = std::chrono::high_resolution_clock::now();
    operations = shellSort(data, gapsSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), (double)operations, gapsSequence });
}

std::vector<Result> compareShellSorts(unsigned long sortingRange, std::vector<GapsSequence> gapsSequences, int iterations)
{
    int sortsCount = gapsSequences.size();
    std::vector<Result> avgResults(sortsCount);

    std::cout << "Compare iterations:";

    for (int i = 0; i < iterations; i++)
    {
        if (i % 50 == 0) std::cout << "\n";
        std::cout << "+";

        std::vector<int> data = getRandomData(sortingRange);

        // Use promises & futures for safe thread return value
        std::vector<std::promise<Result>> promises(sortsCount);
        std::vector < std::future<Result>> futures(sortsCount);
        for (int j = 0; j < sortsCount; j++) futures[j] = promises[j].get_future();

        std::vector<std::thread> threads;

        int cpuCores = std::thread::hardware_concurrency();


        for (int j = 0; j < sortsCount; j++)
        {
            threads.push_back(std::thread(measureShellSort, data, gapsSequences[j], std::move(promises[j])));
            if (j % cpuCores == cpuCores - 1)
            {
                for (int l = j - (cpuCores - 1); l <= j; l++)
                {
                    threads[l].join();
                }
            }
        }
        for (int l = threads.size() - (sortsCount % cpuCores); l < threads.size(); l++) threads[l].join();

        // Get results from threads
        std::vector<Result> results;
        for (int j = 0; j < sortsCount; j++) results.push_back(futures[j].get());

        if (i == 0)
        {
            for (int j = 0; j < sortsCount; j++)
            {
                avgResults[j] = results[j];
            }
        }
        else
        {
            for (int j = 0; j < sortsCount; j++)
            {
                avgResults[j].time += results[j].time;
                avgResults[j].operations += results[j].operations;
            }
        }

        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.operations < b.operations;
            });

        for (Result& r : avgResults) if (r.gapsSequence == results[0].gapsSequence) { r.wins++; }
    }

    for (Result& r : avgResults)
    {
        r.time = r.time / iterations;
        r.operations = r.operations / iterations;
    }

    // Sort results to find the fastest
    std::sort(avgResults.begin(), avgResults.end(), [](const Result& a, const Result& b) {
        return a.operations < b.operations;
        });

    return avgResults;
}


#endif // !SHELLSORT_COMPARISIONS_HPP
