﻿#include <iostream>
#include <vector>
#include "GeneticAlgorithm.hpp"
#include "Shellsort.hpp"
#include "ShellsortComparisions.hpp"

const unsigned long SORTING_RANGE = 1000;

void PrintResults(std::vector<Result>& results)
{
    std::cout << "\n\nResults:\n";
    for (Result& r : results)
    {
        r.gapsSequence.PrintInstance();
        std::cout << "\n  Time: " << r.time << "ms | Operations: " << r.operations << " | Wins: " << r.wins << "\n\n";
    }
}

int main() 
{
    std::vector<GapsSequence> gapsSequences = 
    { 
        getTokudaGaps(SORTING_RANGE),
        getCiuraGaps(SORTING_RANGE),
        getLeeGaps(SORTING_RANGE),
        getSkeanEhrenborgJaromczykGaps(SORTING_RANGE),
        GapsSequence("Compare1", {989, 269, 118, 41, 20, 9, 4, 1 }),
        GapsSequence("Compare2", {895, 283, 91, 29, 14, 4, 1}),
    };


    //for (int i = gapsSequences.size(); i<100; i++) gapsSequences.push_back(GapsSequence("Random" + std::to_string(i), getRandomizedGaps(SORTING_RANGE)));
    //genetic::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);

    auto results = compareShellSorts(SORTING_RANGE, gapsSequences, 1);
    PrintResults(results);
}