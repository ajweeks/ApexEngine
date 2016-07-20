#pragma once

enum class StateType
{
	MAIN_MENU, GAME
};

enum class ApexCursor
{
	NORMAL, POINT,

	_LAST_ELEMENT
};

enum ActorID
{
	WALL		= (1 << 0),
	PLAYER		= (1 << 1),
	BULLET		= (1 << 2),
	GUN			= (1 << 3),
	SHEEP		= (1 << 4),
	AMMO		= (1 << 5)
};