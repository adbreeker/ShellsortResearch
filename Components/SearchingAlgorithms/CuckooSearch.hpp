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
    double GetLevyDistribution(double beta)
    {
            // --- compute sigma_u ---
        double sigma_u = std::pow(
            (std::tgamma(1 + beta) * std::sin(M_PI * beta / 2)) /
            (std::tgamma((1 + beta) / 2) * beta * std::pow(2, (beta - 1) / 2)),
            1.0 / beta
        );

        double u = utilis::GetNormalDistribution(0.0, sigma_u);
        double v = utilis::GetNormalDistribution(0.0, 1.0);

        // --- Lévy step ---
        return u / std::pow(std::fabs(v), 1.0 / beta);
    }

    GapSequence PerformLevyFlight(GapSequence currentSolution, double beta, double stepSizeMultiplier = 0.01)
    {
        //std::cout << "\nLeavy in: ";
        //currentSolution.PrintInstance();

        for (std::size_t i = 0; i + 1 < currentSolution.gaps.size(); ++i)
        {
            unsigned long currentGap = currentSolution.gaps[i]; 

            double stepSize = stepSizeMultiplier * (currentGap);
            double levyStep = GetLevyDistribution(beta) * stepSize;
            double newGap = currentGap + levyStep;

            if (newGap > currentGap) { newGap = std::ceil(newGap); }
            else { newGap = std::floor(newGap); }
            if (newGap < 1) newGap = 1;

            currentSolution.gaps[i] = static_cast<unsigned long>(newGap);
        }

        //std::cout << "\nLeavy out: ";
        //currentSolution.PrintInstance();

        return currentSolution;
    }

    std::vector<GapSequence> ExchangeNestsByLevyFlight(std::vector<GapSequence> currentNests, double beta, int populationIndex)
    {
        std::vector<GapSequence> newNests;

        for (std::size_t i = 0; i < currentNests.size() / 3; ++i)
        {
            GapSequence newNest = PerformLevyFlight(currentNests[i], beta);
            newNest.name = std::to_string(populationIndex) + "|LevyChild|" + std::to_string(i + 1);
            newNests.push_back(newNest);
        }

        currentNests.erase(currentNests.end()-newNests.size(), currentNests.end());

        for (std::size_t i = 0; i < currentNests.size(); ++i)
        {
            currentNests[i].name = std::to_string(populationIndex) + "|Survivor|" + std::to_string(i + 1);
        }

        currentNests.insert(currentNests.end(), newNests.begin(), newNests.end());

        return currentNests;
    }

    std::vector<GapSequence> GetNewPopulation(unsigned long sortingRange, std::vector<GapSequence> oldPopulation, int populationIndex)
    {
        std::vector<GapSequence> newPopulation = std::vector<GapSequence>(oldPopulation.begin(), oldPopulation.begin() + oldPopulation.size() * 3 / 4);

        newPopulation = ExchangeNestsByLevyFlight(newPopulation, 1.5, populationIndex);

        //Validate population to ensure all gaps are within range to avoid fake results
        for (GapSequence& gs : newPopulation) { gs.ValidateSequence(sortingRange); }

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
            std::cout << "\n\nCuckoo iteration " << i << ":\n";
            std::cout << "Gaps:\n";
            for (GapSequence sequence : algorithmGapSequences)
            {
                sequence.PrintInstance();
                std::cout << "\n";
            }
            std::cout << "Sum of sequences: " << algorithmGapSequences.size() << "\n";

            std::cout << "\nCuckoo generated gaps";
            results = CompareShellSorts(sortingRange, algorithmGapSequences, tryoutsIterations);

            std::cout << "\nChecking for new best";
            GapSequence best = CompareShellSorts(sortingRange, { results[0].gapSequence, GetCiuraGaps(sortingRange), GetSkeanEhrenborgJaromczykGaps(sortingRange) }, tryoutsIterations, true)[0].gapSequence;
            if (best == results[0].gapSequence && !IsGapSequenceIn(best, alreadyFound))
            {
                alreadyFound.push_back(best);
                std::cout << "\n\nNEW CANDIDATE SEQUENCE ---------------------------- NEW CANDIDATE SEQUENCE ---------------------------- NEW CANDIDATE SEQUENCE\n\n";
                files::SaveGapsToFile(sortingRange, "cuckoo", best);
            }

            //creating new cuckoo sequences
            std::vector<GapSequence> newGapSequences;
            for (Result& r : results) newGapSequences.push_back(r.gapSequence);

            algorithmGapSequences = GetNewPopulation(sortingRange, newGapSequences, i + 1);
        }
    }
}

#endif // !CUCKOO_SEARCH_HPP