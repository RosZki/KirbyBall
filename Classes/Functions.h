#pragma once
#include "cocos2d.h"

USING_NS_CC;


static class Functions {
public:
	Vector<SpriteFrame*> static getAnimation(const char *format, int count);
};