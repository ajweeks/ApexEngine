
#include "LoadingState.h"
#include "ApexMain.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexAudio.h"
#include "GameState.h"
#include "TextureManager.h"
#include "MainMenuState.h"
#include "ApexMath.h"

LoadingState::LoadingState()
	: BaseState(StateType::LOADING)
{
	m_LogoSprite.setTexture(*TextureManager::GetTexture(TextureManager::LIQWIDICE_GAMES_LOGO_SCREEN));
	
	TransitionData start;
	start.color = sf::Color::Black;
	TransitionData end;
	end.color = sf::Color::White;
	m_FadeTransition.Create(start, end, sf::seconds(0.75f), ApexTransition::EaseType::QUADRATIC_OUT);

	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const sf::Vector2u imageSize = m_LogoSprite.getTexture()->getSize();
	m_LogoSprite.setScale(windowSize.x / float(imageSize.x), windowSize.y / float(imageSize.y));

	m_BufferTime = sf::seconds(0.4f);
	m_TimeToShowLogo = sf::seconds(2.5f);

	m_LogoSprite.setColor(sf::Color::Black);

	m_AnimState = AnimationState::INITAL_BLACK;

	m_SpriteFlicker = 1.0f;
}

LoadingState::~LoadingState()
{
}

void LoadingState::Tick(sf::Time elapsed)
{
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const sf::Vector2i mousePos = APEX->GetMouseCoordsScreenSpace();
	m_LogoSprite.setPosition(mousePos.x / 50.0f , mousePos.y / 50.0f);
	
	switch (m_AnimState)
	{
	case AnimationState::INITAL_BLACK:
	{
		if (m_BufferTime > sf::Time::Zero)
		{
			m_BufferTime -= elapsed;
			return;
		}

		m_AnimState = AnimationState::FADE_IN;
	} break;
	case AnimationState::FADE_IN:
	{
		m_FadeTransition.Tick(elapsed);
		if (m_FadeTransition.GetPercentComplete() >= 1.0f)
		{
			m_AnimState = AnimationState::HOLD;
			m_SpriteFlicker = 0.95f;
		}
		else
		{
			m_SpriteFlicker = (std::rand() % 50) / 100.0f + 0.5f;
		}
	} break;
	case AnimationState::HOLD:
	{
		m_TimeToShowLogo -= elapsed;
		if (m_TimeToShowLogo <= sf::Time::Zero)
		{
			m_FadeTransition.Swap();
			m_AnimState = AnimationState::FADE_OUT;
		}
		else
		{
			if (m_SpriteFlicker > 0.95f)
			{
				m_SpriteFlicker += ApexMath::Random(-0.05f, -0.01f);
			}
			else if (m_SpriteFlicker < 0.5f)
			{
				m_SpriteFlicker += ApexMath::Random(0.01f, 0.05f);
			}
			else
			{
				m_SpriteFlicker += ApexMath::Random(0.02f, 0.04f) * (std::rand() % 2 == 0 ? 1 : -1);
			}
		}
	} break;
	case AnimationState::FADE_OUT:
	{
		m_FadeTransition.Tick(elapsed);
		if (m_FadeTransition.GetPercentComplete() >= 1.0f)
		{
			m_AnimState = AnimationState::FINAL_BLACK;
			m_BufferTime = sf::seconds(0.4f);
		}
		else
		{
			m_SpriteFlicker = (std::rand() % 50) / 100.0f + 0.5f;
		}
	} break;
	case AnimationState::FINAL_BLACK:
	{
		m_BufferTime -= elapsed;
		if (m_BufferTime < sf::Time::Zero)
		{
			APEX->GetStateManager()->SetState(new MainMenuState());
			return;
		}
	} break;
	}

	sf::Color spriteColor = m_FadeTransition.GetCurrentTransitionData().color;
	spriteColor.a = sf::Uint8(m_SpriteFlicker * 127 + 128);
	m_LogoSprite.setColor(spriteColor);
}

void LoadingState::Draw(sf::RenderTarget& target)
{
	target.draw(m_LogoSprite);
}

bool LoadingState::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	switch (key)
	{
	case ApexKeyboard::PAUSE:
	case ApexKeyboard::INTERACT:
		m_AnimState = AnimationState::FINAL_BLACK;
		m_BufferTime = sf::Time::Zero;
		break;
	}
	return false;
}

void LoadingState::OnKeyRelease(ApexKeyboard::Key key)
{
}
