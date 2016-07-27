#pragma once

class Interactable
{
public:
	Interactable() {};
	virtual ~Interactable() {};

	Interactable(const Interactable&) = delete;
	Interactable& operator=(const Interactable&) = delete;

	virtual void Interact() = 0;
};
