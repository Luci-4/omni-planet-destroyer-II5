#pragma once
#include <random>

class Randoms
{
public:
    Randoms() = delete;

    static bool choice()
    {
        static std::bernoulli_distribution dist(0.5f);
        return dist(rng) == 0;
    }

private:
    inline static std::mt19937 rng{std::random_device{}()};
};

