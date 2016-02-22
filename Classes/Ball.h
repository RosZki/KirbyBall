#pragma once

#include "cocos2d.h"

USING_NS_CC;

class Ball : public cocos2d::Sprite{

public:
	Ball(Vec2 position);
	~Ball();
	static Ball* create(Vec2 position);
};