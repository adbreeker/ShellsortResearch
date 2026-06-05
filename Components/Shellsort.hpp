#ifndef SHELLSORT_HPP
#define SHELLSORT_HPP


#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

class GapsSequence
{
    public:
    std::string name;
    std::vector<unsigned long> gaps;

    GapsSequence() {}

    GapsSequence(std::string name, std::vector<unsigned long> gaps) :
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

    bool operator==(const GapsSequence& other) const {
        return gaps == other.gaps;
    }
};

unsigned long ShellSort(std::vector<int>& arr, std::vector<unsigned long>& gaps)
{
    unsigned long operations = 0;
    for (unsigned long gap : gaps)
    {
        operations++;
        for (unsigned long i = gap; i < arr.size(); i++)
        {
            operations++;
            int temp = arr[i];
            unsigned long j;
            for (j = i; (j >= gap) && (arr[j - gap] > temp); j -= gap)
            {
                operations++;
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
    return operations;
}

GapsSequence GetTokudaGaps(unsigned long sortingRange)
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
    return GapsSequence("Tokuda", tokudaGaps);
}

GapsSequence GetCiuraGaps(unsigned long sortingRange)
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
    return GapsSequence("Ciura", ciuraGaps);
}

GapsSequence GetLeeGaps(unsigned long sortingRange)
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
    return GapsSequence("Lee", leeGaps);
}

GapsSequence GetSkeanEhrenborgJaromczykGaps(unsigned long sortingRange)
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
    return GapsSequence("SEJ", sejGaps);
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