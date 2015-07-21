#include "HelloWorldScene.h"
#include "AppMacros.h"

USING_NS_CC;


Scene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    Director::getInstance()->setClearColor(Color4F::RED);
    _visibleSize = Director::getInstance()->getVisibleSize();
    
    bool isSprite = false;
    
    if (isSprite) {
        sprite1 = Sprite::create("1_bg_ironfloor_mid_.png");
        sprite1->getTexture()->setAliasTexParameters();
        sprite1->setPosition(Vec2(_visibleSize.width/2, _visibleSize.height/2));
        this->addChild(sprite1, 1);
        
        sprite2 = Sprite::create("1_bg_ironfloor_mid_.png");
        sprite2->getTexture()->setAliasTexParameters();
        sprite2->setPosition(Vec2(_visibleSize.width/2, _visibleSize.height/2 + 30.0f));
        this->addChild(sprite2, 1);
    }
    else
    {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("levelone_pack_complete4.plist");
        
        sprite1 = Sprite::createWithSpriteFrameName("1_bg_ironfloor_mid_.png");
        sprite1->getTexture()->setAliasTexParameters();
        sprite1->setPosition(Vec2(_visibleSize.width/2, _visibleSize.height/2));
        this->addChild(sprite1, 1);
        
        sprite2 = Sprite::createWithSpriteFrameName("1_bg_ironfloor_mid_.png");
        sprite2->getTexture()->setAliasTexParameters();
        sprite2->setPosition(Vec2(_visibleSize.width/2, _visibleSize.height/2 + 30.0f));
        this->addChild(sprite2, 1);
    }
    this->scheduleUpdate();
    
    return true;
}

void HelloWorld::menuCloseCallback(Ref* sender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::update(float dt)
{
    auto pinfo = sprite1->getPolygonInfo();
    sprite1->setPositionY(sprite1->getPositionY() + 0.1f);
    sprite2->setPositionY(sprite2->getPositionY() + 0.1f);
}
