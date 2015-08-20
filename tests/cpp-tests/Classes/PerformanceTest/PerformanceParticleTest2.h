#ifndef __PERFORMANCE_PARTICLE_TEST2_H__
#define __PERFORMANCE_PARTICLE_TEST2_H__

#include "BaseTest.h"

DEFINE_TEST_SUITE(PerformceParticleTests2);

class ParticleMainScene2 : public TestCase
{
public:
    virtual void initWithSubTest(int subtest, int particles);

    void createParticleSystem(int idx);
    void testNCallback(cocos2d::Ref* sender);
    void updateQuantityLabel();
    int getSubTestNum() { return _subtestNumber; }
    int getParticlesNum() { return _quantityParticles; }
    virtual void doTest() = 0;

protected:
    int             _lastRenderedCount;
    int             _quantityParticles;
    int             _subtestNumber;
    cocos2d::Label *_particleLab;
};

class ParticleSystemPerformTest2 : public ParticleMainScene2
{
public:
    CREATE_FUNC(ParticleSystemPerformTest2);

    virtual bool init() override;
    virtual std::string title() const override;
    virtual void doTest(){};
};

#endif
