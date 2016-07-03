
#include "Camera.h"
#include "Player.h"
#include "Level.h"

const float Camera::ACCELERATION = 4.0f;

Camera::Camera(sf::Vector2f windowSize) :
	m_View(sf::FloatRect(0, 0, windowSize.x, windowSize.y))
{
}

Camera& Camera::operator=(const Camera& other)
{
	if (&other == this) return *this;

	m_View = other.GetCurrentView();
	return *this;
}

Camera::~Camera()
{
}

void Camera::Tick(sf::Time elapsed, Level* level)
{
	Player* player = level->GetPlayer();
	sf::Vector2f playerPos = player->GetPosition();

	sf::View newView(m_View);
	
	const float dx = playerPos.x - newView.getCenter().x;
	const float dy = playerPos.y - newView.getCenter().y;

	const float dt = elapsed.asSeconds();
	const float acc = ACCELERATION * dt;
	newView.move(dx * acc, dy * acc);

	BoundsCheck(newView, level);
	m_View = newView;
}

sf::View Camera::GetCurrentView() const
{
	return m_View;
}

void Camera::SetZoom(float zoom)
{
	m_View.setSize(m_View.getSize().x / zoom, m_View.getSize().y / zoom);
}

void Camera::BoundsCheck(sf::View& view, Level* level)
{
	const float levelWidth = float(level->GetWidth());
	const float levelHeight = float(level->GetHeight());

	const float halfCameraWidth = m_View.getSize().x / 2.0f;
	const float halfCameraHeight = m_View.getSize().y / 2.0f;

	if (view.getCenter().x - halfCameraWidth < 0.0f)
	{
		view.setCenter(halfCameraWidth, view.getCenter().y);
	}
	if (view.getCenter().x + halfCameraWidth > levelWidth)
	{
		view.setCenter(levelWidth - halfCameraWidth, view.getCenter().y);
	}

	if (view.getCenter().y - halfCameraHeight < 0.0f)
	{
		view.setCenter(view.getCenter().x, halfCameraHeight);
	}
	if (view.getCenter().y + halfCameraHeight > levelHeight)
	{
		view.setCenter(view.getCenter().x, levelHeight - halfCameraHeight);
	}
}
