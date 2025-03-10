#include <iostream>
#include <vector>
#include <fstream>
#include "GeneticAlgorithm.hpp"
#include "Shellsort.hpp"
#include "ShellsortComparisions.hpp"

const unsigned long SORTING_RANGE = 10000;

void PrintResults(std::vector<Result>& results)
{
    std::cout << "\n\nResults:\n";
    for (Result& r : results)
    {
        r.gapsSequence.PrintInstance();
        std::cout << "\n  Time: " << r.time << "ms | Operations: " << r.operations << " | Wins: " << r.wins << "\n\n";
    }
}

std::vector<std::string> splitString(std::string toSplit, const std::string& spliter) 
{
    std::vector<std::string> tokens;
    size_t start = 0, end;

    while ((end = toSplit.find(spliter, start)) != std::string::npos) 
    {
        std::string sub = toSplit.substr(start, end - start);
        if(sub.length() > 0) tokens.push_back(sub);
        start = end + spliter.length();
    }
    std::string sub = toSplit.substr(start);
    if(sub.length() > 0) tokens.push_back(sub); // Last part after the final delimiter
    return tokens;
}

std::vector<GapsSequence> getSequencesFromFile(unsigned long sortingRange)
{
    std::vector<GapsSequence> gapsFromFile;

    std::ifstream file("./Results/BestGapsSequences" + std::to_string(sortingRange) + ".txt");
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::vector<std::string> splited = splitString(line, ":");

            std::string sequenceName = splited[0];
            splited = splitString(splited[1], " ");
            
            std::vector<unsigned long> gaps;
            for (std::string& gap : splited)
            {
                gaps.push_back(std::stoul(gap));
            }

            gapsFromFile.push_back(GapsSequence(sequenceName, gaps));
        }
    }

    return gapsFromFile;
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


    //for (int i = gapsSequences.size(); i<100; i++) gapsSequences.push_back(GapsSequence("Random" + std::to_string(i + 1), getRandomizedGaps(SORTING_RANGE)));
    //genetic::endlessGapsSeeking(SORTING_RANGE, gapsSequences, 100);

    gapsSequences = getSequencesFromFile(SORTING_RANGE);

    auto results = compareShellSorts(SORTING_RANGE, gapsSequences, 100);
    PrintResults(results);
}