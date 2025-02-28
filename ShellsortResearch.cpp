#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <math.h>
#include <future>
#include <random>
#include <string>

struct Result
{
    double time;
    std::string name;
    std::vector<unsigned long> gapsUsed;
    int wins = 0;
};

void shellSort(std::vector<int>& arr, std::vector<unsigned long>& gaps)
{
    for(unsigned long gap : gaps)
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

void measureSortTime(void(*sortFunc)(std::vector<int>&), std::vector<int> data, std::string name, std::promise<Result> output) 
{
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(data);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), name });
}

void measureShellSortTime(std::vector<int> data, std::vector<unsigned long> gaps, std::string name, std::promise<Result> output)
{
    auto start = std::chrono::high_resolution_clock::now();
    shellSort(data, gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), name, gaps });
}

std::vector<unsigned long> getTokudaGaps(unsigned long sortingRange)
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
    return tokudaGaps;
}

std::vector<unsigned long> getCiuraGaps(unsigned long sortingRange)
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
    return ciuraGaps;
}

std::vector<unsigned long> getLeeGaps(unsigned long sortingRange)
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
    return leeGaps;
}

std::vector<unsigned long> getSkeanEhrenborgJaromczykGaps(unsigned long sortingRange)
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
    return sejGaps;
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

std::vector<Result> compareShellSorts(unsigned long sortingRange, std::vector<std::vector<unsigned long>> gapSequences, int iterations)
{
    int sortsCount = gapSequences.size();
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
            threads.push_back(std::thread(measureShellSortTime, data, gapSequences[j], "Random" + std::to_string(j), std::move(promises[j])));
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
            }
        }

        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.time < b.time;
            });

        for (Result& r : avgResults) if (r.name == results[0].name) { r.wins++; }
    }

    for (Result& r : avgResults) r.time = r.time / iterations;

    // Sort results to find the fastest
    std::sort(avgResults.begin(), avgResults.end(), [](const Result& a, const Result& b) {
        return a.time < b.time;
        });

    return avgResults;
}

std::vector<Result> compareShellWith_stdSort_Ciura_SEJ(std::pair<unsigned long, unsigned long> sortingRange, std::vector<unsigned long> gapSequence, int iterations)
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

        std::vector<unsigned long> gapsCiura = getCiuraGaps(toSortSize);
        std::vector<unsigned long> gapsSEJ = getSkeanEhrenborgJaromczykGaps(toSortSize);

        // Use promises & futures for safe thread return value
        std::promise<Result> pCompare, pStdSort, pCiura, pSEJ;
        std::future<Result> fCompare = pCompare.get_future();
        std::future<Result> fStdSort = pStdSort.get_future();
        std::future<Result> fCiura = pCiura.get_future();
        std::future<Result> fSEJ = pSEJ.get_future();

        std::vector<std::thread> threads;

        std::thread tCompare(measureShellSortTime, data, gapSequence, "Compared", std::move(pCompare));
        std::thread tStdSort(measureSortTime, [](std::vector<int>& d) {std::sort(d.begin(), d.end());}, data, "stdSort", std::move(pStdSort));
        std::thread tCiura(measureShellSortTime, data, gapsCiura, "Ciura", std::move(pCiura));
        std::thread tSEJ(measureShellSortTime, data, gapsSEJ, "SEJ", std::move(pSEJ));

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

        for (Result& r : avgResults) if (r.name == results[0].name) { r.wins++; }
    }

    for (Result& r : avgResults) r.time = r.time / iterations;

    // Sort results to find the fastest
    std::sort(avgResults.begin(), avgResults.end(), [](const Result& a, const Result& b) {
        return a.time < b.time;
        });

    return avgResults;
}

std::vector<std::vector<unsigned long>> geneticAlgorithmCrossParents(std::vector<std::vector<unsigned long>> parents)
{
    std::vector<std::vector<unsigned long>> childs;

    for (int i = 0; i < parents.size(); i+=2)
    {
        std::vector<unsigned long> parent1 = parents[i];
        std::vector<unsigned long> parent2;
        if (i + 1 < parents.size()) { parent2 = parents[i + 1]; }
        else { parent2 = parents[0]; }

        std::vector<unsigned long> child1(parent1.begin(), parent1.begin() + parent1.size() / 2);
        for (unsigned long gap : parent2)
        {
            if (gap < child1.back()) { child1.push_back(gap); }
        }

        std::vector<unsigned long> child2(parent2.begin(), parent2.begin() + parent2.size() / 2);
        for (unsigned long gap : parent1)
        {
            if (gap < child2.back()) { child2.push_back(gap); }
        }

        childs.push_back(child1);
        childs.push_back(child2);
    }

    return childs;
}

std::vector<unsigned long> geneticAlgorithmForGapsSeeking(unsigned long sortingRange, std::vector<std::vector<unsigned long>> startGapsSequences, int tryoutsIterations, int geneticIterations)
{   
    std::vector<Result> results;
    bool fasterFound = false;

    for (int i = 0; !fasterFound ; i++)
    {
        std::cout << "\n\nGenetic iteration " << i << ":\n";
        std::cout << "Gaps:\n";
        for (std::vector<unsigned long> sequence : startGapsSequences)
        {
            for (unsigned long gap : sequence)
            {
                std::cout << gap << " ";
            }
            std::cout << "\n";
        }
        std:: cout << "\n";

        if (i >= geneticIterations)
        {
            std::cout << "Checking top 3: \n";

            for (int w = 0; w < 3; w++)
            {
                std::cout << w + 1 << ":   ";
                for (unsigned long gap : startGapsSequences[w]) std::cout << gap << " ";
                std::cout << "  ";

                std::vector<Result> checkTop = compareShellWith_stdSort_Ciura_SEJ(
                    { sortingRange, sortingRange },
                    startGapsSequences[w],
                    tryoutsIterations);

                if (checkTop[3].gapsUsed != startGapsSequences[w] 
                    && !(startGapsSequences[w] == getCiuraGaps(sortingRange) 
                        || startGapsSequences[w] == getSkeanEhrenborgJaromczykGaps(sortingRange)))
                {
                    return startGapsSequences[w];
                }

                std::cout << "\n";
            }

            std::cout << "\n\n";
        }

        std::cout << "Genetic generated gaps ";
        results = compareShellSorts(sortingRange, startGapsSequences, tryoutsIterations);

        std::vector<std::vector<unsigned long>> newGapsSequences;

        for (int r = 0; r < results.size() / 4; r++) { newGapsSequences.push_back(results[r].gapsUsed); }

        std::vector<std::vector<unsigned long>> childs = geneticAlgorithmCrossParents(newGapsSequences);

        newGapsSequences.insert(newGapsSequences.end(), childs.begin(), childs.end());

        for (int g = newGapsSequences.size(); g < startGapsSequences.size(); g++)
        {
            newGapsSequences.push_back(getRandomizedGaps(sortingRange));
        }

        startGapsSequences = newGapsSequences;
    }
}

int main() 
{
    std::vector<Result> results;

    const unsigned long SORTING_RANGE = 1000;

    std::vector<unsigned long> tokudaGaps = getTokudaGaps(SORTING_RANGE);
    std::vector<unsigned long> ciuraGaps = getCiuraGaps(SORTING_RANGE);
    std::vector<unsigned long> leeGaps = getLeeGaps(SORTING_RANGE);
    std::vector<unsigned long> sejGaps = getSkeanEhrenborgJaromczykGaps(SORTING_RANGE);

    std::vector<std::vector<unsigned long>> randomizedGaps = { tokudaGaps, ciuraGaps, leeGaps, sejGaps};
    for (int i = randomizedGaps.size(); i<100; i++) randomizedGaps.push_back(getRandomizedGaps(SORTING_RANGE));

    std::vector<unsigned long> geneticResultSequence = geneticAlgorithmForGapsSeeking(SORTING_RANGE, randomizedGaps, 100, 10);

    std::cout << "\n\n    COMPARING FINAL SEQUENCEs\n\n";
    results = compareShellWith_stdSort_Ciura_SEJ(
        { SORTING_RANGE, SORTING_RANGE },
        geneticResultSequence,
        100);

    std::cout << "\n\nResults:\n";
    for (Result r : results)
    {
        std::cout << r.name << ": " << r.time << "ms | " << r.wins << "w\nGaps: ";
        for (unsigned long gap : r.gapsUsed) std::cout << gap << " ";
        std::cout << "\n\n";
    }
}