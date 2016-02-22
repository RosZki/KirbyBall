#pragma once

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "EntityContactListener.h"

class GameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	
	void tick(float dt);
	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);

private:
	b2World* _world;
	b2Body *_groundBody;
	b2Fixture *_bottomFixture;
	b2Fixture *_ballFixture;

	b2Body *_paddleBody;
	b2Fixture *_paddleFixture;
	b2MouseJoint *_mouseJoint;

	EntityContactListener *_contactListener;
};
