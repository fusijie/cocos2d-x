#include "PerformanceParticleTest2.h"
#include "../testResource.h"

USING_NS_CC;
using namespace cocos2d::ui;

static int kTagInfoLayer = 1;
static int kTagParticleSystem = 1001;

static int kMaxParticles = 14000;
static int kNodesIncrease = 1;

PerformceParticleTests2::PerformceParticleTests2()
{
    ADD_TEST_CASE(ParticleSystemPerformTest2);
}

////////////////////////////////////////////////////////
//
// ParticleMainScene
//
////////////////////////////////////////////////////////
void ParticleMainScene2::initWithSubTest(int asubtest, int particles)
{
    //srandom(0);

    _subtestNumber = asubtest;
    auto s = Director::getInstance()->getWinSize();

    _lastRenderedCount = 0;
    _quantityParticles = 0;

    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", [=](Ref *sender) {
        _quantityParticles -= kNodesIncrease;
        if( _quantityParticles < 0 )
            _quantityParticles = 0;

        updateQuantityLabel();
        removeChildByTag(kTagParticleSystem + _quantityParticles, true);
    });
    decrease->setColor(Color3B(0,200,20));
    auto increase = MenuItemFont::create(" + ", [=](Ref *sender) {
        _quantityParticles += kNodesIncrease;
        if( _quantityParticles > kMaxParticles )
            _quantityParticles = kMaxParticles;

        updateQuantityLabel();
        createParticleSystem(_quantityParticles - 1);
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

    TTFConfig config("fonts/tahoma.ttf",10);
    _particleLab = Label::createWithTTF(config,"Particle Count: 0",TextHAlignment::LEFT);
    _particleLab->setPosition(Vec2(0.0f, s.height / 6.0f));
    _particleLab->setAnchorPoint(Vec2(0.0f, 0.0f));
    this->addChild(_particleLab);
    _quantityParticles = 1;
    updateQuantityLabel();
    createParticleSystem(_quantityParticles - 1);
    
}

void ParticleMainScene2::createParticleSystem(int idx)
{
    auto particleSystem = ParticleFireworks::create();
    particleSystem->retain();
    
    auto s = Director::getInstance()->getWinSize();
    
    if (particleSystem != nullptr)
    {
        particleSystem->setPosition( Vec2(s.width * CCRANDOM_0_1(), s.height * CCRANDOM_0_1()) );
    }

    particleSystem->setTexture( Director::getInstance()->getTextureCache()->addImage(s_stars1) );
    addChild(particleSystem, 0, kTagParticleSystem + idx);
}

void ParticleMainScene2::updateQuantityLabel()
{
    if( _quantityParticles != _lastRenderedCount )
    {
        auto infoLabel = (Label *) getChildByTag(kTagInfoLayer);
        char str[20] = {0};
        sprintf(str, "%u Particle Systems", _quantityParticles);
        infoLabel->setString(str);

        _lastRenderedCount = _quantityParticles;
    }
}

////////////////////////////////////////////////////////
//
// ParticleSystemPerformTest2
//
////////////////////////////////////////////////////////
std::string ParticleSystemPerformTest2::title() const
{
    return "Particle3D Test";
}

bool ParticleSystemPerformTest2::init()
{
    if (ParticleMainScene2::init())
    {
        initWithSubTest(1, kNodesIncrease);
        return true;
    }

    return false;
}
