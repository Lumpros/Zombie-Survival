#include "RandomNumberGenerator.h"

NODISCARD int PZS::RandomNumberGenerator::RandInt(int min, int max)
{
	static std::random_device device;
	static std::mt19937 generator(device());

	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

NODISCARD double PZS::RandomNumberGenerator::RandDouble(double min, double max)
{
	static std::random_device device;
	static std::mt19937 generator(device());

	std::uniform_real_distribution<double> distribution(min, max);

	return distribution(generator);
}
