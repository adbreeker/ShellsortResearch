#ifndef CUCKOO_SEARCH_HPP
#define CUCKOO_SEARCH_HPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "../Shellsort.hpp"
#include "../ShellsortComparisions.hpp"
#include "../FilesManagement.hpp"

namespace search_cuckoo
{
    double getLevyDistribution(double beta)
    {
            // --- compute sigma_u ---
        double sigma_u = std::pow(
            (std::tgamma(1 + beta) * std::sin(M_PI * beta / 2)) /
            (std::tgamma((1 + beta) / 2) * beta * std::pow(2, (beta - 1) / 2)),
            1.0 / beta
        );

        double u = utilis::getNormalDistribution(0.0, sigma_u);
        double v = utilis::getNormalDistribution(0.0, 1.0);

        // --- Lévy step ---
        return u / std::pow(std::fabs(v), 1.0 / beta);
    }

    GapsSequence performLevyFlight(GapsSequence currentSolution, double beta, double stepSizeMultiplier = 0.01)
    {
        std::cout << "\nLeavy in: ";
        currentSolution.PrintInstance();

        for (std::size_t i = 0; i + 1 < currentSolution.gaps.size(); ++i)
        {
            unsigned long currentGap = currentSolution.gaps[i]; 

            double stepSize = stepSizeMultiplier * (currentGap);
            double levyStep = getLevyDistribution(beta) * stepSize;
            double newGap = currentGap + levyStep;

            if (newGap > currentGap) { newGap = std::ceil(newGap); }
            else { newGap = std::floor(newGap); }
            if (newGap < 1) newGap = 1;

            currentSolution.gaps[i] = static_cast<unsigned long>(newGap);
        }

        std::cout << "\nLeavy out: ";
        currentSolution.PrintInstance();

        return currentSolution;
    }

    std::vector<GapsSequence> exchangeNestsByLevyFlight(std::vector<GapsSequence> currentNests, double beta, int populationIndex)
    {
        std::vector<GapsSequence> newNests;

        for (std::size_t i = 0; i < currentNests.size() / 3; ++i)
        {
            GapsSequence newNest = performLevyFlight(currentNests[i], beta);
            newNest.name = "LevyChild" + std::to_string(populationIndex) + "-" + std::to_string(i + 1);
            newNests.push_back(newNest);
        }

        currentNests.erase(currentNests.end()-newNests.size(), currentNests.end());

        for (std::size_t i = 0; i < currentNests.size(); ++i)
        {
            currentNests[i].name = "Survivor" + std::to_string(populationIndex) + "-" + std::to_string(i + 1);
        }

        currentNests.insert(currentNests.end(), newNests.begin(), newNests.end());

        return currentNests;
    }

    std::vector<GapsSequence> getNewPopulation(unsigned long sortingRange, std::vector<GapsSequence> oldPopulation, int populationIndex)
    {
        std::vector<GapsSequence> newPopulation = std::vector<GapsSequence>(oldPopulation.begin(), oldPopulation.begin() + oldPopulation.size() * 3 / 4);

        newPopulation = exchangeNestsByLevyFlight(newPopulation, 1.5, populationIndex);

        for (std::size_t i = 0; newPopulation.size() < oldPopulation.size(); ++i)
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
            std::cout << "\n\nCuckoo iteration " << i << ":\n";
            std::cout << "Gaps:\n";
            for (GapsSequence sequence : algorithmGapsSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "Sum of sequences: " << algorithmGapsSequences.size() << "\n";

            std::cout << "\nCuckoo generated gaps - ";
            results = compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);

            std::cout << "\nChecking for new best - ";
            GapsSequence best = compareShellSorts(sortingRange, { results[0].gapsSequence, getCiuraGaps(sortingRange), getSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations, true)[0].gapsSequence;
            if (best == results[0].gapsSequence && !isGapsSequenceIn(best, alreadyFound))
            {
                alreadyFound.push_back(best);
                std::cout << "\n\n--------------------------------------------------- NEW BEST -------------------------------------------------------\n\n";
                files::saveGapsToFile(sortingRange, "cuckoo", best);
            }

            //creating new cuckoo sequences
            std::vector<GapsSequence> newGapsSequences;
            for (Result& r : results) newGapsSequences.push_back(r.gapsSequence);

            algorithmGapsSequences = getNewPopulation(sortingRange, newGapsSequences, i + 1);
        }
    }
}

#endif // !CUCKOO_SEARCH_HPP