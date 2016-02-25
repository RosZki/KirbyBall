#pragma once

#include "cocos2d.h"

class GameFinishScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createGameOverScene();
	static cocos2d::Scene* createWinScene();

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(GameFinishScene);

};
