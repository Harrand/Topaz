#include "utility.hpp"

Random::Random(std::default_random_engine::result_type seed): seed(seed)
{
	this->random_engine.seed(this->seed);
}

Random::Random(const Random& copy): Random(copy.seed){}

const std::default_random_engine::result_type& Random::getSeed() const
{
	return this->seed;
}

const std::default_random_engine& Random::getEngine() const
{
	return this->random_engine;
}

std::default_random_engine& Random::getEngineR()
{
	return this->random_engine;
}

int Random::nextInt(int min, int max)
{
	return std::uniform_int_distribution<>(min, max)(this->random_engine);
}

float Random::nextFloat(float min, float max)
{
	return std::uniform_real_distribution<>(min, max)(this->random_engine);
}

MersenneTwister::MersenneTwister(std::mt19937::result_type seed): seed(seed)
{
	this->mersenne_twister_engine.seed(this->seed);
}

MersenneTwister::MersenneTwister(const MersenneTwister& copy): MersenneTwister(copy.seed){}

const std::mt19937::result_type& MersenneTwister::getSeed() const
{
	return this->seed;
}
const std::mt19937& MersenneTwister::getEngine() const
{
	return this->mersenne_twister_engine;
}
std::mt19937& MersenneTwister::getEngineR()
{
	return this->mersenne_twister_engine;
}

int MersenneTwister::nextInt(int min, int max)
{
	return std::uniform_int_distribution<>(min,max)(this->mersenne_twister_engine);
}

float MersenneTwister::nextFloat(float min, float max)
{
	return std::uniform_real_distribution<>(min, max)(this->mersenne_twister_engine);
}