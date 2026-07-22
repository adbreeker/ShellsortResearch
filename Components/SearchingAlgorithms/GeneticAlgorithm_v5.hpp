#ifndef GENETIC_ALGORITHM_V5_HPP
#define GENETIC_ALGORITHM_V5_HPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "../Utilis.hpp"
#include "../Shellsort.hpp"
#include "../ShellsortComparisions.hpp"
#include "../FilesManagement.hpp"
#include "CuckooSearch.hpp"

namespace search_genetic_v5
{
    long stagnatedGenerations = 0;

    GapSequence MutateGapSequences(GapSequence gapSequence)
    {
        for (std::size_t i = 0; i + 1 < gapSequence.gaps.size(); ++i)
        {
            if (utilis::GetRandomFloat(0.0f, 1.0f) < 0.25f) //25% chance to mutate each gap
            {
                unsigned long currentGap = gapSequence.gaps[i];

                double mutationAmount = utilis::GetRandomFloat(-0.2f, 0.2f); //mutate by -20% to +20%
                double newGap = currentGap + (currentGap * mutationAmount);

                if (newGap > currentGap) { newGap = std::ceil(newGap); }
                else { newGap = std::floor(newGap); }
                if (newGap < 1) newGap = 1;

                gapSequence.gaps[i] = static_cast<unsigned long>(newGap);
            }
        }

        gapSequence.name += "|Mutated";

        return gapSequence;
    }

    //Best 4, unscaled - 1 survivor and 3 top pretendents crossed for exploitation of best solutions
    std::vector<GapSequence> CrossParentsForExploitation(std::vector<GapSequence> parents, int populationIndex)
    {
        std::vector<GapSequence> childs;

        for (int i = 0; parents.size() >= 2 ; i += 6)
        {
            //Getting 2 random parents and removing them from the pool
            int randomIndex1 = utilis::GetRandomInt(0, parents.size() - 1);
            GapSequence parent1 = parents[randomIndex1];
            parents.erase(parents.begin() + randomIndex1);
            int randomIndex2 = utilis::GetRandomInt(0, parents.size() - 1);
            GapSequence parent2 = parents[randomIndex2];
            parents.erase(parents.begin() + randomIndex2);
            //If parents are the same, mutate one of them
            if(parent1.gaps == parent2.gaps) { parent2 = MutateGapSequences(parent2); }

            //Child 1 and 2: new children generated gap by gap from distances between parents - as in ABC
            std::size_t minSize = std::min(parent1.gaps.size(), parent2.gaps.size());
            std::vector<unsigned long> child1Gaps = {1};
            std::vector<unsigned long> child2Gaps = {1};
            for (std::size_t j = 1; j < minSize; ++j)
            {
                double currentGapC1 = static_cast<double>(parent1.gaps[parent1.gaps.size() - j - 1]);
                double currentGapC2 = static_cast<double>(parent2.gaps[parent2.gaps.size() - j - 1]);

                double newGapC1 = currentGapC1 + (utilis::GetRandomFloat(-1.0f, 1.0f) * (currentGapC1 - currentGapC2));
                double newGapC2 = currentGapC2 + (utilis::GetRandomFloat(-1.0f, 1.0f) * (currentGapC2 - currentGapC1));

                if (newGapC1 > currentGapC1) { newGapC1 = std::ceil(newGapC1); }
                else { newGapC1 = std::floor(newGapC1); }
                if (newGapC1 < 1) newGapC1 = 1;

                if (newGapC2 > currentGapC2) { newGapC2 = std::ceil(newGapC2); }
                else { newGapC2 = std::floor(newGapC2); }
                if (newGapC2 < 1) newGapC2 = 1;

                child1Gaps.push_back(static_cast<unsigned long>(newGapC1));
                child2Gaps.push_back(static_cast<unsigned long>(newGapC2));
            }
            std::reverse(child1Gaps.begin(), child1Gaps.end());
            std::reverse(child2Gaps.begin(), child2Gaps.end());

            //Child 3: parent 1 changed by levy flight - as in cuckoo search
            std::vector<unsigned long> child3Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", parent1.gaps), 1.5, 0.03).gaps;

            //Child 4: parent 2 changed by levy flight - as in cuckoo search
            std::vector<unsigned long> child4Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", parent2.gaps), 1.5, 0.03).gaps;

            //Child 5: child 1 changed by levy flight - as in cuckoo search
            std::vector<unsigned long> child5Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child1Gaps), 1.5, 0.03).gaps;

            //Child 6: child 2 changed by levy flight - as in cuckoo search
            std::vector<unsigned long> child6Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child2Gaps), 1.5, 0.03).gaps;

            //Indexing and adding children to the new population
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_CrossABC1|" + std::to_string(i  + 1), child1Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_CrossABC2|" + std::to_string(i  + 2), child2Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyP1|" + std::to_string(i  + 3), child3Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyP2|" + std::to_string(i  + 4), child4Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyABC1|" + std::to_string(i  + 5), child5Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyABC2|" + std::to_string(i  + 6), child6Gaps));
        }

        return childs;
    }

    std::vector<GapSequence> CrossParentsForExploration(std::vector<GapSequence> parents, int populationIndex)
    {
        std::vector<GapSequence> childs;

        for (int i = 0; parents.size() >= 2 ; i += 4)
        {
            //Getting 2 random parents and removing them from the pool
            int randomIndex1 = utilis::GetRandomInt(0, parents.size() - 1);
            GapSequence parent1 = parents[randomIndex1];
            parents.erase(parents.begin() + randomIndex1);
            int randomIndex2 = utilis::GetRandomInt(0, parents.size() - 1);
            GapSequence parent2 = parents[randomIndex2];
            parents.erase(parents.begin() + randomIndex2);
            //If parents are the same, mutate one of them
            if(parent1.gaps == parent2.gaps) { parent2 = MutateGapSequences(parent2); }

            //Child 1: first half of parent1, then gaps from parent2 that are smaller than last gap in child1
            std::vector<unsigned long> child1Gaps(parent1.gaps.begin(), parent1.gaps.begin() + parent1.gaps.size() / 2);
            for (unsigned long gap : parent2.gaps)
            {
                if (gap < child1Gaps.back()) { child1Gaps.push_back(gap); }
            }

            //Child 2: first half of parent2, then gaps from parent1 that are smaller than last gap in child2
            std::vector<unsigned long> child2Gaps(parent2.gaps.begin(), parent2.gaps.begin() + parent2.gaps.size() / 2);
            for (unsigned long gap : parent1.gaps)
            {
                if (gap < child2Gaps.back()) { child2Gaps.push_back(gap); }
            }

            //Child 3: average of parent1 and parent2 gaps - inspired by ABC
            std::vector<unsigned long> child3Gaps = {1};
            for (std::size_t j = 1; j < std::min(parent1.gaps.size(), parent2.gaps.size()); ++j)
            {
                child3Gaps.push_back((parent1.gaps[parent1.gaps.size() - j - 1] + parent2.gaps[parent2.gaps.size() - j - 1]) / 2);
            }
            std::reverse(child3Gaps.begin(), child3Gaps.end());

            //Child 4: average of parent1 and parent2, changed by levy flight - as in cuckoo search
            std::vector<unsigned long> child4Gaps;
            child4Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child3Gaps), 2.0, 0.04).gaps;

            //Indexing and adding children to the new population
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Cross12|" + std::to_string(i  + 1), child1Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Cross21|" + std::to_string(i  + 2), child2Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Avg|" + std::to_string(i  + 3), child3Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Levy|" + std::to_string(i  + 4), child4Gaps));
        }

        return childs;
    }

    std::vector<GapSequence> GetNewPopulation(unsigned long sortingRange, std::vector<GapSequence> oldPopulation, int populationIndex)
    {
        //Keep top 1 solutions, rest will be generated by crossing, mutation, and new random sequences
        std::vector<GapSequence> newPopulation = std::vector<GapSequence>(oldPopulation.begin(), oldPopulation.begin() + 1);
        for (std::size_t i = 0; i < newPopulation.size(); ++i)
        {
            newPopulation[i].name = std::to_string(populationIndex) + "|Survivor|" + std::to_string(i + 1);
        }

        if (utilis::GetRandomInt(0, 1000) <= (0 + stagnatedGenerations)) //Increasing 0.1% for a cataclysm event wiping all but the survivor
        {
            stagnatedGenerations = 0;
            std::cout << "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CATACLYSM EVENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";
        }
        else //Population crossing and mutation (no cataclysm event)
        {
            //Cross top ~4% solutions to get ~12% children aimed at exploitation
            std::vector<GapSequence> crossPopulationExploitation = std::vector<GapSequence>(oldPopulation.begin(), oldPopulation.begin() + utilis::RoundUpToEven(oldPopulation.size() * 0.04f));
            crossPopulationExploitation = CrossParentsForExploitation(crossPopulationExploitation, populationIndex);
            newPopulation.insert(newPopulation.end(), crossPopulationExploitation.begin(), crossPopulationExploitation.end());

            //Cross top ~30% to get ~60% children solutions aimed at exploration
            std::vector<GapSequence> crossPopulationExploration = std::vector<GapSequence>(oldPopulation.begin(), oldPopulation.begin() + utilis::RoundUpToEven(oldPopulation.size() * 0.3f));
            crossPopulationExploration = CrossParentsForExploration(crossPopulationExploration, populationIndex);
            newPopulation.insert(newPopulation.end(), crossPopulationExploration.begin(), crossPopulationExploration.end());

            //Mutate some of the new population (survivors and childs)
            for (GapSequence& gs : newPopulation)
            {
                if (utilis::GetRandomFloat(0.0f, 1.0f) < 0.1f) //10% chance to mutate each gaps sequence
                {
                    gs = MutateGapSequences(gs);
                }
            }

            //Validate population to ensure all gaps are within range to avoid fake results
            for (GapSequence& gs : newPopulation) { gs.ValidateSequence(sortingRange); }
        }

        //Generate random solutions to fill the population with new genes (~27%-1 or 100%-1 if cataclysm event)
        for (std::size_t i = 0; newPopulation.size() < oldPopulation.size(); ++i)
        {
            newPopulation.push_back(GapSequence(
                std::to_string(populationIndex) + "|Random|" + std::to_string(i+1),
                GetRandomizedGaps(sortingRange)));
        }

        return newPopulation;
    }

    void EndlessGapSeeking(unsigned long sortingRange, std::vector<GapSequence> algorithmGapSequences, int tryoutsIterations)
    {
        std::vector<Result> results;

        std::vector<GapSequence> alreadyFound = 
        { 
            GetTokudaGaps(sortingRange),
            GetCiuraGaps(sortingRange), 
            GetLeeGaps(sortingRange),
            GetSkeanEhrenborgJaromczykGaps(sortingRange) 
        };

        for (long i = 1; true; i++)
        {
            std::cout << "\n\nGenetic Algorithm v5 iteration " << i << ":\n";
            std::cout << "Gaps sequences:\n";
            for (GapSequence sequence : algorithmGapSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "Sum of sequences: " << algorithmGapSequences.size() << "\n";

            std::cout << "\nGenetic Algorithm v5 generated gaps";
            results = CompareShellSorts(sortingRange, algorithmGapSequences, tryoutsIterations);

            std::cout << "\nChecking for new best";
            GapSequence best = CompareShellSorts(sortingRange, { results[0].gapSequence, GetCiuraGaps(sortingRange), GetSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations, true)[0].gapSequence;
            if (best == results[0].gapSequence && !IsGapSequenceIn(best, alreadyFound))
            {
                stagnatedGenerations = 0;
                alreadyFound.push_back(best);
                std::cout << "\n\nNEW CANDIDATE SEQUENCE ---------------------------- NEW CANDIDATE SEQUENCE ---------------------------- NEW CANDIDATE SEQUENCE\n\n";
                files::SaveGapsToFile(sortingRange, "GAv5", best);
            }
            else
            {
                stagnatedGenerations++;
            }

            //creating new genetic sequences
            std::vector<GapSequence> newGapSequences;
            for (Result& r : results) newGapSequences.push_back(r.gapSequence);
            algorithmGapSequences = GetNewPopulation(sortingRange, newGapSequences, i + 1);
        }
    }
}

#endif // !GENETIC_ALGORITHM_V5_HPP