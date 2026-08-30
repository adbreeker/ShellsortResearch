#ifndef FILES_MANAGEMENT_HPP
#define FILES_MANAGEMENT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "Utilis.hpp"
#include "Shellsort.hpp"

namespace files
{
    void SaveGapsToFile(unsigned long sortingRange, std::string algorithmName, GapSequence sequence)
    {
        std::string filename = "Results/CandidateGapSequences" + std::to_string(sortingRange) + "_" + algorithmName + ".txt";
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


    std::vector<GapSequence> GetGapsFromFile(std::string path)
    {
        std::vector<GapSequence> gapsFromFile;

        std::ifstream file(path);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                if (!line.empty() && line.back() == '\r') { line.pop_back(); }
                if (line == "###") { std::cout << "Reading from file " << path << " forced break." << std::endl; break; } // Reading break condition
                if (line.empty()) continue;

                std::vector<std::string> splited = utilis::SplitString(line, ":");
                if (splited.size() < 2) continue;

                std::string sequenceName = splited[0];
                std::vector<std::string> strGaps = utilis::SplitString(splited[1], " ");

                std::vector<unsigned long> gaps;
                for (const std::string& gap : strGaps)
                {
                    if (gap.empty() || gap.find_first_not_of(" \t\r\n") == std::string::npos) { continue; }

                    try 
                    {
                        gaps.push_back(std::stoul(gap));
                    } 
                    catch (const std::invalid_argument& e) 
                    {
                        printf("WARNING: Invalid gap value '%s' in file '%s'. Skipping this gap.\n", gap.c_str(), path.c_str());
                        continue;
                    }
                }

                gapsFromFile.push_back(GapSequence(sequenceName, gaps));
            }
        }

        return gapsFromFile;
    }
}


#endif // !FILES_MANAGEMENT_HPP