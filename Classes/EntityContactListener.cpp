#pragma once

#include "EntityContactListener.h"

EntityContactListener::EntityContactListener() : _contacts()
{
}

EntityContactListener::~EntityContactListener()
{
}

void EntityContactListener::BeginContact(b2Contact* contact)
{
	EntityContact entityContact = { contact->GetFixtureA(), contact->GetFixtureB() };
	_contacts.push_back(entityContact);
}

void EntityContactListener::EndContact(b2Contact* contact)
{
	EntityContact entityContact = { contact->GetFixtureA(), contact->GetFixtureB() };
	std::vector<EntityContact>::iterator pos;
	pos = std::find(_contacts.begin(), _contacts.end(), entityContact);
	if (pos != _contacts.end())
	{
		_contacts.erase(pos);
	}
}

void EntityContactListener::PreSolve(b2Contact* contact,
	const b2Manifold* oldManifold)
{
}

void EntityContactListener::PostSolve(b2Contact* contact,
	const b2ContactImpulse* impulse)
{
}