#ifndef SHELLSORT_COMPARISONS_HPP
#define SHELLSORT_COMPARISONS_HPP


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
        return comparisons;
    }
};

double MeasureShellsort_Time(std::vector<int> data, GapSequence gapSequence)
{
    auto start = std::chrono::high_resolution_clock::now();
    Shellsort(data, gapSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    return elapsed.count();
}

Result MeasureShellsort_Full(std::vector<int> data, GapSequence gapSequence)
{
    //comparisons, loops, operations
    std::tuple<unsigned long, unsigned long, unsigned long> stats;

    auto start = std::chrono::high_resolution_clock::now();
    stats = Shellsort_Stats(data, gapSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    return Result{ elapsed.count(), (double)std::get<0>(stats), (double)std::get<1>(stats), (double)std::get<2>(stats), gapSequence };
}

std::vector<Result> CompareShellsorts(unsigned long sortingRange, std::vector<GapSequence> gapSequences, int iterations)
{
    int sortsCount = gapSequences.size();
    std::vector<Result> avgResults(sortsCount);

    for (int i = 0; i < iterations; i++)
    {
        // Get random data for sorting
        std::vector<int> data = utilis::GetRandomSortingData(sortingRange);

        std::vector<Result> results(sortsCount);
        // Use OpenMP for parallel execution
        #pragma omp parallel for
        for (int j = 0; j < sortsCount; j++)
        {
            results[j] = MeasureShellsort_Full(data, gapSequences[j]);
        }

        // Accumulate results for averaging
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

        // Getting best result for wins count
        auto winner_it = std::min_element(results.begin(), results.end(), 
            [](const Result& a, const Result& b) {
                return a.GetFitnessScore() < b.GetFitnessScore();
            }
        );
        Result winner = *winner_it;

        for (Result& r : avgResults) if (r.gapSequence == winner.gapSequence) { r.wins++; }
    }

    // Average the results over the number of iterations
    for (Result& r : avgResults)
    {
        r.time = r.time / iterations;
        r.comparisons = r.comparisons / iterations;
        r.loops = r.loops / iterations;
        r.operations = r.operations / iterations;
    }

    // Sort results return order by fitness score
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


#endif // !SHELLSORT_COMPARISONS_HPP
