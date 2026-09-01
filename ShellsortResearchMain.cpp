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

const unsigned long SORTING_RANGE = 1000; 

int main() 
{
    // --- Searching process:

    std::vector<GapSequence> gapSequences = 
    { 
        GetTokudaGaps(SORTING_RANGE),
        GetCiuraGaps(SORTING_RANGE),
        GetLeeGaps(SORTING_RANGE),
        GetSkeanEhrenborgJaromczykGaps(SORTING_RANGE)
    };

    for (int i = gapSequences.size(); i<100; i++) gapSequences.push_back(GapSequence("1|Random|" + std::to_string(i + 1), GetRandomizedGaps(SORTING_RANGE)));
    
    // a) with a single leading algorithm (GAv5):
    search_genetic_v5::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

    // b) comparing multiple algorithm in parallel (GAv1-5):
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

    // --- Evaluations process:

    // std::string path = "Results/FinalSets/CompEval-WinningSequences.txt";
    // std::vector<GapSequence> filesGaps = files::GetGapsFromFile(path);
    // int ranges[] = {100, 250, 500, 750, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 100000};

    // for(int range : ranges)
    // {
    //     std::cout << "------------------------------------------------- " << range << " --------------------------------------------------" << std::endl;
    //     std::vector<GapSequence> finalGroup = filesGaps;
    //     finalGroup.push_back(GetTokudaGaps(range));
    //     finalGroup.push_back(GetCiuraGaps(range));
    //     finalGroup.push_back(GetLeeGaps(range));
    //     finalGroup.push_back(GetSkeanEhrenborgJaromczykGaps(range));
    //     auto results = CompareShellsorts(range, finalGroup, 1000000);
    //     PrintResults(results, results.size());
    // }

    // std::cout << "------------------------------------------------- Dynamic Ranges --------------------------------------------------" << std::endl;
    // std::vector<GapSequence> finalGroup = filesGaps;
    // finalGroup.push_back(GetTokudaGaps(10000));
    // finalGroup.push_back(GetCiuraGaps(10000));
    // finalGroup.push_back(GetLeeGaps(10000));
    // finalGroup.push_back(GetSkeanEhrenborgJaromczykGaps(10000));
    // auto results = CompareShellsorts_DynamicRanges(finalGroup, 1000000);
    // PrintResults(results, results.size());


    // std::cout << " ---------------------------------- Final Dynamic Benchmark ----------------------------------" << std::endl;
    // std::vector<GapSequence> finalBenchmarkGroup = 
    // {
    //     GetTokudaGaps(10000),
    //     GetCiuraGaps(10000),
    //     GetLeeGaps(10000),
    //     GetSkeanEhrenborgJaromczykGaps(10000)
    // };
    // auto benchmarkResults = CompareShellsorts_DynamicRanges(finalBenchmarkGroup, 100000);
    // PrintResults(benchmarkResults, 4);
}