#include <iostream>
#include <vector>
#include <fstream>
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v1.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v2.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v3.hpp"
#include "Components/SearchingAlgorithms/CuckooSearch.hpp"
#include "Components/SearchingAlgorithms/ArtificialBeeColony.hpp"
#include "Components/Shellsort.hpp"
#include "Components/ShellsortComparisions.hpp"
#include "Components/FilesManagement.hpp"
#include "omp.h"

const unsigned long SORTING_RANGE = 100000; //do with 7500, 4999, 7499, 100000

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
        GetTokudaGaps(SORTING_RANGE),
        GetCiuraGaps(SORTING_RANGE),
        GetLeeGaps(SORTING_RANGE),
        GetSkeanEhrenborgJaromczykGaps(SORTING_RANGE)
    };


    for (int i = gapsSequences.size(); i<100; i++) gapsSequences.push_back(GapsSequence("1|Random|" + std::to_string(i + 1), GetRandomizedGaps(SORTING_RANGE)));
    
    // #pragma omp parallel sections num_threads(3) firstprivate(gapsSequences)
    // {
    //     #pragma omp section
    //     {
    //         search_genetic_v2::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_cuckoo::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_abc::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //     }
    // }

    //search_genetic_v2::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //search_cuckoo::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);
    //search_abc::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);

    search_genetic_v3::EndlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);

    //unreachable while endless seeking
    for (GapsSequence& gs : files::GetGapsFromFile("CandidateGapsSequences" + std::to_string(SORTING_RANGE) + "_GAv3.txt")) gapsSequences.push_back(gs);
    auto results = CompareShellSorts(SORTING_RANGE, gapsSequences, 100);
    PrintResults(results);
}