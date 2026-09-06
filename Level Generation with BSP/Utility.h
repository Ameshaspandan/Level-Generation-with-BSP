#pragma once

#include <random>

class Random
{
public:
	Random(unsigned seed = std::random_device{}());

	int randomInt(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}
private:
	std::mt19937 rng;
};

Random::Random(unsigned seed) : rng{ seed }
{
}