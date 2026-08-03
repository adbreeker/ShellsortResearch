#ifndef UTILIS_HPP
#define UTILIS_HPP

#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

namespace utilis
{
    float GetRandomFloat(float min, float max)
    {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }

    double GetRandomDouble(double min, double max)
    {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }

    int GetRandomInt(int min, int max)
    {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    std::vector<int> GetRandomSortingData(unsigned long sortingRange)
    {
        std::vector<int> data(sortingRange);

        #pragma omp parallel
        {
            std::random_device rd;
            std::mt19937 gen(rd() ^ static_cast<unsigned int>(omp_get_thread_num()));
            std::uniform_int_distribution<int> dist(-10000, 10000);

            #pragma omp for
            for (std::size_t i = 0; i < data.size(); ++i)
            {
                data[i] = dist(gen);
            }
        }

        return data;
    }

    double GetNormalDistribution(double mean, double stddev)
    {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 gen(rd());
        std::normal_distribution<double> dist(mean, stddev);
        return dist(gen);
    }

    int RoundUpToOdd(int number)
    {
        return (number % 2 == 0) ? number + 1 : number;
    }

    int RoundUpToEven(int number)
    {
        return (number % 2 != 0) ? number + 1 : number;
    }   

    std::vector<std::string> SplitString(std::string toSplit, const std::string& spliter)
    {
        std::vector<std::string> tokens;
        size_t start = 0, end;

        while ((end = toSplit.find(spliter, start)) != std::string::npos)
        {
            std::string sub = toSplit.substr(start, end - start);
            if (sub.length() > 0) tokens.push_back(sub);
            start = end + spliter.length();
        }
        std::string sub = toSplit.substr(start);
        if (sub.length() > 0) tokens.push_back(sub); // Last part after the final delimiter
        return tokens;
    }
}

#endif // !UTILIS_HPP