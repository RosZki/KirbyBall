#pragma once
#include "Ball.h"
#include "Functions.h"

Ball::Ball(Vec2 position){
	this->setPosition(position);
}

Ball::~Ball() {}

Ball* Ball::create(Vec2 position) {
	Ball* ball = new Ball(position);
	auto frames = Functions::getAnimation("KIRBY_BALL/frame%d.png", 15);
	if (ball->initWithSpriteFrame(frames.front())) {
		auto animation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(frames, 1.0f / 15)));
		ball->runAction(animation);
		ball->autorelease();
		return ball;
	}
	CC_SAFE_DELETE(ball);
	return NULL;
}