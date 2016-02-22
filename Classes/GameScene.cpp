#include "GameScene.h"
#include "Ball.h"
#include "Functions.h"
#include "EntityContactListener.h"
#include "Box2D\Box2D.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"

USING_NS_CC;
#define PTM_RATIO 32

Sprite *background, *paddle;

Ball *ball;

Vector<SpriteFrame *> background_frames;

RepeatForever *background_animation;

unsigned int playing_effect = 0;

Scene* GameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("hit.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("boing.wav");

	auto listener = EventListenerMouse::create();
	listener->onMouseMove = [&](Event* event) {
		try {
			if (_mouseJoint == NULL)
				return;
			EventMouse* mouseEvent = dynamic_cast<EventMouse *>(event);
			auto location = mouseEvent->getLocation();
			location = Director::getInstance()->convertToGL(location);
			b2Vec2 locationWorld = b2Vec2(location.x / PTM_RATIO, location.y / PTM_RATIO);

			_mouseJoint->SetTarget(locationWorld);
			
		}
		catch (std::bad_cast& e) {
			return;
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	_touchListener = listener;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("kirbyball.plist");

	background_frames = Functions::getAnimation("BACKGROUND/frame%d.png", 2);
	background_animation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(background_frames, 1.0f / 2)));
	background_animation->retain();

	background = Sprite::createWithSpriteFrame(background_frames.front());
	background->setAnchorPoint(Vec2(0, 0));
	background->setPosition(0, 0);
	background->runAction(background_animation);
	this->addChild(background);

	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);

	bool doSleep = true;
	_world = new b2World(gravity);
	_world->SetAllowSleeping(doSleep);

	float widthInMeters = visibleSize.width / PTM_RATIO;
	float heightInMeters = visibleSize.height / PTM_RATIO;
	b2Vec2 lowerLeftCorner = b2Vec2(0, 0);
	b2Vec2 lowerRightCorner = b2Vec2(widthInMeters, 0);
	b2Vec2 upperLeftCorner = b2Vec2(0, heightInMeters);
	b2Vec2 upperRightCorner = b2Vec2(widthInMeters, heightInMeters);

	b2BodyDef screenBorderDef;
	screenBorderDef.position.Set(0, 0);
	_groundBody = _world->CreateBody(&screenBorderDef);
	b2EdgeShape screenBorderShape;

	screenBorderShape.Set(lowerLeftCorner, lowerRightCorner);
	_bottomFixture = _groundBody->CreateFixture(&screenBorderShape, 0);
	screenBorderShape.Set(lowerRightCorner, upperRightCorner);
	_groundBody->CreateFixture(&screenBorderShape, 0);
	screenBorderShape.Set(upperRightCorner, upperLeftCorner);
	_groundBody->CreateFixture(&screenBorderShape, 0);
	screenBorderShape.Set(upperLeftCorner, lowerLeftCorner);
	_groundBody->CreateFixture(&screenBorderShape, 0);

	ball = Ball::create(Vec2(100, 100));
	ball->setTag(1);
	background->addChild(ball);

	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(100 / PTM_RATIO, 100 / PTM_RATIO);
	ballBodyDef.userData = ball;

	b2Body *ballBody = _world->CreateBody(&ballBodyDef);

	b2CircleShape circle;
	circle.m_radius = 25.0 / PTM_RATIO;

	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;
	ballShapeDef.density = 1.0f;
	ballShapeDef.friction = 0.f;
	ballShapeDef.restitution = 1.0f;
	_ballFixture = ballBody->CreateFixture(&ballShapeDef);

	b2Vec2 force = b2Vec2(10, 10);
	ballBody->ApplyLinearImpulse(force, ballBodyDef.position, true);

	paddle = Sprite::createWithSpriteFrameName("paddle.png");
	paddle->setPosition(visibleSize.width / 2, 50);
	background->addChild(paddle);

	b2BodyDef paddleBodyDef;
	paddleBodyDef.type = b2_dynamicBody;
	paddleBodyDef.position.Set(visibleSize.width / 2 / PTM_RATIO, 50 / PTM_RATIO);
	paddleBodyDef.userData = paddle;
	_paddleBody = _world->CreateBody(&paddleBodyDef);

	b2PolygonShape paddleShape;
	paddleShape.SetAsBox(paddle->getContentSize().width / PTM_RATIO / 2,
		paddle->getContentSize().height / PTM_RATIO / 2);

	b2FixtureDef paddleShapeDef;
	paddleShapeDef.shape = &paddleShape;
	paddleShapeDef.density = 10.0f;
	paddleShapeDef.friction = 0.4f;
	paddleShapeDef.restitution = 0.1f;
	_paddleFixture = _paddleBody->CreateFixture(&paddleShapeDef);

	b2PrismaticJointDef jointDef;
	b2Vec2 worldAxis(1.0f, 0.0f);
	jointDef.collideConnected = true;
	jointDef.Initialize(_paddleBody, _groundBody,
		_paddleBody->GetWorldCenter(), worldAxis);
	_world->CreateJoint(&jointDef);

	b2MouseJointDef md;
	md.bodyA = _groundBody;
	md.bodyB = _paddleBody;
	md.target = b2Vec2(visibleSize.width / 2 / PTM_RATIO, 50 / PTM_RATIO);
	md.collideConnected = true;
	md.maxForce = 1000.0f * _paddleBody->GetMass();

	_mouseJoint = (b2MouseJoint *)_world->CreateJoint(&md);
	_paddleBody->SetAwake(true);

	_contactListener = new EntityContactListener();
	_world->SetContactListener(_contactListener);

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			Sprite *block = Sprite::createWithSpriteFrameName("block.png");

			int xOffset = block->getContentSize().width / 2 + 50;
			float xPos = xOffset + block->getContentSize().width * i;
			int yOffset = background->getContentSize().height - block->getContentSize().height / 2;
			float yPos = yOffset - block->getContentSize().height* 1.5 * j;

			block->setPosition(xPos, yPos);
			block->setTag(2);
			background->addChild(block);

			b2BodyDef blockBodyDef;
			blockBodyDef.type = b2_staticBody;
			blockBodyDef.position.Set(xPos / PTM_RATIO, yPos / PTM_RATIO);
			blockBodyDef.userData = block;
			b2Body *blockBody = _world->CreateBody(&blockBodyDef);

			b2PolygonShape blockShape;
			blockShape.SetAsBox(block->getContentSize().width / PTM_RATIO / 2,
				block->getContentSize().height / PTM_RATIO / 2);

			b2FixtureDef blockShapeDef;
			blockShapeDef.shape = &blockShape;
			blockShapeDef.density = 10.0;
			blockShapeDef.friction = 0.0;
			blockShapeDef.restitution = 0.1f;
			blockBody->CreateFixture(&blockShapeDef);
		}
	}

	this->schedule(schedule_selector(GameScene::tick));

	cocos2d::experimental::AudioEngine::play2d("background.mp3", true, 0.2);

	return true;
}

void GameScene::tick(float dt)
{
	int velocityIterations = 8;
	int positionIterations = 1;

	_world->Step(dt, velocityIterations, positionIterations);

	bool blockFound = false;

	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			Sprite* myActor = (Sprite*)b->GetUserData();
			myActor->setPosition(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO);
			myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));

			if (myActor->getTag() == 1) {
				static int maxSpeed = 10;

				b2Vec2 velocity = b->GetLinearVelocity();
				float32 speed = velocity.Length();

				if (speed > maxSpeed) {
					b->SetLinearDamping(0.5);
				}
				else if (speed < maxSpeed) {
					b->SetLinearDamping(0.0);
				}

			}

			if (myActor->getTag() == 2) {
				blockFound = true;
			}
		}
	}

	std::vector<b2Body *>toDestroy;
	std::vector<EntityContact>::iterator pos;
	for (pos = _contactListener->_contacts.begin();
	pos != _contactListener->_contacts.end(); ++pos) {
		EntityContact contact = *pos;
		if (playing_effect != 0) {
			CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(playing_effect);
			playing_effect = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("boing.wav");
		}
		else {
			playing_effect = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("boing.wav");
		}
		if ((contact.fixtureA == _bottomFixture && contact.fixtureB == _ballFixture) ||
			(contact.fixtureA == _ballFixture && contact.fixtureB == _bottomFixture)) {
			//GameOverScene *gameOverScene = GameOverScene::create();
			//gameOverScene->getLayer()->getLabel()->setString("You Lose! :[");
			//CCDirector::getInstance()->replaceScene(gameOverScene);
			Director::getInstance()->pause();
		}

		b2Body *bodyA = contact.fixtureA->GetBody();
		b2Body *bodyB = contact.fixtureB->GetBody();
		if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL) {
			Sprite *spriteA = (Sprite *)bodyA->GetUserData();
			Sprite *spriteB = (Sprite *)bodyB->GetUserData();

			if (spriteA->getTag() == 1 && spriteB->getTag() == 2) {
				if (std::find(toDestroy.begin(), toDestroy.end(), bodyB)
					== toDestroy.end()) {
					toDestroy.push_back(bodyB);
				}
			}
			// Sprite B = block, Sprite A = ball
			else if (spriteA->getTag() == 2 && spriteB->getTag() == 1) {
				if (std::find(toDestroy.begin(), toDestroy.end(), bodyA)
					== toDestroy.end()) {
					toDestroy.push_back(bodyA);
				}
			}
			else {
				
			}
		}
	}

	std::vector<b2Body *>::iterator pos2;
	for (pos2 = toDestroy.begin(); pos2 != toDestroy.end(); ++pos2) {
		b2Body *body = *pos2;
		if (body->GetUserData() != NULL) {
			Sprite *sprite = (Sprite *)body->GetUserData();
			background->removeChild(sprite, true);
		}
		_world->DestroyBody(body);
	}

	if (!blockFound)
	{
		Director::getInstance()->pause();
		//GameOverScene *gameOverScene = GameOverScene::create();
		//gameOverScene->getLayer()->getLabel()->setString("You Win!");
		//Director::getInstance()->replaceScene(gameOverScene);
	}

	if (toDestroy.size() > 0)
	{
		// SimpleAudioEngine::getInstance()->playEffect("blip.caf");
		if (playing_effect != 0) {
			CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(playing_effect);
			playing_effect = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
		}
		else {
			playing_effect = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
		}
	}
}