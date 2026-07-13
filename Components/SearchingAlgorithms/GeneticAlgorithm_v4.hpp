#ifndef GENETIC_ALGORITHM_V4_HPP
#define GENETIC_ALGORITHM_V4_HPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "../Utilis.hpp"
#include "../Shellsort.hpp"
#include "../ShellsortComparisions.hpp"
#include "../FilesManagement.hpp"
#include "CuckooSearch.hpp"

namespace search_genetic_v4
{
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

    std::vector<GapSequence> CrossParents(std::vector<GapSequence> parents, int populationIndex)
    {
        std::vector<GapSequence> childs;

        for (int i = 0; parents.size() >= 2 ; i += 12)
        {
            //Getting 2 random parents and removing them from the pool
            int randomIndex1 = utilis::GetRandomInt(0, parents.size() - 1);
            GapSequence parent1 = parents[randomIndex1];
            parents.erase(parents.begin() + randomIndex1);
            int randomIndex2 = utilis::GetRandomInt(0, parents.size() - 1);
            GapSequence parent2 = parents[randomIndex2];
            parents.erase(parents.begin() + randomIndex2);

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
            std::vector<unsigned long> child3Gaps;
            for (std::size_t j = 0; j < std::min(parent1.gaps.size(), parent2.gaps.size()); ++j)
            {
                child3Gaps.push_back((parent1.gaps[parent1.gaps.size() - j - 1] + parent2.gaps[parent2.gaps.size() - j - 1]) / 2);
            }
            std::reverse(child3Gaps.begin(), child3Gaps.end());

            //Child 4 and 5: new children generated gap by gap from distances between parents - as in ABC
            const std::size_t minSize = std::min(parent1.gaps.size(), parent2.gaps.size());
            std::vector<unsigned long> child4Gaps = std::vector<unsigned long>(parent1.gaps.begin(), parent1.gaps.end());
            std::reverse(child4Gaps.begin(), child4Gaps.end());
            std::vector<unsigned long> child5Gaps = std::vector<unsigned long>(parent2.gaps.begin(), parent2.gaps.end());
            std::reverse(child5Gaps.begin(), child5Gaps.end());
            for (std::size_t i = 1; i < minSize; ++i)
            {
                double currentGapC4 = static_cast<double>(child4Gaps[i]);
                double currentGapC5 = static_cast<double>(child5Gaps[i]);

                float phi = utilis::GetRandomFloat(-1.0f, 1.0f);
                double modifier = phi * (currentGapC4 - currentGapC5);

                double newGapC4 = currentGapC4 + modifier;
                double newGapC5 = currentGapC5 + modifier;

                if (newGapC4 > currentGapC4) { newGapC4 = std::ceil(newGapC4); }
                else { newGapC4 = std::floor(newGapC4); }
                if (newGapC4 < 1) newGapC4 = 1;

                if (newGapC5 > currentGapC5) { newGapC5 = std::ceil(newGapC5); }
                else { newGapC5 = std::floor(newGapC5); }
                if (newGapC5 < 1) newGapC5 = 1;

                child4Gaps[i] = static_cast<unsigned long>(newGapC4);
                child5Gaps[i] = static_cast<unsigned long>(newGapC5);
            }
            std::reverse(child4Gaps.begin(), child4Gaps.end());
            std::reverse(child5Gaps.begin(), child5Gaps.end());

            //Child 6,7,8,9,10,11,12: parents and children changed by levy flight - as in cuckoo search
            std::vector<unsigned long> child6Gaps;
            child6Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", parent1.gaps), 1.5, 0.03).gaps;
            std::vector<unsigned long> child7Gaps;
            child7Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", parent2.gaps), 1.5, 0.03).gaps;
            std::vector<unsigned long> child8Gaps;
            child8Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child1Gaps), 1.5, 0.03).gaps;
            std::vector<unsigned long> child9Gaps;
            child9Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child2Gaps), 1.5, 0.03).gaps;
            std::vector<unsigned long> child10Gaps;
            child10Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child3Gaps), 1.5, 0.03).gaps;
            std::vector<unsigned long> child11Gaps;
            child11Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child4Gaps), 1.5, 0.03).gaps;
            std::vector<unsigned long> child12Gaps;
            child12Gaps = search_cuckoo::PerformLevyFlight(GapSequence("TempChild", child5Gaps), 1.5, 0.03).gaps;

            //Indexing and adding children to the new population
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Cross12|" + std::to_string(i  + 1), child1Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Cross21|" + std::to_string(i  + 2), child2Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_Avg|" + std::to_string(i  + 3), child3Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_ABC1|" + std::to_string(i  + 4), child4Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_ABC2|" + std::to_string(i  + 5), child5Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyP1|" + std::to_string(i  + 6), child6Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyP2|" + std::to_string(i  + 7), child7Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyC1|" + std::to_string(i  + 8), child8Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyC2|" + std::to_string(i  + 9), child9Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyAVG|" + std::to_string(i  + 10), child10Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyABC1|" + std::to_string(i  + 11), child11Gaps));
            childs.push_back(GapSequence(std::to_string(populationIndex) + "|Child_LevyABC2|" + std::to_string(i  + 12), child12Gaps));
        }

        return childs;
    }

    std::vector<GapSequence> GetNewPopulation(unsigned long sortingRange, std::vector<GapSequence> oldPopulation, int populationIndex)
    {
        //Keep top 3 solutions, rest will be generated by crossing, mutation, and new random sequences
        std::vector<GapSequence> newPopulation = std::vector<GapSequence>(oldPopulation.begin(), oldPopulation.begin() + 1);
        for (std::size_t i = 0; i < newPopulation.size(); ++i)
        {
            newPopulation[i].name = std::to_string(populationIndex) + "|Survivor|" + std::to_string(i + 1);
        }

        //Cross top ~10% to get ~60% new children solutions
        std::vector<GapSequence> crossPopulation = std::vector<GapSequence>(oldPopulation.begin(), oldPopulation.begin() + utilis::RoundUpToEven(oldPopulation.size() / 10));
        crossPopulation = CrossParents(crossPopulation, populationIndex);

        newPopulation.insert(newPopulation.end(), crossPopulation.begin(), crossPopulation.end());

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

        //Generate random solutions to fill the population with new genes (~40%)
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
            std::cout << "\n\nGenetic Algorithm v3 iteration " << i << ":\n";
            std::cout << "Gaps sequences:\n";
            for (GapSequence sequence : algorithmGapSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "Sum of sequences: " << algorithmGapSequences.size() << "\n";

            std::cout << "\nGenetic Algorithm v3 generated gaps";
            results = CompareShellSorts(sortingRange, algorithmGapSequences, tryoutsIterations);

            std::cout << "\nChecking for new best";
            GapSequence best = CompareShellSorts(sortingRange, { results[0].gapSequence, GetCiuraGaps(sortingRange), GetSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations, true)[0].gapSequence;
            if (best == results[0].gapSequence && !IsGapSequenceIn(best, alreadyFound))
            {
                alreadyFound.push_back(best);
                std::cout << "\n\nNEW CANDIDATE SEQUENCE ---------------------------- NEW CANDIDATE SEQUENCE ---------------------------- NEW CANDIDATE SEQUENCE\n\n";
                files::SaveGapsToFile(sortingRange, "GAv4", best);
            }

            //creating new genetic sequences
            std::vector<GapSequence> newGapSequences;
            for (Result& r : results) newGapSequences.push_back(r.gapSequence);

            algorithmGapSequences = GetNewPopulation(sortingRange, newGapSequences, i + 1);
        }
    }
}

#endif // !GENETIC_ALGORITHM_V4_HPP