#pragma once

#include <Box2D/Box2D.h>
#include <vector>

struct EntityContact {
	b2Fixture *fixtureA;
	b2Fixture *fixtureB;
	bool operator==(const EntityContact& other) const
	{
		return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
	}
};


class EntityContactListener : public b2ContactListener {
public:
	std::vector<EntityContact> _contacts;

	EntityContactListener();
	~EntityContactListener();

	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

private:
};
