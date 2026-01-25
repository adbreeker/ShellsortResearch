#include <iostream>
#include <vector>
#include <fstream>
#include "Components/SearchingAlgorithms/GeneticAlgorithm.hpp"
#include "Components/SearchingAlgorithms/CuckooSearch.hpp"
#include "Components/SearchingAlgorithms/ArtificialBeeColony.hpp"
#include "Components/Shellsort.hpp"
#include "Components/ShellsortComparisions.hpp"
#include "Components/FilesManagement.hpp"
#include "omp.h"

const unsigned long SORTING_RANGE = 2500;

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
    
    #pragma omp parallel sections num_threads(3)
    {
        #pragma omp section
        {
            search_genetic::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
        }
        #pragma omp section
        {
            search_cuckoo::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
        }
        #pragma omp section
        {
            search_abc::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
        }
    }
    //search_genetic::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //search_cuckoo::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //search_abc::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);

    //unreachable while endless seeking
    for (GapsSequence& gs : files::getGapsFromFile("BestGapsSequences" + std::to_string(SORTING_RANGE) + "_cuckoo.txt")) gapsSequences.push_back(gs);
    auto results = compareShellSorts(SORTING_RANGE, gapsSequences, 100);
    PrintResults(results);
}