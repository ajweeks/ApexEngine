#pragma once

#include "Physics\PhysicsActor.h"

#include <SFML\System\Vector2.hpp>

#include <random>

namespace apex
{
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

		static float DistSqr(sf::Vector2f pointA, sf::Vector2f pointB)
		{
			sf::Vector2f vecC = pointA - pointB;
			return Dot(vecC, vecC);
		}

		static float Distance(sf::Vector2f a, sf::Vector2f b)
		{
			return std::sqrt(DistSqr(a, b));
		}

		static float Length(sf::Vector2f v)
		{
			return std::sqrt(LengthSqr(v));
		}

		static float LengthSqr(sf::Vector2f v)
		{
			return v.x * v.x + v.y * v.y;
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

		static double GetFloatPrecision(float value, float precision)
		{
			return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
		}

		static sf::Vector2f b2Vec2ToSFVec2fS(b2Vec2 vec)
		{
			return sf::Vector2f(vec.x * PhysicsActor::SCALE, vec.y * PhysicsActor::SCALE);
		}

		static sf::Vector2f b2Vec2ToSFVec2fSS(b2Vec2 vec)
		{
			return sf::Vector2f(vec.x, vec.y);
		}

		static b2Vec2 SFVec2fToB2Vec2S(sf::Vector2f vec)
		{
			return b2Vec2(vec.x / PhysicsActor::SCALE, vec.y / PhysicsActor::SCALE);
		}

		static b2Vec2 SFVec2fToB2Vec2(sf::Vector2f vec)
		{
			return b2Vec2(vec.x, vec.y);
		}

	};
} // namespace apex