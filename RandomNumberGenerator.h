#pragma once

#include <random>

#define NODISCARD [[nodiscard]]

namespace PZS
{
	typedef class RandomNumberGenerator
	{
	private:
		RandomNumberGenerator(void) {}

	public:
		static NODISCARD int RandInt(int min, int max);

		static NODISCARD double RandDouble(double min, double max);
	} RNG;
}



