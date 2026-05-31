#ifndef UTILIS_HPP
#define UTILIS_HPP

#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

namespace utilis
{
    float getRandomFloat(float min, float max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }

    double getRandomDouble(double min, double max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }

    int getRandomInt(int min, int max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    std::vector<int> getRandomSortingData(unsigned long sortingRange)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(-10000, 10000);

        std::vector<int> data(sortingRange);
        #pragma omp parallel for
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            data[i] = dist(gen);
        }

        return data;
    }

    double getNormalDistribution(double mean, double stddev)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dist(mean, stddev);
        return dist(gen);
    }

    int roundUpToOdd(int number)
    {
        return (number % 2 == 0) ? number + 1 : number;
    }

    int roundUpToEven(int number)
    {
        return (number % 2 != 0) ? number + 1 : number;
    }   
}

#endif // !UTILIS_HPP