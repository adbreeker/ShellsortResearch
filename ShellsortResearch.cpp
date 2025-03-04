#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <math.h>
#include <future>
#include <random>
#include <string>

class GapsSequence
{
    public:
    std::string name;
    std::vector<unsigned long> gaps;

    GapsSequence(){}

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

    bool operator==(const GapsSequence& other) const {
        return name == other.name && gaps == other.gaps;
    }
};

struct Result
{
    double time;
    double operations;
    GapsSequence gapsSequence;
    int wins = 0;
};

unsigned long shellSort(std::vector<int>& arr, std::vector<unsigned long>& gaps)
{
    unsigned long operations = 0;
    for(unsigned long gap : gaps)
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

void measureSortTime(void(*sortFunc)(std::vector<int>&), std::vector<int> data, std::string name, std::promise<Result> output) 
{
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(data);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    //output.set_value(Result{ elapsed.count(), NULL, NULL });
}

void measureShellSortTime(std::vector<int> data, GapsSequence gapsSequence, std::promise<Result> output)
{
    unsigned long operations;

    auto start = std::chrono::high_resolution_clock::now();
    operations = shellSort(data, gapsSequence.gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), (double)operations, gapsSequence });
}

GapsSequence getTokudaGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> tokudaGaps;

    for (long k = 1; k < sortingRange; k++)
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

GapsSequence getCiuraGaps(unsigned long sortingRange)
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

GapsSequence getLeeGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> leeGaps;

    double lambda = 2.24360906142;

    for (long k = 1; k < sortingRange; k++)
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

GapsSequence getSkeanEhrenborgJaromczykGaps(unsigned long sortingRange)
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

std::vector<unsigned long> getRandomizedGaps(unsigned long sortingRange)
{
    std::vector<unsigned long> randomizedGaps = {1};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(1.1f, 5.0f);

    for (long k = 1; k < sortingRange; k++)
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

std::vector<Result> compareShellSorts(unsigned long sortingRange, std::vector<GapsSequence> gapsSequences, int iterations)
{
    int sortsCount = gapsSequences.size();
    std::vector<Result> avgResults(sortsCount);

    std::cout << "Compare iterations:";

    for (int i = 0; i < iterations; i++)
    {
        if (i % 50 == 0) std::cout << "\n";
        std::cout << "+";

        std::vector<int> data(sortingRange);
        for (int& num : data) num = rand() % 10000;

        // Use promises & futures for safe thread return value
        std::vector<std::promise<Result>> promises(sortsCount);
        std::vector < std::future<Result>> futures(sortsCount);
        for (int j = 0; j < sortsCount; j++) futures[j] = promises[j].get_future();

        std::vector<std::thread> threads;
        
        int cpuCores = std::thread::hardware_concurrency();


        for (int j = 0; j < sortsCount; j++)
        {
            threads.push_back(std::thread(measureShellSortTime, data, gapsSequences[j], std::move(promises[j])));
            if (j % cpuCores == cpuCores - 1)
            {
                for (int l = j - (cpuCores - 1); l <= j; l++)
                {
                    threads[l].join();
                }
            }
        }

        // Get results from threads
        std::vector<Result> results;
        for (int j = 0; j < sortsCount; j++) results.push_back(futures[j].get());

        if (i == 0)
        {
            for (int j = 0; j < sortsCount; j++)
            {
                avgResults[j] = results[j];
            }
        }
        else
        {
            for (int j = 0; j < sortsCount; j++)
            {
                avgResults[j].time += results[j].time;
                avgResults[j].operations += results[j].operations;
            }
        }

        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.operations < b.operations;
            });

        for (Result& r : avgResults) if (r.gapsSequence == results[0].gapsSequence) { r.wins++; }
    }

    for (Result& r : avgResults)
    {
        r.time = r.time / iterations;
        r.operations = r.operations / iterations;
    }

    // Sort results to find the fastest
    std::sort(avgResults.begin(), avgResults.end(), [](const Result& a, const Result& b) {
        return a.operations < b.operations;
        });

    return avgResults;
}

std::vector<Result> compareShellWith_stdSort_Ciura_SEJ(std::pair<unsigned long, unsigned long> sortingRange, GapsSequence gapsSequence, int iterations)
{
    std::vector<Result> avgResults(4);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> dist(sortingRange.first, sortingRange.second);

    std::cout << "Compare iterations:";

    for (int i = 0; i < iterations; i++)
    {
        if (i % 50 == 0) std::cout << "\n";
        std::cout << "+";

        unsigned long toSortSize = dist(gen);

        std::vector<int> data(toSortSize);
        for (int& num : data) num = rand() % 10000;

        GapsSequence gapsCiura = getCiuraGaps(toSortSize);
        GapsSequence gapsSEJ = getSkeanEhrenborgJaromczykGaps(toSortSize);

        // Use promises & futures for safe thread return value
        std::promise<Result> pCompare, pStdSort, pCiura, pSEJ;
        std::future<Result> fCompare = pCompare.get_future();
        std::future<Result> fStdSort = pStdSort.get_future();
        std::future<Result> fCiura = pCiura.get_future();
        std::future<Result> fSEJ = pSEJ.get_future();

        std::vector<std::thread> threads;

        std::thread tCompare(measureShellSortTime, data, gapsSequence, std::move(pCompare));
        std::thread tStdSort(measureSortTime, [](std::vector<int>& d) {std::sort(d.begin(), d.end());}, data, "stdSort", std::move(pStdSort));
        std::thread tCiura(measureShellSortTime, data, gapsCiura, std::move(pCiura));
        std::thread tSEJ(measureShellSortTime, data, gapsSEJ, std::move(pSEJ));

        tCompare.join();
        tStdSort.join();
        tCiura.join();
        tSEJ.join();

        std::vector<Result> results;
        results.push_back(fCompare.get());
        results.push_back(fStdSort.get());
        results.push_back(fCiura.get());
        results.push_back(fSEJ.get());

        // Get results from threads
        if (i == 0)
        {
            avgResults[0] = results[0];
            avgResults[1] = results[1];
            avgResults[2] = results[2];
            avgResults[3] = results[3];
        }
        else
        {
            avgResults[0].time += results[0].time;
            avgResults[1].time += results[1].time;
            avgResults[2].time += results[2].time;
            avgResults[3].time += results[3].time;
        }

        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.time < b.time;
            });

        for (Result& r : avgResults) if (r.gapsSequence == results[0].gapsSequence) { r.wins++; }
    }

    for (Result& r : avgResults) r.time = r.time / iterations;

    // Sort results to find the fastest
    std::sort(avgResults.begin(), avgResults.end(), [](const Result& a, const Result& b) {
        return a.time < b.time;
        });

    return avgResults;
}

std::vector<GapsSequence> geneticAlgorithmCrossParents(std::vector<GapsSequence> parents)
{
    std::vector<GapsSequence> childs;

    for (int i = 0; i < parents.size(); i+=2)
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

        childs.push_back(GapsSequence("Child" + parent1.name + parent2.name, child1Gaps));
        childs.push_back(GapsSequence("Child" + parent2.name + parent1.name, child2Gaps));
    }

    return childs;
}

std::vector<Result> geneticAlgorithmForGapsSeeking(unsigned long sortingRange, std::vector<GapsSequence> algorithmGapsSequences, int tryoutsIterations, int geneticIterations)
{   
    std::vector<Result> results;
    bool fasterFound = false;

    for (int i = 0; i < geneticIterations ; i++)
    {
        std::cout << "\n\nGenetic iteration " << i << ":\n";
        std::cout << "Gaps:\n";
        for (GapsSequence sequence : algorithmGapsSequences)
        {
            sequence.PrintInstance();
            std::cout << "\n";
        }
        std:: cout << "\n";

        std::cout << "Genetic generated gaps ";
        results = compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);

        std::vector<GapsSequence> newGapsSequences;

        for (int r = 0; r < results.size() / 2; r++) { newGapsSequences.push_back(results[r].gapsSequence); }

        std::vector<GapsSequence> childs = geneticAlgorithmCrossParents(newGapsSequences);

        //newGapsSequences.insert(newGapsSequences.end(), childs.begin(), childs.end());
        newGapsSequences = childs;

        for (int g = newGapsSequences.size(); g < algorithmGapsSequences.size(); g++)
        {
            newGapsSequences.push_back(GapsSequence(
                "Random" + std::to_string(i) + "-" + std::to_string(g), 
                getRandomizedGaps(sortingRange)));
        }

        algorithmGapsSequences = newGapsSequences;
    }

    return compareShellSorts(sortingRange, algorithmGapsSequences, tryoutsIterations);
}

int main() 
{
    std::vector<Result> results;

    const unsigned long SORTING_RANGE = 1000;

    GapsSequence tokudaGaps = getTokudaGaps(SORTING_RANGE);
    GapsSequence ciuraGaps = getCiuraGaps(SORTING_RANGE);
    GapsSequence leeGaps = getLeeGaps(SORTING_RANGE);
    GapsSequence sejGaps = getSkeanEhrenborgJaromczykGaps(SORTING_RANGE);

    std::vector<GapsSequence> randomizedGapsSequences = { tokudaGaps, ciuraGaps, leeGaps, sejGaps};
    for (int i = randomizedGapsSequences.size(); i<60; i++) randomizedGapsSequences.push_back(GapsSequence("Random" + std::to_string(i), getRandomizedGaps(SORTING_RANGE)));

    results = geneticAlgorithmForGapsSeeking(SORTING_RANGE, randomizedGapsSequences, 50, 10);

    //results = compareShellSorts(SORTING_RANGE, randomizedGapsSequences, 100);

    std::cout << "\n\nResults:\n";
    for (Result r : results)
    {
        r.gapsSequence.PrintInstance();
        std::cout << "\n  Time: " << r.time << "ms | Operations: " << r.operations << " | Wins: " << r.wins << "\n\n";
    }
}