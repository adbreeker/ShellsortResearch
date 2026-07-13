#include <iostream>
#include <vector>
#include <fstream>
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v1.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v2.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v3.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v4.hpp"
#include "Components/SearchingAlgorithms/CuckooSearch.hpp"
#include "Components/SearchingAlgorithms/ArtificialBeeColony.hpp"
#include "Components/Shellsort.hpp"
#include "Components/ShellsortComparisions.hpp"
#include "Components/FilesManagement.hpp"
#include "omp.h"

const unsigned long SORTING_RANGE = 10000; 

void PrintResults(std::vector<Result>& results, int topN = 10)
{
    std::cout << "\n\nResults:\n";
    for (int i = 0; i < std::min(topN, static_cast<int>(results.size())); ++i)
    {
        auto& r = results[i];
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
    //         search_genetic_v1::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_genetic_v2::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_genetic_v3::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    // }

    // search_genetic_v2::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    // search_cuckoo::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    // search_abc::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

    search_genetic_v4::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

    // for (GapSequence& gs : files::GetGapsFromFile("CandidateGapSequences" + std::to_string(SORTING_RANGE) + "_GAv3.txt")) gapSequences.push_back(gs);
    // auto results = CompareShellSorts(SORTING_RANGE, gapSequences, 1000);
    // PrintResults(results, 10);
    // std::vector<GapSequence> finalGroup = 
    // { 
    //     GetTokudaGaps(SORTING_RANGE),
    //     GetCiuraGaps(SORTING_RANGE),
    //     GetLeeGaps(SORTING_RANGE),
    //     GetSkeanEhrenborgJaromczykGaps(SORTING_RANGE),
    //     results[0].gapSequence,
    //     results[1].gapSequence,
    //     results[2].gapSequence,
    //     results[3].gapSequence,
    //     results[4].gapSequence,
    //     results[5].gapSequence
    // };
    // auto finalResults = CompareShellSorts(SORTING_RANGE, finalGroup, 10000);
    // std::sort(finalResults.begin(), finalResults.end(), [](const Result& a, const Result& b) {
    //     return a.time < b.time;
    //     });
    // PrintResults(finalResults, finalGroup.size()); 

    // unsigned long sortingRange = 1000;
    // std::vector<GapSequence> finalGroup = 
    // { 
    //     GetTokudaGaps(sortingRange),
    //     GetCiuraGaps(sortingRange),
    //     GetLeeGaps(sortingRange),
    //     GetSkeanEhrenborgJaromczykGaps(sortingRange),
    //     GapSequence("GAv3-10000|Top3", { 2905, 891, 355, 160, 75, 29, 11, 5, 1 }),
    //     GapSequence("GAv3-10000|Top2", { 2304, 919, 390, 159, 69, 28, 12, 5, 1 }),
    //     GapSequence("GAv3-10000|Top1", { 3170, 983, 432, 191, 93, 35, 13, 5, 1 }),
    // };
    // auto finalResults = CompareShellSorts(sortingRange, finalGroup, 10000);
    // // std::sort(finalResults.begin(), finalResults.end(), [](const Result& a, const Result& b) {
    // //     return a.time < b.time;
    // //     });
    // PrintResults(finalResults, 7); 
}