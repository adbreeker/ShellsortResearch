#include <iostream>
#include <vector>
#include <fstream>
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v1.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v2.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v3.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v4.hpp"
#include "Components/SearchingAlgorithms/GeneticAlgorithm_v5.hpp"
#include "Components/SearchingAlgorithms/CuckooSearch.hpp"
#include "Components/SearchingAlgorithms/ArtificialBeeColony.hpp"
#include "Components/Shellsort.hpp"
#include "Components/ShellsortComparisons.hpp"
#include "Components/FilesManagement.hpp"
#include "omp.h"

const unsigned long SORTING_RANGE = 500; 

void PrintResults(std::vector<Result>& results, int topN = 10)
{
    std::cout << "\n\nResults:\n";
    for (int i = 0; i < std::min(topN, static_cast<int>(results.size())); ++i)
    {
        auto& r = results[i];
        r.gapSequence.PrintInstance();
        std::cout << "\n  Time: " << r.time << "ms | Wins: " << r.wins
            << "\n  Comparisons: " << r.comparisons << " | Loops: " << r.loops << " | Operations: " << r.operations << "\n\n";
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
    
    // #pragma omp parallel sections num_threads(5) firstprivate(gapSequences)
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
    //     #pragma omp section
    //     {
    //         search_genetic_v4::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    //     #pragma omp section
    //     {
    //         search_genetic_v5::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);
    //     }
    // }

    search_genetic_v5::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

    // std::vector<GapSequence> filesGaps = files::GetGapsFromFile("CandidateGapSequences" + std::to_string(SORTING_RANGE) + "_GAv5-nr2.txt");
    // auto results = CompareShellsorts(SORTING_RANGE, filesGaps, 1000);
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
    // auto finalResults = CompareShellsorts(SORTING_RANGE, finalGroup, 10000);
    // std::sort(finalResults.begin(), finalResults.end(), [](const Result& a, const Result& b) {
    //     return a.GetFitnessScore() < b.GetFitnessScore();
    //     });
    // PrintResults(finalResults, finalGroup.size()); 

    // unsigned long sortingRange = 5000;
    // std::vector<GapSequence> finalGroup2 = 
    // { 
    //     GetTokudaGaps(sortingRange),
    //     GetCiuraGaps(sortingRange),
    //     GetLeeGaps(sortingRange),
    //     GetSkeanEhrenborgJaromczykGaps(sortingRange),
    //     GapSequence("GAv5-5000-1|Top1", { 1903, 517, 209, 90, 28, 12, 5, 1 }),
    //     GapSequence("GAv5-5000-1|Top2", { 1879, 498, 191, 69, 30, 11, 5, 1 }),
    //     GapSequence("GAv5-5000-2|Top1", finalResults[0].gapSequence.gaps),
    //     GapSequence("GAv5-5000-2|Top2", finalResults[1].gapSequence.gaps),
    // };
    // auto finalResults2 = CompareShellsorts(sortingRange, finalGroup2, 10000);
    // // std::sort(finalResults.begin(), finalResults.end(), [](const Result& a, const Result& b) {
    // //     return a.time < b.time;
    // //     });
    // PrintResults(finalResults2, finalGroup2.size()); 
}