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
    double comparisons = 0;
    double loops = 0;
    double operations = 0;
    GapSequence gapSequence;
    int wins = 0;

    double GetFitnessScore() const
    {
        return operations;
    }
};

double MeasureShellSort_Time(std::vector<int> data, GapSequence gapSequence)
{
    auto start = std::chrono::high_resolution_clock::now();
    ShellSort(data, gapSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    return elapsed.count();
}

Result MeasureShellSort_Full(std::vector<int> data, GapSequence gapSequence)
{
    //comparisons, loops, operations
    std::tuple<unsigned long, unsigned long, unsigned long> stats;

    auto start = std::chrono::high_resolution_clock::now();
    stats = ShellSort_Stats(data, gapSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    return Result{ elapsed.count(), (double)std::get<0>(stats), (double)std::get<1>(stats), (double)std::get<2>(stats), gapSequence };
}

std::vector<Result> CompareShellSorts(unsigned long sortingRange, std::vector<GapSequence> gapSequences, int iterations, bool debugs = false)
{
    int sortsCount = gapSequences.size();
    std::vector<Result> avgResults(sortsCount);

    if (debugs) { std::cout << " - Compare iterations:"; }

    for (int i = 0; i < iterations; i++)
    {
        if (debugs)
        {
            if (i % 50 == 0) std::cout << "\n";
            std::cout << "+";
        }

        std::vector<int> data = utilis::GetRandomSortingData(sortingRange);

        // Use OpenMP for parallel execution
        std::vector<Result> results(sortsCount);

        #pragma omp parallel for
        for (int j = 0; j < sortsCount; j++)
        {
            results[j] = MeasureShellSort_Full(data, gapSequences[j]);
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
                avgResults[j].comparisons += results[j].comparisons;
                avgResults[j].loops += results[j].loops;
                avgResults[j].operations += results[j].operations;
            }
        }

        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.GetFitnessScore() < b.GetFitnessScore();
            });

        for (Result& r : avgResults) if (r.gapSequence == results[0].gapSequence) { r.wins++; }
    }

    for (Result& r : avgResults)
    {
        r.time = r.time / iterations;
        r.comparisons = r.comparisons / iterations;
        r.loops = r.loops / iterations;
        r.operations = r.operations / iterations;
    }

    // Sort results to find the fastest
    std::sort(avgResults.begin(), avgResults.end(), [](const Result& a, const Result& b) {
        return a.GetFitnessScore() < b.GetFitnessScore();
        });

    return avgResults;
}

bool IsGapSequenceIn(const GapSequence& sequence, const std::vector<GapSequence>& listOfSequences)
{
    for (const GapSequence& gs : listOfSequences)
    {
        if (gs == sequence)
        {
            return true;
        }
    }
    return false;
}


#endif // !SHELLSORT_COMPARISIONS_HPP
