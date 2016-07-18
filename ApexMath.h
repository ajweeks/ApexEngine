#pragma once

#include <SFML\System\Vector2.hpp>

#include <cmath>
#include <random>

#define PI 3.14159265359f
#define PI_2 (3.14159265359f / 2.0f)

class ApexMath
{
public:
	static sf::Vector2f Normalize(sf::Vector2f v)
	{
		const float length = std::hypot(v.x, v.y);
		if (length != 0) v /= length;
		return v;
	}

	static float Dot(sf::Vector2f a, sf::Vector2f b)
	{
		return a.x * b.x + a.y * b.y;
	}

	static float Cross(sf::Vector2f a, sf::Vector2f b)
	{
		return a.x * b.y - a.y * b.x;
	}

	static float Length(sf::Vector2f v)
	{
		return std::sqrt(LengthSqr(v));
	}

	static float LengthSqr(sf::Vector2f v)
	{
		return v.x * v.x + v.y * v.y;
	}
	static float DistSqr(sf::Vector2f pointA, sf::Vector2f pointB)
	{
		sf::Vector2f vecC = pointA - pointB;
		return Dot(vecC, vecC);
	}

	static sf::Vector2f Cross(float a, sf::Vector2f v)
	{
		return sf::Vector2f(-a * v.y, a * v.x);
	}

	static sf::Vector2f PerpendicularVector(const sf::Vector2f& vector)
	{
		return sf::Vector2f(-vector.y, vector.x);
	}

	static float Random(float min, float max)
	{
		assert(max > min);

		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(generator);

		// Pre-C++11:
		//return (max - min) * ((float)std::rand() / (float)RAND_MAX) + min;
	}
};