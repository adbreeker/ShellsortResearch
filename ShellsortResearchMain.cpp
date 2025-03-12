#include <iostream>
#include <vector>
#include <fstream>
#include "GeneticAlgorithm.hpp"
#include "Shellsort.hpp"
#include "ShellsortComparisions.hpp"
#include "FilesManagement.hpp"

const unsigned long SORTING_RANGE = 5000;

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
        getSkeanEhrenborgJaromczykGaps(SORTING_RANGE)
    };


    for (int i = gapsSequences.size(); i<100; i++) gapsSequences.push_back(GapsSequence("Random" + std::to_string(i + 1), getRandomizedGaps(SORTING_RANGE)));
    genetic::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);

    //for (GapsSequence& gs : files::getGapsFromFile(SORTING_RANGE)) gapsSequences.push_back(gs);

    auto results = compareShellSorts(SORTING_RANGE, gapsSequences, 100);
    PrintResults(results);
}