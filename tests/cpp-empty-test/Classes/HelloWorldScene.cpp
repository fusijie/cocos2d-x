#include "HelloWorldScene.h"
#include "AppMacros.h"

USING_NS_CC;
using namespace cocos2d::ui;

static int kTagInfoLayer = 9999;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    static int kNodesIncrease = 10;
#else
    static int kNodesIncrease = 10;
#endif

inline static float
RANDOM_M11(unsigned int *seed) {
    *seed = *seed * 134775813 + 1;
    union {
        uint32_t d;
        float f;
    } u;
    u.d = (((uint32_t)(*seed) & 0x7fff) << 8) | 0x40000000;
    return u.f - 3.0f;
}

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
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        CC_CALLBACK_1(HelloWorld::menuCloseCallback,this));
    
    closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    initParticleTest();
    
//    struct timeval t1, t2;
//    gettimeofday(&t1, nullptr);
//    uint32_t RANDSEED = rand();
//    for (int i = 0; i < 100000000; i++) {
////        float a = CCRANDOM_MINUS1_1();
//        float a = RANDOM_M11(&RANDSEED);
//        CC_UNUSED_PARAM(a);
//    }
//    gettimeofday(&t2, nullptr);
//    log("%ld",t2.tv_sec * 1000000 + t2.tv_usec - t1.tv_sec * 1000000 - t1.tv_usec);

    return true;
}

void HelloWorld::menuCloseCallback(Ref* sender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

HelloWorld::HelloWorld()
{
    _ps.clear();
    _quantityParticles = 0;
}

HelloWorld::~HelloWorld()
{
    _ps.clear();
    _quantityParticles = 0;
}

void HelloWorld::initParticleTest()
{
    auto s = Director::getInstance()->getWinSize();
    
    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", [=](Ref *sender) {
        _quantityParticles -= kNodesIncrease;
        if( _quantityParticles >= 0 )
        {
            updateQuantityLabel();
            for (int i = 0; i < kNodesIncrease; i++) {
                removeChild(_ps.at(_ps.size()-1));
                _ps.popBack();
            }
        }
        else
        {
            _quantityParticles = 0;
        }
    });
    decrease->setColor(Color3B(0,200,20));
    auto increase = MenuItemFont::create(" + ", [=](Ref *sender) {
        _quantityParticles += kNodesIncrease;

        updateQuantityLabel();
        createParticleSystem(kNodesIncrease);
    });
    increase->setColor(Color3B(0,200,20));
    
    auto menu = Menu::create(decrease, increase, nullptr);
    menu->alignItemsHorizontally();
    menu->setPosition(Vec2(s.width/2, s.height/2+15));
    addChild(menu, 1);
    
    auto infoLabel = Label::createWithTTF("0 Particle Systems", "fonts/Marker Felt.ttf", 30);
    infoLabel->setColor(Color3B(0,200,20));
    infoLabel->setPosition(Vec2(s.width/2, s.height - 90));
    addChild(infoLabel, 1, kTagInfoLayer);
    
    _quantityParticles = kNodesIncrease;
    updateQuantityLabel();
    createParticleSystem(kNodesIncrease);
}

void HelloWorld::createParticleSystem(int count)
{
    auto s = Director::getInstance()->getWinSize();
    for (int i = 0; i < count; i++) {
        auto particleSystem = ParticleFireworks::create();
        if (particleSystem != nullptr)
        {
            particleSystem->setPosition( Vec2(s.width * CCRANDOM_0_1(), s.height * CCRANDOM_0_1()) );
        }
        particleSystem->setTexture( Director::getInstance()->getTextureCache()->addImage("stars.png") );
        addChild(particleSystem);
        _ps.pushBack(particleSystem);
    }
}

void HelloWorld::updateQuantityLabel()
{
    auto infoLabel = (Label *) getChildByTag(kTagInfoLayer);
    char str[20] = {0};
    sprintf(str, "%u Particle Systems", _quantityParticles);
    infoLabel->setString(str);
}