#pragma once
#include "GameFinishScene.h"
#include "GameScene.h"
#include "AudioEngine.h"
USING_NS_CC;

Label* textLabel;

Scene* GameFinishScene::createGameOverScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameFinishScene::create();
	
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();

	textLabel = Label::createWithSystemFont("Oh, dear! The town has permanently frozen over!", "Calibri", 36);
	textLabel->setDimensions(300, 0);
	textLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
	textLabel->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	layer->addChild(textLabel);

	// add layer as a child to scene
	scene->addChild(layer);

	cocos2d::experimental::AudioEngine::stopAll();
	cocos2d::experimental::AudioEngine::play2d("gameover.mp3", true, 1);

	// return the scene
	return scene;
}

Scene* GameFinishScene::createWinScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameFinishScene::create();

	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();

	textLabel = Label::createWithSystemFont("Congratulations! The town is saved!", "Calibri", 36);
	textLabel->setDimensions(300, 0);
	textLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
	textLabel->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	layer->addChild(textLabel);

	cocos2d::experimental::AudioEngine::stopAll();
	cocos2d::experimental::AudioEngine::play2d("win.mp3", true, 1);

	// add layer as a child to scene
	scene->addChild(layer);
	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameFinishScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_R:
			Director::getInstance()->replaceScene(TransitionFade::create(0.5, GameScene::createScene(), Color3B(0, 255, 255)));
			break;
		}
	};
	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}