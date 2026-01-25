#ifndef SHELLSORT_COMPARISIONS_HPP
#define SHELLSORT_COMPARISIONS_HPP


#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <omp.h>
#include "Shellsort.hpp"
#include "Utilis.hpp"

struct Result
{
    double time = 0.0;
    double operations = 0;
    GapsSequence gapsSequence;
    int wins = 0;

    double getFitnessScore()
    {
        return operations;
    }
};

Result measureShellSort(std::vector<int> data, GapsSequence gapsSequence)
{
    unsigned long operations;

    auto start = std::chrono::high_resolution_clock::now();
    operations = shellSort(data, gapsSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    return Result{ elapsed.count(), (double)operations, gapsSequence };
}

std::vector<Result> compareShellSorts(unsigned long sortingRange, std::vector<GapsSequence> gapsSequences, int iterations, bool debugs = false)
{
    int sortsCount = gapsSequences.size();
    std::vector<Result> avgResults(sortsCount);

    if (debugs) { std::cout << "Compare iterations:"; }

    for (int i = 0; i < iterations; i++)
    {
        if (debugs)
        {
            if (i % 50 == 0) std::cout << "\n";
            std::cout << "+";
        }

        std::vector<int> data = utilis::getRandomSortingData(sortingRange);

        // Use OpenMP for parallel execution
        std::vector<Result> results(sortsCount);

        #pragma omp parallel for
        for (int j = 0; j < sortsCount; j++)
        {
            results[j] = measureShellSort(data, gapsSequences[j]);
        }

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

bool isGapsSequenceIn(const GapsSequence& sequence, const std::vector<GapsSequence>& listOfSequences)
{
    for (const GapsSequence& gs : listOfSequences)
    {
        if (gs == sequence)
        {
            return true;
        }
    }
    return false;
}


#endif // !SHELLSORT_COMPARISIONS_HPP
