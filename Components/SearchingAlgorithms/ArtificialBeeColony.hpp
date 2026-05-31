#ifndef ARTIFICIAL_BEE_COLONY_HPP
#define ARTIFICIAL_BEE_COLONY_HPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <tuple>
#include "../Utilis.hpp"
#include "../Shellsort.hpp"
#include "../ShellsortComparisions.hpp"
#include "../FilesManagement.hpp"

namespace search_abc
{
    struct FoodSource
    {
        GapsSequence gapsSequence;
        double fitnessScore;
        int trialCounter;
    };

    GapsSequence getNeighborSolution(GapsSequence currentFoodSource, GapsSequence randomFoodSource)
    {
        std::reverse(currentFoodSource.gaps.begin(), currentFoodSource.gaps.end());
        std::reverse(randomFoodSource.gaps.begin(), randomFoodSource.gaps.end());

        const std::size_t minSize = std::min(currentFoodSource.gaps.size(), randomFoodSource.gaps.size());
        for (std::size_t i = 1; i < minSize; ++i)
        {
            double currentGap = static_cast<double>(currentFoodSource.gaps[i]);

            float phi = utilis::getRandomFloat(-1.0f, 1.0f);
            double newGap = currentGap + phi * (static_cast<double>(currentFoodSource.gaps[i]) - static_cast<double>(randomFoodSource.gaps[i]));

            if (newGap > currentGap) { newGap = std::ceil(newGap); }
            else { newGap = std::floor(newGap); }
            if (newGap < 1) newGap = 1;

            currentFoodSource.gaps[i] = static_cast<unsigned long>(newGap);
        }
        
        std::reverse(currentFoodSource.gaps.begin(), currentFoodSource.gaps.end());
        return currentFoodSource;
    }

    // Employed Bees Phase tuple (sequence, fitnessScore, trialCounter)
    std::vector<FoodSource> employedBeesPhase(std::vector<FoodSource> currentSolutions, unsigned long sortingRange, int populationIndex)
    {
        for (std::size_t i = 0; i < currentSolutions.size(); ++i)
        {
            int j;
            do {
                j = utilis::getRandomInt(0, static_cast<int>(currentSolutions.size() - 1));
            } while (j == static_cast<int>(i));

            GapsSequence currentFoodSource = currentSolutions[i].gapsSequence;
            GapsSequence randomFoodSource = currentSolutions[j].gapsSequence;
            GapsSequence neighborSolution = getNeighborSolution(currentFoodSource, randomFoodSource);

            neighborSolution.name = "EmployedNeighborhood" + std::to_string(populationIndex) + "-" + std::to_string(i + 1);
            currentFoodSource.name = "EmployedRemaining" + std::to_string(populationIndex) + "-" + std::to_string(i + 1); 

            Result better = compareShellSorts(sortingRange, { currentFoodSource, neighborSolution }, 10)[0];

            if (better.gapsSequence == neighborSolution)
            {
                currentSolutions[i] = FoodSource{neighborSolution, better.operations, 0}; //replace source
            }
            else
            {
                currentSolutions[i].gapsSequence = currentFoodSource; //keep old source
                currentSolutions[i].fitnessScore = better.operations;
                currentSolutions[i].trialCounter += 1; //increase trial counter
            }
        }
        return currentSolutions;
    }

    std::vector<FoodSource> onlookerBeesPhase(std::vector<FoodSource> currentSolutions, unsigned long sortingRange, int populationIndex)
    {
        // Calculate fitness (inverse of operations - lower operations = higher fitness)
        std::vector<double> fitness(currentSolutions.size());
        double fitnessSum = 0.0;
        
        for (std::size_t i = 0; i < currentSolutions.size(); ++i)
        {
            // Inverse fitness: better solutions (fewer operations) get higher fitness
            // Add 1 to avoid division by zero
            fitness[i] = (currentSolutions[i].fitnessScore > 0) ? 
                         (1.0 / currentSolutions[i].fitnessScore) : 1.0;
            fitnessSum += fitness[i];
        }

        // Each onlooker bee evaluates one solution (same number as food sources)
        for (std::size_t onlooker = 0; onlooker < currentSolutions.size(); ++onlooker)
        {
            // Roulette wheel selection - select solution based on fitness probability
            double r = utilis::getRandomDouble(0.0, fitnessSum);
            double cumulativeFitness = 0.0;
            int selectedIndex = 0;

            for (std::size_t i = 0; i < currentSolutions.size(); ++i)
            {
                cumulativeFitness += fitness[i];
                if (cumulativeFitness >= r)
                {
                    selectedIndex = static_cast<int>(i);
                    break;
                }
            }

            // Find a different random solution for perturbation
            int j;
            do {
                j = utilis::getRandomInt(0, static_cast<int>(currentSolutions.size() - 1));
            } while (j == selectedIndex);

            GapsSequence currentFoodSource = currentSolutions[selectedIndex].gapsSequence;
            GapsSequence randomFoodSource = currentSolutions[j].gapsSequence;
            GapsSequence neighborSolution = getNeighborSolution(currentFoodSource, randomFoodSource);

            neighborSolution.name = "OnlookerNeighborhood" + std::to_string(populationIndex) + "-" + std::to_string(selectedIndex + 1);
            currentFoodSource.name = "OnlookerRemaining" + std::to_string(populationIndex) + "-" + std::to_string(selectedIndex + 1); 

            Result better = compareShellSorts(sortingRange, { currentFoodSource, neighborSolution }, 10)[0];

            if (better.gapsSequence == neighborSolution)
            {
                currentSolutions[selectedIndex] = FoodSource{neighborSolution, better.operations, 0};
                fitness[selectedIndex] = 1.0 / better.operations;
                // Update fitnessSum
                fitnessSum = 0.0;
                for (std::size_t i = 0; i < currentSolutions.size(); ++i) fitnessSum += fitness[i];
            }
            else
            {
                currentSolutions[selectedIndex].gapsSequence = currentFoodSource; //keep old source
                currentSolutions[selectedIndex].trialCounter += 1; //increase trial counter
            }
        }

        return currentSolutions;
    }

    std::vector<FoodSource> scoutBeesPhase(std::vector<FoodSource> currentSolutions, unsigned long sortingRange, int populationIndex, int sourceLimit)
    {
        for (std::size_t i = 0; i < currentSolutions.size(); ++i)
        {
            if (currentSolutions[i].trialCounter >= sourceLimit)
            {
                GapsSequence newSolution = GapsSequence(
                    "ScoutRandom" + std::to_string(populationIndex) + "-" + std::to_string(i + 1),
                    getRandomizedGaps(sortingRange));

                currentSolutions[i] = FoodSource{newSolution, 0, 0};
            }
        }
        return currentSolutions;
    }

    std::vector<FoodSource> getNewPopulation(unsigned long sortingRange, std::vector<FoodSource> oldPopulation, int populationIndex)
    {
        std::vector<FoodSource> newPopulation = employedBeesPhase(oldPopulation, sortingRange, populationIndex);
        newPopulation = onlookerBeesPhase(newPopulation, sortingRange, populationIndex);
        newPopulation = scoutBeesPhase(newPopulation, sortingRange, populationIndex, 20);
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

        std::vector<FoodSource> foodSources = std::vector<FoodSource>();
        for (std::size_t i = 0; i < algorithmGapsSequences.size(); ++i)
        {
            foodSources.push_back(FoodSource{ algorithmGapsSequences[i], 0, 0});
        }

        for (long i = 1; true; i++)
        {
            std::cout << "\n\nABC iteration " << i << ":\n";
            std::cout << "Gaps:\n";
            for (FoodSource foodSource : foodSources)
            {
                foodSource.gapsSequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "Sum of sequences: " << foodSources.size() << "\n";

            std::cout << "\nABC generated gaps - ";
            algorithmGapsSequences.clear();
            for (FoodSource& fs : foodSources) algorithmGapsSequences.push_back(fs.gapsSequence);
            results = compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);

            std::cout << "\nChecking for new best - ";
            GapsSequence best = compareShellSorts(sortingRange, { results[0].gapsSequence, getCiuraGaps(sortingRange), getSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations, true)[0].gapsSequence;
            if (best == results[0].gapsSequence && !isGapsSequenceIn(best, alreadyFound))
            {
                alreadyFound.push_back(best);
                std::cout << "\n\n--------------------------------------------------- NEW BEST -------------------------------------------------------\n\n";
                files::saveGapsToFile(sortingRange, "abc", best);
            }

            foodSources = getNewPopulation(sortingRange, foodSources, i + 1);
        }
    }
}

#endif // !ARTIFICIAL_BEE_COLONY_HPP