#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "Shellsort.hpp"
#include "ShellsortComparisions.hpp"
#include "FilesManagement.hpp"

namespace genetic
{
    std::vector<GapsSequence> crossParents(std::vector<GapsSequence> parents, int populationIndex)
    {
        std::vector<GapsSequence> childs;

        for (int i = 0; i < parents.size(); i += 2)
        {
            GapsSequence parent1 = parents[i];
            GapsSequence parent2;
            if (i + 1 < parents.size()) { parent2 = parents[i + 1]; }
            else { parent2 = parents[0]; }

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

            childs.push_back(GapsSequence("Child" + std::to_string(populationIndex) + "-" + std::to_string(i + 1), child1Gaps));
            childs.push_back(GapsSequence("Child" + std::to_string(populationIndex) + "-" + std::to_string(i + 2), child2Gaps));
        }

        return childs;
    }

    std::vector<GapsSequence> getNewPopulation(unsigned long sortingRange, std::vector<GapsSequence> oldPopulation, int populationIndex)
    {
        std::vector<GapsSequence> newPopulation = std::vector<GapsSequence>(oldPopulation.begin(), oldPopulation.begin() + oldPopulation.size() / 2);

        newPopulation = crossParents(newPopulation, populationIndex);

        for (int i = newPopulation.size(); i < oldPopulation.size(); i++)
        {
            newPopulation.push_back(GapsSequence(
                "Random" + std::to_string(populationIndex) + "-" + std::to_string(i+1),
                getRandomizedGaps(sortingRange)));
        }

        return newPopulation;
    }

    std::vector<Result> iteratedGapsSeeking(unsigned long sortingRange, std::vector<GapsSequence> algorithmGapsSequences, int tryoutsIterations, int geneticIterations)
    {
        std::vector<Result> results;

        for (int i = 0; i < geneticIterations; i++)
        {
            std::cout << "\n\nGenetic iteration " << i << ":\n";
            std::cout << "Gaps:\n";
            for (GapsSequence sequence : algorithmGapsSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "\n";

            std::cout << "Genetic generated gaps -  ";
            results = compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);

            //creating new genetic sequences
            std::vector<GapsSequence> newGapsSequences;
            for (Result& r : results) newGapsSequences.push_back(r.gapsSequence);

            algorithmGapsSequences = getNewPopulation(sortingRange, newGapsSequences, i+1);
        }

        return compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);
    }

    void endlessGapsSeeking(unsigned long sortingRange, std::vector<GapsSequence> algorithmGapsSequences, int tryoutsIterations)
    {
        std::vector<Result> results;

        for (long i = 1; true; i++)
        {
            std::cout << "\n\nGenetic iteration " << i << ":\n";
            std::cout << "Gaps:\n";
            for (GapsSequence sequence : algorithmGapsSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }

            std::cout << "\nGenetic generated gaps - ";
            results = compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);

            std::cout << "\nChecking for new best - ";
            GapsSequence best = compareShellSorts(sortingRange, { results[0].gapsSequence, getCiuraGaps(sortingRange), getSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations)[0].gapsSequence;
            if (best == results[0].gapsSequence && !(best.gaps == getCiuraGaps(sortingRange).gaps || best.gaps == getSkeanEhrenborgJaromczykGaps(sortingRange).gaps))
            {
                std::cout << "\n\n--------------------------------------------------- NEW BEST -------------------------------------------------------\n\n";
                files::saveGapsToFile(sortingRange, best);
            }

            //creating new genetic sequences
            std::vector<GapsSequence> newGapsSequences;
            for (Result& r : results) newGapsSequences.push_back(r.gapsSequence);

            algorithmGapsSequences = getNewPopulation(sortingRange, newGapsSequences, i + 1);
        }
    }
}

#endif // !GENETIC_ALGORITHM_HPP