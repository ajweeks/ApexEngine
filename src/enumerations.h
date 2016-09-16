#pragma once

enum class StateType
{
	LOADING, MAIN_MENU, GAME
};

enum class ApexCursor
{
	NORMAL, POINT,

	_LAST_ELEMENT
};

enum class Direction
{
	UP, DOWN, LEFT, RIGHT
};

// TODO: FIXME: XXX: Make this an std bitset? (just somehow automate indexing)
enum ActorID
{
	PLAYER		= (1 << 1),
	BULLET		= (1 << 2),
	GUN			= (1 << 3),
	SHEEP		= (1 << 4),
	AMMO		= (1 << 5),
	NPC			= (1 << 6),
	COIN		= (1 << 7),
	DOOR		= (1 << 8),
	EXIT		= (1 << 9),
	BED			= (1 << 10),
	BUILDING	= (1 << 11),
	SOLID_BLOCK = (1 << 12)
};