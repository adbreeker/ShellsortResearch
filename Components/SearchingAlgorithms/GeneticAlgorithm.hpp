#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "../Shellsort.hpp"
#include "../ShellsortComparisions.hpp"
#include "../FilesManagement.hpp"

namespace search_genetic
{
    GapsSequence MutateGapsSequence(GapsSequence gapsSequence)
    {
        for (int i = 0; i < gapsSequence.gaps.size() - 1; i++)
        {
            if (utilis::getRandomFloat(0.0f, 1.0f) < 0.25f) //25% chance to mutate each gap
            {
                unsigned long currentGap = gapsSequence.gaps[i];

                double mutationAmount = utilis::getRandomFloat(-0.2f, 0.2f); //mutate by -20% to +20%
                double newGap = currentGap + (currentGap * mutationAmount);

                if (newGap > currentGap) { newGap = std::ceil(newGap); }
                else { newGap = std::floor(newGap); }
                if (newGap < 1) newGap = 1;

                gapsSequence.gaps[i] = static_cast<unsigned long>(newGap);
            }
        }

        gapsSequence.name += "_mutated";

        return gapsSequence;
    }

    std::vector<GapsSequence> crossParents(std::vector<GapsSequence> parents, int populationIndex)
    {
        std::vector<GapsSequence> childs;

        for (int i = 0; parents.size() >= 2 ; i += 2)
        {
            int randomIndex1 = utilis::getRandomInt(0, parents.size() - 1);
            GapsSequence parent1 = parents[randomIndex1];
            parents.erase(parents.begin() + randomIndex1);
            int randomIndex2 = utilis::getRandomInt(0, parents.size() - 1);
            GapsSequence parent2 = parents[randomIndex2];
            parents.erase(parents.begin() + randomIndex2);

            std::vector<unsigned long> child1Gaps(parent1.gaps.begin(), parent1.gaps.begin() + parent1.gaps.size() / 2);
            for (unsigned long gap : parent2.gaps)
            {
                if (gap < child1Gaps.back()) { child1Gaps.push_back(gap); }
            }

            std::vector<unsigned long> child2Gaps(parent2.gaps.begin(), parent2.gaps.begin() + parent2.gaps.size() / 2);
            for (unsigned long gap : parent1.gaps)
            {
                if (gap < child2Gaps.back()) { child2Gaps.push_back(gap); }
            }

            childs.push_back(GapsSequence("Child" + std::to_string(populationIndex) + "-" + std::to_string(i  + 1), child1Gaps));
            childs.push_back(GapsSequence("Child" + std::to_string(populationIndex) + "-" + std::to_string(i  + 2), child2Gaps));
        }

        return childs;
    }

    std::vector<GapsSequence> getNewPopulation(unsigned long sortingRange, std::vector<GapsSequence> oldPopulation, int populationIndex)
    {
        std::vector<GapsSequence> newPopulation = std::vector<GapsSequence>(oldPopulation.begin(), oldPopulation.begin() + oldPopulation.size() / 10);
        for (int i = 0; i < newPopulation.size(); i++)
        {
            newPopulation[i].name = "Survivor" + std::to_string(populationIndex) + "-" + std::to_string(i + 1);
        }

        std::vector<GapsSequence> crossPopulation = std::vector<GapsSequence>(oldPopulation.begin(), oldPopulation.begin() + oldPopulation.size() / 2);
        crossPopulation = crossParents(crossPopulation, populationIndex);

        newPopulation.insert(newPopulation.end(), crossPopulation.begin(), crossPopulation.end());

        for (GapsSequence& gs : newPopulation)
        {
            if (utilis::getRandomFloat(0.0f, 1.0f) < 0.1f) //10% chance to mutate each gaps sequence
            {
                gs = MutateGapsSequence(gs);
            }
        }

        for (int i = 0; newPopulation.size() < oldPopulation.size(); i++)
        {
            newPopulation.push_back(GapsSequence(
                "Random" + std::to_string(populationIndex) + "-" + std::to_string(i+1),
                getRandomizedGaps(sortingRange)));
        }

        return newPopulation;
    }

    void endlessGapsSeeking(unsigned long sortingRange, std::vector<GapsSequence> algorithmGapsSequences, int tryoutsIterations)
    {
        std::vector<Result> results;

        std::vector<GapsSequence> alreadyFound = 
        { 
            getTokudaGaps(sortingRange),
            getCiuraGaps(sortingRange), 
            getLeeGaps(sortingRange),
            getSkeanEhrenborgJaromczykGaps(sortingRange) 
        };

        for (long i = 1; true; i++)
        {
            std::cout << "\n\nGenetic iteration " << i << ":\n";
            std::cout << "Gaps sequences:\n";
            for (GapsSequence sequence : algorithmGapsSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "Sum of sequences: " << algorithmGapsSequences.size() << "\n";

            std::cout << "\nGenetic generated gaps - ";
            results = compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);

            std::cout << "\nChecking for new best - ";
            GapsSequence best = compareShellSorts(sortingRange, { results[0].gapsSequence, getCiuraGaps(sortingRange), getSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations)[0].gapsSequence;
            if (best == results[0].gapsSequence && !isGapsSequenceIn(best, alreadyFound))
            {
                alreadyFound.push_back(best);
                std::cout << "\n\n--------------------------------------------------- NEW BEST -------------------------------------------------------\n\n";
                files::saveGapsToFile(sortingRange, "genetic", best);
            }

            //creating new genetic sequences
            std::vector<GapsSequence> newGapsSequences;
            for (Result& r : results) newGapsSequences.push_back(r.gapsSequence);

            algorithmGapsSequences = getNewPopulation(sortingRange, newGapsSequences, i + 1);
        }
    }
}

#endif // !GENETIC_ALGORITHM_HPP