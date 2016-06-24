
#include "BulletManager.h"
#include "Bullet.h"

BulletManager::BulletManager()
{
}

BulletManager::~BulletManager()
{
	for (size_t i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i] != nullptr)
		{
			delete m_Bullets[i];
		}
	}
}

void BulletManager::Reset()
{
	for (size_t i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i] != nullptr)
		{
			delete m_Bullets[i];
		}
	}
	m_Bullets.clear();
}

void BulletManager::Tick(sf::Time elapsed)
{
	for (size_t i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i] != nullptr)
		{
			m_Bullets[i]->Tick(elapsed);
		}
	}
}

void BulletManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	for (size_t i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i] != nullptr)
		{
			m_Bullets[i]->Draw(target, states);
		}
	}
}

void BulletManager::AddBullet(Bullet* bullet)
{
	for (size_t i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i] == nullptr)
		{
			m_Bullets[i] = bullet;
			return;
		}
	}
	
	m_Bullets.push_back(bullet);
}

void BulletManager::RemoveBullet(Bullet* bullet)
{
	for (size_t i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i] == bullet)
		{
			delete m_Bullets[i];
			m_Bullets[i] = nullptr;
			break;
		}
	}
}
