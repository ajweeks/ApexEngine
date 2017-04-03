#pragma once

#include "Apex\WindowListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\View.hpp>

class World;

class Camera : public apex::WindowListener
{
public:
	Camera(sf::Vector2f windowSize, World& world);
	Camera& operator=(const Camera&);
	virtual ~Camera();

	Camera(const Camera&) = delete;

	void Tick(sf::Time elapsed);

	sf::View GetCurrentView() const;
	// NOTE: This only effects the scale that the world renders at, (not the GUI)
	void SetZoom(float zoom);

	void SnapToPlayer();
	void Jolt(float xScale, float yScale);
	void Shake(float xScale, float yScale);

	virtual void OnWindowResize(sf::Vector2u windowSize) override;

private:
	void BoundsCheck(sf::View& view);

	static const float DEFAULT_ZOOM;
	static const float ACCELERATION;

	World& m_World;

	sf::View m_View;
	sf::Vector2f m_ShakeRadius;
	sf::Vector2f m_ShakeOffset;
	float m_CurrentZoom;

};
