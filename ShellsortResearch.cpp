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
    std::vector<long> gapsUsed;
    int wins = 0;
};

void shellSort(std::vector<int>& arr, std::vector<long>& gaps)
{
    for(int gap : gaps)
    {
        for (int i = gap; i < arr.size(); i++)
        {
            int temp = arr[i];
            int j;
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

void measureShellSortTime(std::vector<int> data, std::vector<long> gaps, std::string name, std::promise<Result> output) 
{
    auto start = std::chrono::high_resolution_clock::now();
    shellSort(data, gaps);
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = stop - start;
    output.set_value(Result{ elapsed.count(), name, gaps });
}

std::vector<long> getTokudaGaps(long sortingRange)
{
    std::vector<long> tokudaGaps;

    for (long k = 1; k < sortingRange; k++)
    {
        long gap = (long)std::ceil((std::pow(9.0 / 4.0, k) - 1) / ((9.0 / 4.0) - 1));

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

std::vector<long> getCiuraGaps(long sortingRange)
{
    std::vector<long> ciuraGaps;
    for (long gap : {1, 4, 10, 23, 57, 132, 301, 701 /*later extension:*/, 1750})
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

std::vector<long> getLeeGaps(long sortingRange)
{
    std::vector<long> leeGaps;

    double lambda = 2.24360906142;

    for (long k = 1; k < sortingRange; k++)
    {
        long gap = (long)std::ceil((std::pow(lambda, k) - 1) / (lambda - 1));

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

std::vector<long> getSkeanEhrenborgJaromczykGaps(long sortingRange)
{
    std::vector<long> sejGaps;

    for (long k = -1; k < sortingRange; k++)
    {
        long gap = (long)std::floor(4.0816 * std::pow(8.5714, k / 2.2449));

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

std::vector<long> getRandomizedGaps(long sortingRange)
{
    std::vector<long> randomizedGaps = {1};

    std::random_device rd;
    std::mt19937 gen(rd());

    if (rand() % 2 == 0)
    {
        std::uniform_real_distribution<float> dist(1.0001f, 5.0f);

        for (long k = 1; k < sortingRange; k++)
        {

            float x = dist(gen);
            long gap = (long)std::ceil(randomizedGaps[k-1] * x);

            if (gap < sortingRange)
            {
                randomizedGaps.push_back(gap);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        std::uniform_real_distribution<float> dist(2.0f, 5.0f);

        for (long k = 1; k < sortingRange; k++)
        {

            float x = dist(gen);
            long gap = (long)std::floor(randomizedGaps[k - 1] * x);

            if (gap < sortingRange)
            {
                randomizedGaps.push_back(gap);
            }
            else
            {
                break;
            }
        }
    }

    std::reverse(randomizedGaps.begin(), randomizedGaps.end());
    return randomizedGaps;
}

std::vector<Result> compareShellSorts(long sortingRange, std::vector<std::vector<long>> gapSequences, int iterations)
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
                for (int l = j - 3; l <= j; l++)
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

std::vector<Result> compareShellWith_stdSort_Ciura_SEJ(long sortingRange, std::vector<long> gapSequence, int iterations)
{
    std::vector<Result> avgResults(4);

    std::vector<long> gapsCiura = getCiuraGaps(sortingRange);
    std::vector<long> gapsSEJ = getSkeanEhrenborgJaromczykGaps(sortingRange);

    std::cout << "Compare iterations:";

    for (int i = 0; i < iterations; i++)
    {
        if (i % 50 == 0) std::cout << "\n";
        std::cout << "+";

        std::vector<int> data(sortingRange);
        for (int& num : data) num = rand() % 10000;

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

int main() 
{
    const long SORTING_RANGE = 5000;

    std::vector<long> tokudaGaps = getTokudaGaps(SORTING_RANGE);
    std::vector<long> ciuraGaps = getCiuraGaps(SORTING_RANGE);
    std::vector<long> leeGaps = getLeeGaps(SORTING_RANGE);
    std::vector<long> sejGaps = getSkeanEhrenborgJaromczykGaps(SORTING_RANGE);

    /*std::vector<std::vector<long>> randomizedGaps = { tokudaGaps, ciuraGaps, leeGaps, sejGaps};
    for (int i = 0; i<96; i++) randomizedGaps.push_back(getRandomizedGaps(SORTING_RANGE));

    std::vector<Result> results = compareShellSorts(SORTING_RANGE ,randomizedGaps, 100);*/

    std::vector<Result> results = compareShellWith_stdSort_Ciura_SEJ(
        SORTING_RANGE,
        std::vector<long> {3219, 716, 196, 90, 19, 4, 1},
        1000);

    std::cout << "\n\n    Results:\n";

    for (Result r : results)
    {
        std::cout << r.name << ": " << r.time << "ms | " << r.wins << "w\nGaps: ";
        for (long gap : r.gapsUsed) std::cout << gap << " ";
        std::cout << "\n\n";
    }
}