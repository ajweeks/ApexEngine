#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\View.hpp>

class Level;

class Camera
{
public:
	Camera(sf::Vector2f windowSize);
	Camera& operator=(const Camera&);
	virtual ~Camera();

	Camera(const Camera&) = delete;

	void Tick(sf::Time elapsed, Level* level);

	sf::View GetCurrentView() const;
	// NOTE: This only effects the scale that the level renders at, (not the GUI)
	void SetZoom(float zoom);

	void Jolt(float xScale, float yScale);
	void Shake(float xScale, float yScale);

private:
	void BoundsCheck(sf::View& view, Level* level);

	static const float DEFAULT_ZOOM;
	static const float ACCELERATION;

	sf::View m_View;
	sf::Vector2f m_ShakeRadius;
	sf::Vector2f m_ShakeOffset;

};
