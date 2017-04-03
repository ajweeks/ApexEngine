#pragma once

#include "Apex\Physics\Contact.h"

namespace apex
{
	class ContactListener
	{
	public:
		ContactListener();
		virtual ~ContactListener();

		ContactListener(const ContactListener&) = delete;
		ContactListener& operator=(const ContactListener&) = delete;

		virtual void BeginContact(Contact* contact) {};
		virtual void EndContact(Contact* contact) {};
		virtual void PreSolve(Contact* contact, bool& enableContact) {};

	private:

	};
} // namespace apex
