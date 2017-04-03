#pragma once

enum class StateType
{
	LOADING, MAIN_MENU, GAME
};

// TODO: FIXME: XXX: Make this an std bitset? (just somehow automate indexing)
enum class ActorID
{
	PLAYER = (1 << 1),
	BULLET = (1 << 2),
	GUN = (1 << 3),
	SHEEP = (1 << 4),
	AMMO = (1 << 5),
	AI_CHARACTER = (1 << 6),
	COIN = (1 << 7),
	DOOR = (1 << 8),
	EXIT = (1 << 9),
	BED = (1 << 10),
	BUILDING = (1 << 11),
	SOLID_BLOCK = (1 << 12)
};

enum class Sound
{
	GUN_FIRE, GUN_FIRE_EMPTY, GUN_RELOAD,
	BOOP, BLIP,
	TYPING_1, TYPING_2, TYPING_3,
	COIN_PICKUP,

	_LAST_ELEMENT
};
enum class Music
{
	A, // There needs to be at least one element in here to compile
	_LAST_ELEMENT
};

enum Texture
{
	LIQWIDICE_GAMES_LOGO_SCREEN,
	GENERAL_TILES,
	DUST_PARTICLE,
	PLAYER,
	SHADOW,
	CURSOR_NORMAL, CURSOR_POINTER,
	AMMO, COIN,
	TITLE_COLOR,
	SPEECH_BUBBLE,
	AI_CHARACTER,
	BUILDINGS,

	_LAST_ELEMENT
};

enum class Key
{
	MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_RIGHT,
	PAUSE, INTERACT, SCREENSHOT,
	DEBUG_TOGGLE_INFO_OVERLAY, DEBUG_TOGGLE_LIGHT_EDITOR, DEBUG_RESTART,
	DEBUG_TOGGLE_PHYSICS_OVERLAY, DEBUG_STEP_ONE_PHYSICS_FRAME, DEBUG_PAUSE_EVERYTHING,

	NONE
};
