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

const unsigned long SORTING_RANGE = 7500; //do with 7500, 7499, 

void PrintResults(std::vector<Result>& results)
{
    std::cout << "\n\nResults:\n";
    for (Result& r : results)
    {
        r.gapSequence.PrintInstance();
        std::cout << "\n  Time: " << r.time << "ms | Operations: " << r.operations << " | Wins: " << r.wins << "\n\n";
    }
}


int main() 
{
    std::vector<GapSequence> gapSequences = 
    { 
        GetTokudaGaps(SORTING_RANGE),
        GetCiuraGaps(SORTING_RANGE),
        GetLeeGaps(SORTING_RANGE),
        GetSkeanEhrenborgJaromczykGaps(SORTING_RANGE)
    };


    for (int i = gapSequences.size(); i<100; i++) gapSequences.push_back(GapSequence("1|Random|" + std::to_string(i + 1), GetRandomizedGaps(SORTING_RANGE)));
    
    // #pragma omp parallel sections num_threads(3) firstprivate(gapSequences)
    // {
    //     #pragma omp section
    //     {
    //         search_genetic_v2::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_cuckoo::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_abc::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    // }

    //search_genetic_v2::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //search_cuckoo::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //search_abc::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

    search_genetic_v3::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

    //unreachable while endless seeking
    for (GapSequence& gs : files::GetGapsFromFile("CandidateGapSequences" + std::to_string(SORTING_RANGE) + "_GAv3.txt")) gapSequences.push_back(gs);
    auto results = CompareShellSorts(SORTING_RANGE, gapSequences, 100);
    PrintResults(results);
}