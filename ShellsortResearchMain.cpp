#include <iostream>
#include <vector>
#include "GeneticAlgorithm.hpp"
#include "Shellsort.hpp"



unsigned long SORTING_RANGE = 1000;

int main() 
{
    std::vector<Result> results;

    GapsSequence tokudaGaps = getTokudaGaps(SORTING_RANGE);
    GapsSequence ciuraGaps = getCiuraGaps(SORTING_RANGE);
    GapsSequence leeGaps = getLeeGaps(SORTING_RANGE);
    GapsSequence sejGaps = getSkeanEhrenborgJaromczykGaps(SORTING_RANGE);

    std::vector<GapsSequence> randomizedGapsSequences = { tokudaGaps, ciuraGaps, leeGaps, sejGaps};
    for (int i = randomizedGapsSequences.size(); i<100; i++) randomizedGapsSequences.push_back(GapsSequence("Random" + std::to_string(i), getRandomizedGaps(SORTING_RANGE)));

    genetic::endlessGapsSeeking(SORTING_RANGE, randomizedGapsSequences, 100);

    /*results = geneticAlgorithmForGapsSeeking(SORTING_RANGE, randomizedGapsSequences, 50, 100);

    std::cout << "\n\nResults:\n";
    for (Result r : results)
    {
        r.gapsSequence.PrintInstance();
        std::cout << "\n  Time: " << r.time << "ms | Operations: " << r.operations << " | Wins: " << r.wins << "\n\n";
    }*/
}