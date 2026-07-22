#ifndef SHELLSORT_HPP
#define SHELLSORT_HPP


#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "Utilis.hpp"

class GapSequence
{
    public:
    std::string name;
    std::vector<unsigned long> gaps;

    GapSequence() {}

    GapSequence(std::string name, std::vector<unsigned long> gaps) :
        name(name),
        gaps(gaps)
    {
    }

    void PrintInstance()
    {
        std::cout << name << ": ";
        for (unsigned long gap : gaps) std::cout << gap << " ";
    }

    void ValidateSequence(unsigned long sortingRange)
    {
        for (unsigned long& gap : gaps)
        {
            if (gap < 1 || gap >= sortingRange)
            {
                gap = utilis::GetRandomInt(2, static_cast<int>(sortingRange - 1));
                name += "|Validated";
            }
        }
    }

    bool operator==(const GapSequence& other) const {
        return gaps == other.gaps;
    }
};

void ShellSort(std::vector<int>& arr, std::vector<unsigned long>& gaps)
{
    for (unsigned long gap : gaps)
    {
        for (unsigned long i = gap; i < arr.size(); i++)
        {
            int temp = arr[i];
            unsigned long j;
            for (j = i; (j >= gap) && (arr[j - gap] > temp); j -= gap)
            {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

std::tuple<unsigned long, unsigned long, unsigned long> ShellSort_Stats(std::vector<int>& arr, std::vector<unsigned long>& gaps)
{
    unsigned long comparisons = 0;
    unsigned long loops = 0;
    unsigned long operations = 0;

    for (unsigned long gap : gaps)
    {
        loops++; 
        for (unsigned long i = gap; i < arr.size(); i++)
        {
            loops++;
            int temp = arr[i];
            unsigned long j = i;
            operations += 2;
            while(j >= gap)
            {
                loops++;
                comparisons++;
                if (arr[j - gap] > temp)
                {
                    arr[j] = arr[j - gap];
                    j -= gap;
                    operations += 2;
                }
                else
                {
                    break;
                }
            }
            arr[j] = temp;
            operations += 1;
        }
    }

    operations += (loops + comparisons);
    return std::make_tuple(comparisons, loops, operations);
}

// Tokuda 1992: 1, 4, 9, 20, 46, 103, 233, 525, 1182, 2660, 5985, 13467, 30301, 68178...
GapSequence GetTokudaGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> tokudaGaps;

    for (unsigned long k = 1; k < sortingRange; ++k)
    {
        unsigned long gap = (unsigned long)std::ceil((std::pow(9.0 / 4.0, k) - 1) / ((9.0 / 4.0) - 1));

        if (gap < sortingRange)
        {
            tokudaGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(tokudaGaps.begin(), tokudaGaps.end());
    return GapSequence("Tokuda", tokudaGaps);
}

// Ciura 2001: 1, 4, 10, 23, 57, 132, 301, 701 (later extension: 1750)
GapSequence GetCiuraGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> ciuraGaps;
    for (unsigned long gap : {1, 4, 10, 23, 57, 132, 301, 701 /*later extension:*/, 1750})
    {
        if (gap < sortingRange)
        {
            ciuraGaps.push_back(gap);
        }
        else break;
    }

    std::reverse(ciuraGaps.begin(), ciuraGaps.end());
    return GapSequence("Ciura", ciuraGaps);
}

// Lee 2021: 1, 4, 9, 20, 45, 102, 230, 516, 1158, 2599, 5831, 13082, 29351, 65853...
GapSequence GetLeeGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> leeGaps;

    double lambda = 2.24360906142;

    for (unsigned long k = 1; k < sortingRange; ++k)
    {
        unsigned long gap = (unsigned long)std::ceil((std::pow(lambda, k) - 1) / (lambda - 1));

        if (gap < sortingRange)
        {
            leeGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(leeGaps.begin(), leeGaps.end());
    return GapSequence("Lee", leeGaps);
}

// Skean, Ehrenborg, Jaromczyk 2023: 1, 4, 10, 27, 72, 187, 488, 1272, 3313, 8627, 22465, 58498...
GapSequence GetSkeanEhrenborgJaromczykGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> sejGaps;

    for (long k = -1; k < (long long)sortingRange; k++)
    {
        unsigned long gap = (unsigned long)std::floor(4.0816 * std::pow(8.5714, k / 2.2449));

        if (gap < sortingRange)
        {
            sejGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(sejGaps.begin(), sejGaps.end());
    return GapSequence("SEJ", sejGaps);
}

std::vector<unsigned long> GetRandomizedGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> randomizedGaps = { 1 };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(1.1f, 5.0f);

    for (unsigned long k = 1; k < sortingRange; ++k)
    {

        float x = dist(gen);
        unsigned long gap;
        if (x < 2) { gap = (unsigned long)std::ceil(randomizedGaps[k - 1] * x); }
        else { gap = (unsigned long)std::floor(randomizedGaps[k - 1] * x); }

        if (gap < sortingRange)
        {
            randomizedGaps.push_back(gap);
        }
        else
        {
            break;
        }
    }

    std::reverse(randomizedGaps.begin(), randomizedGaps.end());
    return randomizedGaps;
}

#endif // !SHELLSORT_HPP