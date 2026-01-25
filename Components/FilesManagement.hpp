#ifndef FILES_MANAGEMENT_HPP
#define FILES_MANAGEMENT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "Shellsort.hpp"

std::vector<std::string> splitString(std::string toSplit, const std::string& spliter)
{
    std::vector<std::string> tokens;
    size_t start = 0, end;

    while ((end = toSplit.find(spliter, start)) != std::string::npos)
    {
        std::string sub = toSplit.substr(start, end - start);
        if (sub.length() > 0) tokens.push_back(sub);
        start = end + spliter.length();
    }
    std::string sub = toSplit.substr(start);
    if (sub.length() > 0) tokens.push_back(sub); // Last part after the final delimiter
    return tokens;
}

namespace files
{
    void saveGapsToFile(unsigned long sortingRange, std::string algorithmName, GapsSequence sequence)
    {
        std::string filename = "Results/BestGapsSequences" + std::to_string(sortingRange) + "_" + algorithmName + ".txt";
        std::ofstream file(filename, std::ios::app);
        
        if (!file.is_open())
        {
            std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
            return;
        }
        
        std::string line = sequence.name + ": ";
        for (unsigned long gap : sequence.gaps) line += (std::to_string(gap) + " ");
        file << line << "\n";
        file.flush();  // Ensure data is written to disk
        file.close();
        
        std::cout << "Saved to: " << filename << std::endl;
    }


    std::vector<GapsSequence> getGapsFromFile(std::string fileName)
    {
        std::vector<GapsSequence> gapsFromFile;

        std::string path = "Results/" + fileName;
        std::ifstream file(path);
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
}


#endif // !FILES_MANAGEMENT_HPP