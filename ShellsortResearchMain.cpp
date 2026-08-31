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

void PrintResults(std::vector<Result>& results, int topN = 10)
{
    std::cout << "\nResults:\n";
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
    std::string path = "Results/FinalSets/CompEval-WinningSequences.txt";
    std::vector<GapSequence> filesGaps = files::GetGapsFromFile(path);
    int ranges[] = {100, 250, 500, 750, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 100000};

    for(int range : ranges)
    {
        std::cout << "------------------------------------------------- " << range << " --------------------------------------------------" << std::endl;
        std::vector<GapSequence> finalGroup = filesGaps;
        finalGroup.push_back(GetTokudaGaps(range));
        finalGroup.push_back(GetCiuraGaps(range));
        finalGroup.push_back(GetLeeGaps(range));
        finalGroup.push_back(GetSkeanEhrenborgJaromczykGaps(range));
        auto results = CompareShellsorts(range, finalGroup, 1000000);
        PrintResults(results, results.size());
    }

    std::cout << "------------------------------------------------- Dynamic Ranges --------------------------------------------------" << std::endl;
    std::vector<GapSequence> finalGroup = filesGaps;
    finalGroup.push_back(GetTokudaGaps(10000));
    finalGroup.push_back(GetCiuraGaps(10000));
    finalGroup.push_back(GetLeeGaps(10000));
    finalGroup.push_back(GetSkeanEhrenborgJaromczykGaps(10000));
    auto results = CompareShellsorts_DynamicRanges(finalGroup, 1000000);
    PrintResults(results, results.size());


    std::cout << " ---------------------------------- Final Dynamic Benchmark ----------------------------------" << std::endl;
    std::vector<GapSequence> finalBenchmarkGroup = 
    {
        GetTokudaGaps(10000),
        GetCiuraGaps(10000),
        GetLeeGaps(10000),
        GetSkeanEhrenborgJaromczykGaps(10000)
    };
    auto benchmarkResults = CompareShellsorts_DynamicRanges(finalBenchmarkGroup, 100000);
    PrintResults(benchmarkResults, 4);
}


// Backups of previous runs

    // std::vector<GapSequence> gapSequences = 
    // { 
    //     GetTokudaGaps(SORTING_RANGE),
    //     GetCiuraGaps(SORTING_RANGE),
    //     GetLeeGaps(SORTING_RANGE),
    //     GetSkeanEhrenborgJaromczykGaps(SORTING_RANGE)
    // };

    // for (int i = gapSequences.size(); i<100; i++) gapSequences.push_back(GapSequence("1|Random|" + std::to_string(i + 1), GetRandomizedGaps(SORTING_RANGE)));
    
    // search_genetic_v5::EndlessGapSeeking(SORTING_RANGE, gapSequences, 100);

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



    // int ranges[] = {100, 250, 500, 750, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 100000};
    // for(int range : ranges)
    // {
    //     std::cout << "------------------------------------------------- " << range << " --------------------------------------------------" << std::endl;
    //     std::string path = "Results/FinalSets/Criterion-Comparisons/CandidateGapSequences_" + std::to_string(range) + ".txt";
    //     std::vector<GapSequence> filesGaps = files::GetGapsFromFile(path);
    //     auto results = CompareShellsorts(range, filesGaps, 10000);
    //     PrintResults(results, 10);

    //     std::vector<GapSequence> finalGroup;
    //     for(int i = 0; i < std::min(10, static_cast<int>(results.size())); ++i)
    //     {
    //         finalGroup.push_back(results[i].gapSequence);
    //     }

    //     results = CompareShellsorts(range, finalGroup, 100000);
    //     std::cout << "! Best:";
    //     PrintResults(results, 1);
    // }

    // std::cout << "------------------------------------------------- Dynamic Ranges --------------------------------------------------" << std::endl;
    // std::string path = "Results/FinalSets/CandidateGapSequences_CL-Merge.txt";
    // std::vector<GapSequence> filesGaps = files::GetGapsFromFile(path);
    // auto results = CompareShellsorts_DynamicRanges(filesGaps, 100);
    // std::cout << "Iteration 1";
    // PrintResults(results, 15);
    
    // std::vector<GapSequence> reducedGroup1;
    // for(int i = 0; i < std::min(10000, static_cast<int>(results.size())); ++i)
    // {
    //     reducedGroup1.push_back(results[i].gapSequence);
    // }
    // results = CompareShellsorts_DynamicRanges(reducedGroup1, 1000);
    // std::cout << "Iteration 2";
    // PrintResults(results, 15);

    // std::vector<GapSequence> reducedGroup2;
    // for(int i = 0; i < std::min(1000, static_cast<int>(results.size())); ++i)
    // {
    //     reducedGroup2.push_back(results[i].gapSequence);
    // }
    // results = CompareShellsorts_DynamicRanges(reducedGroup2, 10000);
    // std::cout << "Iteration 3";
    // PrintResults(results, 15);

    // std::vector<GapSequence> finalGroup;
    // for(int i = 0; i < std::min(15, static_cast<int>(results.size())); ++i)
    // {
    //     finalGroup.push_back(results[i].gapSequence);
    // }
    // results = CompareShellsorts_DynamicRanges(finalGroup, 100000);
    // std::cout << "! Best (iteration 4 - final):";
    // PrintResults(results, 3);