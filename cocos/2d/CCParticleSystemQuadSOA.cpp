/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2009      Leonardo Kasperavičius
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 Copyright (c) 2015      RUKD
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/


#include "2d/CCParticleSystemQuadSOA.h"

#include <algorithm>

#include "2d/CCSpriteFrame.h"
#include "2d/CCParticleBatchNode.h"
#include "renderer/CCTextureAtlas.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCRenderer.h"
#include "base/CCDirector.h"
#include "base/CCEventType.h"
#include "base/CCConfiguration.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"

#include "deprecated/CCString.h"
#include "base/CCProfiling.h"

NS_CC_BEGIN

ParticleDataSOA::ParticleDataSOA()
{
    memset(this, 0, sizeof(ParticleDataSOA));
}

bool ParticleDataSOA::init(int count)
{
    maxCount = count;
    
    posx= (float*)malloc(count * sizeof(float));
    posy= (float*)malloc(count * sizeof(float));
    startPosX= (float*)malloc(count * sizeof(float));
    startPosY= (float*)malloc(count * sizeof(float));
    colorR= (float*)malloc(count * sizeof(float));
    colorG= (float*)malloc(count * sizeof(float));
    colorB= (float*)malloc(count * sizeof(float));
    colorA= (float*)malloc(count * sizeof(float));
    deltaColorR= (float*)malloc(count * sizeof(float));
    deltaColorG= (float*)malloc(count * sizeof(float));
    deltaColorB= (float*)malloc(count * sizeof(float));
    deltaColorA= (float*)malloc(count * sizeof(float));
    size= (float*)malloc(count * sizeof(float));
    deltaSize= (float*)malloc(count * sizeof(float));
    rotation= (float*)malloc(count * sizeof(float));
    deltaRotation= (float*)malloc(count * sizeof(float));
    timeToLive= (float*)malloc(count * sizeof(float));
    atlasIndex= (unsigned int*)malloc(count * sizeof(unsigned int));
    
    modeA.dirX= (float*)malloc(count * sizeof(float));
    modeA.dirY= (float*)malloc(count * sizeof(float));
    modeA.radialAccel= (float*)malloc(count * sizeof(float));
    modeA.tangentialAccel= (float*)malloc(count * sizeof(float));
    
    modeB.angle= (float*)malloc(count * sizeof(float));
    modeB.degreesPerSecond= (float*)malloc(count * sizeof(float));
    modeB.deltaRadius= (float*)malloc(count * sizeof(float));
    modeB.radius= (float*)malloc(count * sizeof(float));
    
    return posx && posy && startPosY && startPosX && colorR && colorG && colorB && colorA &&
    deltaColorR && deltaColorG && deltaColorB && deltaColorA && size && deltaSize &&
    rotation && deltaRotation && timeToLive && atlasIndex && modeA.dirX && modeA.dirY &&
    modeA.radialAccel && modeA.tangentialAccel && modeB.angle && modeB.degreesPerSecond &&
    modeB.deltaRadius && modeB.radius;
}

void ParticleDataSOA::release()
{
    CC_SAFE_FREE(posx);
    CC_SAFE_FREE(posy);
    CC_SAFE_FREE(startPosX);
    CC_SAFE_FREE(startPosY);
    CC_SAFE_FREE(colorR);
    CC_SAFE_FREE(colorG);
    CC_SAFE_FREE(colorB);
    CC_SAFE_FREE(colorA);
    CC_SAFE_FREE(deltaColorR);
    CC_SAFE_FREE(deltaColorG);
    CC_SAFE_FREE(deltaColorB);
    CC_SAFE_FREE(deltaColorA);
    CC_SAFE_FREE(size);
    CC_SAFE_FREE(deltaSize);
    CC_SAFE_FREE(rotation);
    CC_SAFE_FREE(deltaRotation);
    CC_SAFE_FREE(timeToLive);
    CC_SAFE_FREE(atlasIndex);
    
    CC_SAFE_FREE(modeA.dirX);
    CC_SAFE_FREE(modeA.dirY);
    CC_SAFE_FREE(modeA.radialAccel);
    CC_SAFE_FREE(modeA.tangentialAccel);
    
    CC_SAFE_FREE(modeB.angle);
    CC_SAFE_FREE(modeB.degreesPerSecond);
    CC_SAFE_FREE(modeB.deltaRadius);
    CC_SAFE_FREE(modeB.radius);
}



ParticleSystemQuadSOA::ParticleSystemQuadSOA()
:ParticleSystemQuad()
{
    memset(_buffersVBO, 0, sizeof(_buffersVBO));
}

ParticleSystemQuadSOA::~ParticleSystemQuadSOA()
{
    if (nullptr == _batchNode)
    {
        CC_SAFE_FREE(_quads);
        CC_SAFE_FREE(_indices);
        glDeleteBuffers(2, &_buffersVBO[0]);
        if (Configuration::getInstance()->supportsShareableVAO())
        {
            glDeleteVertexArrays(1, &_VAOname);
            GL::bindVAO(0);
        }
    }
    
    mParticleData.release();
}

// implementation ParticleSystemQuadSOA

ParticleSystemQuadSOA * ParticleSystemQuadSOA::create(const std::string& filename)
{
    ParticleSystemQuadSOA *ret = new (std::nothrow) ParticleSystemQuadSOA();
    if (ret && ret->initWithFile(filename))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

ParticleSystemQuadSOA * ParticleSystemQuadSOA::createWithTotalParticles(int numberOfParticles) {
    ParticleSystemQuadSOA *ret = new (std::nothrow) ParticleSystemQuadSOA();
    if (ret && ret->initWithTotalParticles(numberOfParticles))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

ParticleSystemQuadSOA * ParticleSystemQuadSOA::create(ValueMap &dictionary)
{
    ParticleSystemQuadSOA *ret = new (std::nothrow) ParticleSystemQuadSOA();
    if (ret && ret->initWithDictionary(dictionary))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

//implementation ParticleSystemQuadSOA
// overriding the init method
bool ParticleSystemQuadSOA::initWithTotalParticles(int numberOfParticles)
{
    // base initialization
    _totalParticles = numberOfParticles;

    
    mParticleData.release();
    
    
    if( !mParticleData.init(_totalParticles) )
    {
        CCLOG("Particle system Quad Neon: not enough memory");
        this->release();
        return false;
    }
    _allocatedParticles = numberOfParticles;
    
    if (_batchNode)
    {
        for (int i = 0; i < _totalParticles; i++)
        {
            mParticleData.atlasIndex[i] = i;
        }
    }
    // default, active
    _isActive = true;
    
    // default blend function
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    
    // default movement type;
    _positionType = PositionType::FREE;
    
    // by default be in mode A:
    _emitterMode = Mode::GRAVITY;
    
    // default: modulate
    // FIXME:: not used
    //    colorModulate = YES;
    
    _isAutoRemoveOnFinish = false;
    
    // Optimization: compile updateParticle method
    //updateParticleSel = @selector(updateQuadWithParticle:newPosition:);
    //updateParticleImp = (CC_UPDATE_PARTICLE_IMP) [self methodForSelector:updateParticleSel];
    //for batchNode
    _transformSystemDirty = false;

    //if( ParticleSystem::initWithTotalParticles(numberOfParticles) )
        // allocating data space
        if( ! this->allocMemory() ) {
            this->release();
            return false;
        }
        
        initIndices();
        if (Configuration::getInstance()->supportsShareableVAO())
        {
            setupVBOandVAO();
        }
        else
        {
            setupVBO();
        }
        
        setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
        
#if CC_ENABLE_CACHE_TEXTURE_DATA
        // Need to listen the event only when not use batchnode, because it will use VBO
        auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, CC_CALLBACK_1(ParticleSystemQuadSOA::listenRendererRecreated, this));
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
        
        return true;
}

bool ParticleSystemQuadSOA::addParticle()
{
    assert(0&&"never call it");
    return false;
}

bool ParticleSystemQuadSOA::addParticles(int count)
{
    int start = _particleCount;
    _particleCount += count;
    for (int i = start; i < _particleCount ; ++i)
    {
        float theLife = _life + _lifeVar * CCRANDOM_MINUS1_1();
        mParticleData.timeToLive[i] = MAX(0,theLife);
    }
    
    for (int i = start; i < _particleCount; ++i)
    {
        mParticleData.posx[i] = _sourcePosition.x + _posVar.x * CCRANDOM_MINUS1_1();
    }
    
    for (int i = start; i < _particleCount; ++i)
    {
        mParticleData.posy[i] = _sourcePosition.y + _posVar.y * CCRANDOM_MINUS1_1();
    }
    
    //color
#define SET_COLOR(c,b,v)\
    for (int i = start; i < _particleCount; ++i)\
    {\
        c[i] = clampf( b + v * CCRANDOM_MINUS1_1() , 0 , 1 );\
    }
    
    SET_COLOR(mParticleData.colorR, _startColor.r, _startColorVar.r);
    SET_COLOR(mParticleData.colorG, _startColor.g, _startColorVar.g);
    SET_COLOR(mParticleData.colorB, _startColor.b, _startColorVar.b);
    SET_COLOR(mParticleData.colorA, _startColor.a, _startColorVar.a);
    
    SET_COLOR(mParticleData.deltaColorR, _endColor.r, _endColorVar.r);
    SET_COLOR(mParticleData.deltaColorG, _endColor.g, _endColorVar.g);
    SET_COLOR(mParticleData.deltaColorB, _endColor.b, _endColorVar.b);
    SET_COLOR(mParticleData.deltaColorA, _endColor.a, _endColorVar.a);
    
#define SET_DELTA_COLOR(c,dc)\
    for (int i = start; i < _particleCount; ++i)\
    {\
        dc[i] = (dc[i] - c[i])/mParticleData.timeToLive[i];\
    }
    
    SET_DELTA_COLOR(mParticleData.colorR, mParticleData.deltaColorR);
    SET_DELTA_COLOR(mParticleData.colorG, mParticleData.deltaColorG);
    SET_DELTA_COLOR(mParticleData.colorB, mParticleData.deltaColorB);
    SET_DELTA_COLOR(mParticleData.colorA, mParticleData.deltaColorA);
    
    //size
    
    for (int i = start; i < _particleCount; ++i)
    {
        mParticleData.size[i] = _startSize + _startSizeVar * CCRANDOM_MINUS1_1();
        mParticleData.size[i] = MAX(0,mParticleData.size[i]);
    }
    
    if (_endSize != START_SIZE_EQUAL_TO_END_SIZE)
    {
        for (int i = start; i < _particleCount; ++i)
        {
            float endSize = _endSize + _endSizeVar * CCRANDOM_MINUS1_1();
            endSize = MAX(0,endSize);
            mParticleData.deltaSize[i] = (endSize - mParticleData.size[i])/mParticleData.timeToLive[i];
        }
    }else
    {
        for (int i = start; i < _particleCount; ++i)
        {
            mParticleData.deltaSize[i] = 0.0f;
        }
    }
    
    // rotation
    for (int i = start; i < _particleCount; ++i)
    {
        mParticleData.rotation[i] = _startSpin + _startSpinVar * CCRANDOM_MINUS1_1();
    }
    for (int i = start; i < _particleCount; ++i)
    {
        
        float endA = _endSpin + _endSpinVar * CCRANDOM_MINUS1_1();
        mParticleData.deltaRotation[i] = (endA - mParticleData.rotation[i])/mParticleData.timeToLive[i];
    }
    
    // position
    Vec2 pos = _position;
    if (_positionType == PositionType::FREE)
    {
        pos = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if (_positionType == PositionType::RELATIVE)
    {
        pos = _position;
    }
    for (int i = start; i < _particleCount; ++i)
    {
        mParticleData.startPosX[i] = pos.x;
    }
    for (int i = start; i < _particleCount; ++i)
    {
        mParticleData.startPosY[i] = pos.y;
    }
    
    // direction
    //float a = CC_DEGREES_TO_RADIANS( _angle + _angleVar * CCRANDOM_MINUS1_1() );
    
    // Mode Gravity: A
    if (_emitterMode == Mode::GRAVITY)
    {
        
        // radial accel
        for (int i = start; i < _particleCount; ++i)
        {
            mParticleData.modeA.radialAccel[i] = modeA.radialAccel + modeA.radialAccelVar * CCRANDOM_MINUS1_1();
        }
        
        // tangential accel
        for (int i = start; i < _particleCount; ++i)
        {
            mParticleData.modeA.tangentialAccel[i] = modeA.tangentialAccel + modeA.tangentialAccelVar * CCRANDOM_MINUS1_1();
        }
        
        
        // rotation is dir
        if( modeA.rotationIsDir )
        {
            for (int i = start; i < _particleCount; ++i)
            {
                float a = CC_DEGREES_TO_RADIANS( _angle + _angleVar * CCRANDOM_MINUS1_1() );
                Vec2 v(cosf( a ), sinf( a ));
                float s = modeA.speed + modeA.speedVar * CCRANDOM_MINUS1_1();
                Vec2 dir = v * s;
                mParticleData.modeA.dirX[i] = dir.x;//v * s ;
                mParticleData.modeA.dirY[i] = dir.y;
                mParticleData.rotation[i] = -CC_RADIANS_TO_DEGREES(dir.getAngle());
            }
        }else
        {
            for (int i = start; i < _particleCount; ++i)
            {
                float a = CC_DEGREES_TO_RADIANS( _angle + _angleVar * CCRANDOM_MINUS1_1() );
                Vec2 v(cosf( a ), sinf( a ));
                float s = modeA.speed + modeA.speedVar * CCRANDOM_MINUS1_1();
                Vec2 dir = v * s;
                mParticleData.modeA.dirX[i] = dir.x;//v * s ;
                mParticleData.modeA.dirY[i] = dir.y;
            }
        }
        
    }
    
    // Mode Radius: B
    else
    {
        assert(0&&"RUKD need check");
//        // Set the default diameter of the particle from the source position
//        float startRadius = modeB.startRadius + modeB.startRadiusVar * CCRANDOM_MINUS1_1();
//        float endRadius = modeB.endRadius + modeB.endRadiusVar * CCRANDOM_MINUS1_1();
//        
//        mParticleData.modeB.radius[index] = startRadius;
//        
//        if (modeB.endRadius == START_RADIUS_EQUAL_TO_END_RADIUS)
//        {
//            mParticleData.modeB.deltaRadius[index] = 0;
//        }
//        else
//        {
//            mParticleData.modeB.deltaRadius[index] = (endRadius - startRadius) / timeToLive;
//        }
//        
//        mParticleData.modeB.angle[index] = a;
//        mParticleData.modeB.degreesPerSecond[index] = CC_DEGREES_TO_RADIANS(modeB.rotatePerSecond + modeB.rotatePerSecondVar * CCRANDOM_MINUS1_1());
    }

    
    
    return true;
}

bool ParticleSystemQuadSOA::initParticleByIndex(int index)
{
    assert(index < mParticleData.getMaxCount());
    // timeToLive
    // no negative life. prevent division by 0
    float timeToLive = _life + _lifeVar * CCRANDOM_MINUS1_1();
    timeToLive = MAX(0, timeToLive);
    mParticleData.timeToLive[index] = timeToLive;
    
    // position
    mParticleData.posx[index] = _sourcePosition.x + _posVar.x * CCRANDOM_MINUS1_1();
    
    mParticleData.posy[index] = _sourcePosition.y + _posVar.y * CCRANDOM_MINUS1_1();
    
    
    // Color
    Color4F start;
    start.r = clampf(_startColor.r + _startColorVar.r * CCRANDOM_MINUS1_1(), 0, 1);
    start.g = clampf(_startColor.g + _startColorVar.g * CCRANDOM_MINUS1_1(), 0, 1);
    start.b = clampf(_startColor.b + _startColorVar.b * CCRANDOM_MINUS1_1(), 0, 1);
    start.a = clampf(_startColor.a + _startColorVar.a * CCRANDOM_MINUS1_1(), 0, 1);
    
    Color4F end;
    end.r = clampf(_endColor.r + _endColorVar.r * CCRANDOM_MINUS1_1(), 0, 1);
    end.g = clampf(_endColor.g + _endColorVar.g * CCRANDOM_MINUS1_1(), 0, 1);
    end.b = clampf(_endColor.b + _endColorVar.b * CCRANDOM_MINUS1_1(), 0, 1);
    end.a = clampf(_endColor.a + _endColorVar.a * CCRANDOM_MINUS1_1(), 0, 1);
    
    mParticleData.colorR[index] = start.r;
    mParticleData.colorG[index] = start.g;
    mParticleData.colorB[index] = start.b;
    mParticleData.colorA[index] = start.a;
    
    mParticleData.deltaColorR[index] = (end.r - start.r) / timeToLive;
    mParticleData.deltaColorG[index] = (end.g - start.g) / timeToLive;
    mParticleData.deltaColorB[index] = (end.b - start.b) / timeToLive;
    mParticleData.deltaColorA[index] = (end.a - start.a) / timeToLive;
    
    // size
    float startS = _startSize + _startSizeVar * CCRANDOM_MINUS1_1();
    startS = MAX(0, startS); // No negative value
    
    mParticleData.size[index] = startS;
    
    float deltaSize = 0;
    if (_endSize != START_SIZE_EQUAL_TO_END_SIZE)
    {
        float endS = _endSize + _endSizeVar * CCRANDOM_MINUS1_1();
        endS = MAX(0, endS); // No negative values
        deltaSize = (endS - startS) / timeToLive;
        
    }
    
    mParticleData.deltaSize[index] = deltaSize;
    
    // rotation
    float startA = _startSpin + _startSpinVar * CCRANDOM_MINUS1_1();
    float endA = _endSpin + _endSpinVar * CCRANDOM_MINUS1_1();
    mParticleData.rotation[index] = startA;
    mParticleData.deltaRotation[index] = (endA - startA) / timeToLive;
    
    // position
    Vec2 pos = _position;
    if (_positionType == PositionType::FREE)
    {
        pos = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if (_positionType == PositionType::RELATIVE)
    {
        pos = _position;
    }
    mParticleData.startPosX[index] = pos.x;
    mParticleData.startPosY[index] = pos.y;
    
    // direction
    float a = CC_DEGREES_TO_RADIANS( _angle + _angleVar * CCRANDOM_MINUS1_1() );
    
    // Mode Gravity: A
    if (_emitterMode == Mode::GRAVITY)
    {
        Vec2 v(cosf( a ), sinf( a ));
        float s = modeA.speed + modeA.speedVar * CCRANDOM_MINUS1_1();
        
        // direction
        Vec2 dir = v * s;
        mParticleData.modeA.dirX[index] = dir.x;//v * s ;
        mParticleData.modeA.dirY[index] = dir.y;
        
        // radial accel
        mParticleData.modeA.radialAccel[index] = modeA.radialAccel + modeA.radialAccelVar * CCRANDOM_MINUS1_1();
        
        
        // tangential accel
        mParticleData.modeA.tangentialAccel[index] = modeA.tangentialAccel + modeA.tangentialAccelVar * CCRANDOM_MINUS1_1();
        
        // rotation is dir
        if(modeA.rotationIsDir)
            mParticleData.rotation[index] = -CC_RADIANS_TO_DEGREES(dir.getAngle());
    }
    
    // Mode Radius: B
    else
    {
        // Set the default diameter of the particle from the source position
        float startRadius = modeB.startRadius + modeB.startRadiusVar * CCRANDOM_MINUS1_1();
        float endRadius = modeB.endRadius + modeB.endRadiusVar * CCRANDOM_MINUS1_1();
        
        mParticleData.modeB.radius[index] = startRadius;
        
        if (modeB.endRadius == START_RADIUS_EQUAL_TO_END_RADIUS)
        {
            mParticleData.modeB.deltaRadius[index] = 0;
        }
        else
        {
            mParticleData.modeB.deltaRadius[index] = (endRadius - startRadius) / timeToLive;
        }
        
        mParticleData.modeB.angle[index] = a;
        mParticleData.modeB.degreesPerSecond[index] = CC_DEGREES_TO_RADIANS(modeB.rotatePerSecond + modeB.rotatePerSecondVar * CCRANDOM_MINUS1_1());
    }
    return true;
}

void ParticleSystemQuadSOA::resetSystem()
{
    _isActive = true;
    _elapsed = 0;
    
    for (int i = 0; i < _particleCount; ++i)
    {
        mParticleData.timeToLive[i] = 0;
    }
}

void ParticleSystemQuadSOA::update(float dt)
{
    CC_PROFILER_START_CATEGORY(kProfilerCategoryParticles , "CCParticleSystemSOA - update");
    if( !_isActive )
        return;
    
    if (_isActive && _emissionRate)
    {
        
        _elapsed += dt;
        if (_duration != -1 && _duration < _elapsed)
        {
            this->stopSystem();
        }
    }
    int emitCount = 0;
    if (_isActive && _emissionRate)
    {
        _emitCounter += _emissionRate * dt;
        emitCount = _emitCounter;
        _emitCounter -= emitCount;
        emitCount = MIN(emitCount, _totalParticles - _particleCount);
        
        //        float rate = 1.0f / _emissionRate;
        //        //issue #1201, prevent bursts of particles, due to too high emitCounter
        //        if (_particleCount < _totalParticles)
        //        {
        //            _emitCounter += dt;
        //        }
        //
        //        while (_particleCount < _totalParticles && _emitCounter > rate)
        //        {
        //            this->addParticle();
        //            _emitCounter -= rate;
        //        }
    }else
    {
        return;
    }
    

    
    {
        
        
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.timeToLive[i] -= dt;
        }
        
        for (int i = 0 ; i < _particleCount; ++i)
        {
            if( mParticleData.timeToLive[i] <= 0.0f )
            {
                mParticleData.swapParticle(i, _particleCount-1);
                --_particleCount;
            }
            //RUKD checked
//            int currentIndex = p->atlasIndex;
//            if( _particleIdx != _particleCount-1 )
//            {
//                _particles[_particleIdx] = _particles[_particleCount-1];
//            }
//            if (_batchNode)
//            {
//                //disable the switched particle
//                _batchNode->disableParticle(_atlasIndex+currentIndex);
//                
//                //switch indexes
//                _particles[_particleCount-1].atlasIndex = currentIndex;
//            }
//            
//            
//            --_particleCount;
//            
//            if( _particleCount == 0 && _isAutoRemoveOnFinish )
//            {
//                this->unscheduleUpdate();
//                _parent->removeChild(this, true);
//                return;
//            }
        }
        
        addParticles(emitCount);
        
        if( _emitterMode == Mode::GRAVITY )
        {
            for (int i = 0 ; i < _particleCount; ++i)
            {
                Vec2 tmp, radial, tangential;
                
                radial.set(mParticleData.posx[i], mParticleData.posy[i]);// = Vec2::ZERO;
                
                // radial acceleration
                if (radial.x || radial.y)
                {
                    radial.normalize();
                }else
                {
                    radial = Vec2::ZERO;
                }
                tangential = radial;
                radial = radial * mParticleData.modeA.radialAccel[i];
                
                // tangential acceleration
                float newy = tangential.x;
                tangential.x = -tangential.y;
                tangential.y = newy;
                tangential = tangential * mParticleData.modeA.tangentialAccel[i];
                
                // (gravity + radial + tangential) * dt
                tmp = radial + tangential + modeA.gravity;
                tmp = tmp * dt;
                mParticleData.modeA.dirX[i] = mParticleData.modeA.dirX[i] + tmp.x;
                mParticleData.modeA.dirY[i] = mParticleData.modeA.dirY[i] + tmp.y;
                
                
                // this is cocos2d-x v3.0
                //                    if (_configName.length()>0 && _yCoordFlipped != -1)
                
                // this is cocos2d-x v3.0
                tmp.x = mParticleData.modeA.dirX[i] * dt * _yCoordFlipped;
                tmp.y = mParticleData.modeA.dirY[i] * dt * _yCoordFlipped;
                mParticleData.posx[i] = mParticleData.posx[i] + tmp.x;
                mParticleData.posy[i] = mParticleData.posy[i] + tmp.y;
            }
            
        }else
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                mParticleData.modeB.angle[i] += mParticleData.modeB.degreesPerSecond[i] * dt;
            }
            
            for (int i = 0; i < _particleCount; ++i)
            {
                mParticleData.modeB.radius[i] += mParticleData.modeB.deltaRadius[i] * dt;
            }
            
            for (int i = 0; i < _particleCount; ++i)
            {
                mParticleData.posx[i] = - cosf(mParticleData.modeB.angle[i]) * mParticleData.modeB.radius[i];
            }
            for (int i = 0; i < _particleCount; ++i)
            {
                mParticleData.posy[i] = - sinf(mParticleData.modeB.angle[i]) * mParticleData.modeB.radius[i]* _yCoordFlipped;
            }
        }
        

        //color r,g,b,a
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.colorR[i] += mParticleData.deltaColorR[i] * dt;
        }
        
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.colorG[i] += mParticleData.deltaColorG[i] * dt;
        }
        
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.colorB[i] += mParticleData.deltaColorB[i] * dt;
        }
        
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.colorA[i] += mParticleData.deltaColorA[i] * dt;
        }
        //size
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.size[i] += mParticleData.deltaSize[i] * dt;
            //mParticleData.size[i] = MAX( 0, mParticleData.size[i] );
        }
        //angle
        for (int i = 0 ; i < _particleCount; ++i)
        {
            mParticleData.rotation[i] += mParticleData.deltaRotation[i] * dt;
        }
        
        
        
        //update vertex data
        updateVertexData();
        
        _transformSystemDirty = false;
    }
    
    CC_PROFILER_STOP_CATEGORY(kProfilerCategoryParticles , "CCParticleSystemSOA - update");

    
    // only update gl buffer when visible
    if (_visible && ! _batchNode)
    {
        //CC_PROFILER_START_CATEGORY(kProfilerCategoryParticles , "CCParticleSystemNeonPostStep - update");
        postStep();
        //CC_PROFILER_STOP_CATEGORY(kProfilerCategoryParticles , "CCParticleSystemNeonPostStep - update");
    }
    
}

inline void updateColorWithParticle(V3F_C4B_T2F_Quad *quad,float r,float g,float b,float a)
{
    
}

inline void updatePosWithParticle(V3F_C4B_T2F_Quad *quad, const Vec2& newPosition,float size,float rotation)
{
    // vertices
    GLfloat size_2 = size/2;
    //if (particle->rotation)
    {
        GLfloat x1 = -size_2;
        GLfloat y1 = -size_2;
        
        GLfloat x2 = size_2;
        GLfloat y2 = size_2;
        GLfloat x = newPosition.x;
        GLfloat y = newPosition.y;
        
        GLfloat r = (GLfloat)-CC_DEGREES_TO_RADIANS(rotation);
        GLfloat cr = cosf(r);
        GLfloat sr = sinf(r);
        GLfloat ax = x1 * cr - y1 * sr + x;
        GLfloat ay = x1 * sr + y1 * cr + y;
        GLfloat bx = x2 * cr - y1 * sr + x;
        GLfloat by = x2 * sr + y1 * cr + y;
        GLfloat cx = x2 * cr - y2 * sr + x;
        GLfloat cy = x2 * sr + y2 * cr + y;
        GLfloat dx = x1 * cr - y2 * sr + x;
        GLfloat dy = x1 * sr + y2 * cr + y;
        
        // bottom-left
        quad->bl.vertices.x = ax;
        quad->bl.vertices.y = ay;
        
        // bottom-right vertex:
        quad->br.vertices.x = bx;
        quad->br.vertices.y = by;
        
        // top-left vertex:
        quad->tl.vertices.x = dx;
        quad->tl.vertices.y = dy;
        
        // top-right vertex:
        quad->tr.vertices.x = cx;
        quad->tr.vertices.y = cy;
    }
//    else
//    {
//        // bottom-left vertex:
//        quad->bl.vertices.x = newPosition.x - size_2;
//        quad->bl.vertices.y = newPosition.y - size_2;
//        
//        // bottom-right vertex:
//        quad->br.vertices.x = newPosition.x + size_2;
//        quad->br.vertices.y = newPosition.y - size_2;
//        
//        // top-left vertex:
//        quad->tl.vertices.x = newPosition.x - size_2;
//        quad->tl.vertices.y = newPosition.y + size_2;
//        
//        // top-right vertex:
//        quad->tr.vertices.x = newPosition.x + size_2;
//        quad->tr.vertices.y = newPosition.y + size_2;
//    }
}

void ParticleSystemQuadSOA::updateVertexData()
{
    Mat4 worldToNodeTM = getWorldToNodeTransform();
    Vec2 currentPosition = Vec2::ZERO;
    
    if (_positionType == PositionType::FREE)
    {
        currentPosition = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if (_positionType == PositionType::RELATIVE)
    {
        currentPosition = _position;
    }
    
    Vec3 p1(currentPosition.x,currentPosition.y,0);
    worldToNodeTM.transformPoint(&p1);
    
    
    V3F_C4B_T2F_Quad *startQuad;
    Vec2 _pos = Vec2::ZERO;
    if (_batchNode)
    {
        V3F_C4B_T2F_Quad *batchQuads = _batchNode->getTextureAtlas()->getQuads();
        startQuad = &(batchQuads[_atlasIndex]);
        _pos = _position;
    }
    else
    {
        startQuad = &(_quads[0]);
    }
    
    
    
    
    
    if( _positionType == PositionType::FREE )
    {
        float* startX = mParticleData.startPosX;
        float* startY = mParticleData.startPosY;
        float* x = mParticleData.posx;
        float* y = mParticleData.posy;
        float* s = mParticleData.size;
        float* r = mParticleData.rotation;
        V3F_C4B_T2F_Quad* quadStart = startQuad;
        for (int i = 0 ; i < _particleCount; ++i,++startX,++startY,++x,++y,++quadStart,++s,++r)
        {
            Vec3 p2(*startX,*startY,0);
            worldToNodeTM.transformPoint(&p2);
            Vec2 newPos(*x,*y);
            p2 = p1 - p2;
            newPos = newPos - Vec2(p2.x,p2.y) + _pos;
            updatePosWithParticle(quadStart,newPos,*s,*r);
        }
    }else if( _positionType == PositionType::RELATIVE )
    {
        float* startX = mParticleData.startPosX;
        float* startY = mParticleData.startPosY;
        float* x = mParticleData.posx;
        float* y = mParticleData.posy;
        float* s = mParticleData.size;
        float* r = mParticleData.rotation;
        V3F_C4B_T2F_Quad* quadStart = startQuad;
        for (int i = 0 ; i < _particleCount; ++i,++startX,++startY,++x,++y,++quadStart,++s,++r)
        {
            Vec2 newPos(*x,*y);
            newPos.x = *x - (currentPosition.x - *startX);
            newPos.y = *y - (currentPosition.y - *startY);
            newPos += _pos;
            updatePosWithParticle(quadStart,newPos,*s,*r);
        }
    }else
    {
        float* startX = mParticleData.startPosX;
        float* startY = mParticleData.startPosY;
        float* x = mParticleData.posx;
        float* y = mParticleData.posy;
        float* s = mParticleData.size;
        float* r = mParticleData.rotation;
        V3F_C4B_T2F_Quad* quadStart = startQuad;
        for (int i = 0 ; i < _particleCount; ++i,++startX,++startY,++x,++y,++quadStart,++s,++r)
        {
            Vec2 newPos(*x+_pos.x,*y+_pos.y);
            updatePosWithParticle(quadStart,newPos,*s,*r);
        }
    }
    
    //set color
    if(_opacityModifyRGB)
    {
        V3F_C4B_T2F_Quad* quad = startQuad;
        float* r = mParticleData.colorR;
        float* g = mParticleData.colorG;
        float* b = mParticleData.colorB;
        float* a = mParticleData.colorA;
        
        for (int i = 0; i < _particleCount; ++i,++quad,++r,++g,++b,++a)
        {
            Color4B color( *r * *a * 255, *g * *a * 255, *b * *a * 255, *a * 255);
            quad->bl.colors = color;
            quad->br.colors = color;
            quad->tl.colors = color;
            quad->tr.colors = color;
        }
    }else
    {
        V3F_C4B_T2F_Quad* quad = startQuad;
        float* r = mParticleData.colorR;
        float* g = mParticleData.colorG;
        float* b = mParticleData.colorB;
        float* a = mParticleData.colorA;
        
        for (int i = 0; i < _particleCount; ++i,++quad,++r,++g,++b,++a)
        {
            Color4B color( *r * 255, *g * 255, *b * 255, *a * 255);
            quad->bl.colors = color;
            quad->br.colors = color;
            quad->tl.colors = color;
            quad->tr.colors = color;
        }
    }

    

}



void ParticleSystemQuadSOA::updateQuadWithParticle(tParticle* particle, const Vec2& newPosition)
{
    assert(0&&"no need call");
}


// overriding draw method
void ParticleSystemQuadSOA::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    //CCASSERT( _particleIdx == 0 || _particleIdx == _particleCount, "Abnormal error in particle quad");
    //quad command
    
    if(_particleCount > 0)
    {
        _quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _quads, _particleCount, transform, flags);
        renderer->addCommand(&_quadCommand);
    }
}

void ParticleSystemQuadSOA::setTotalParticles(int tp)
{
    // If we are setting the total number of particles to a number higher
    // than what is allocated, we need to allocate new arrays
    if( tp > _allocatedParticles )
    {
        // Allocate new memory
        size_t quadsSize = sizeof(_quads[0]) * tp * 1;
        size_t indicesSize = sizeof(_indices[0]) * tp * 6 * 1;
        
        mParticleData.release();
        if( !mParticleData.init(tp) )
        {
            CCLOG("Particle system: out of memory");
            return;
        }
        
        V3F_C4B_T2F_Quad* quadsNew = (V3F_C4B_T2F_Quad*)realloc(_quads, quadsSize);
        GLushort* indicesNew = (GLushort*)realloc(_indices, indicesSize);
        
        if ( quadsNew && indicesNew)
        {
            // Assign pointers
           
            _quads = quadsNew;
            _indices = indicesNew;
            
            // Clear the memory
            memset(_quads, 0, quadsSize);
            memset(_indices, 0, indicesSize);
            
            _allocatedParticles = tp;
        }
        else
        {
            // Out of memory, failed to resize some array
            if (quadsNew) _quads = quadsNew;
            if (indicesNew) _indices = indicesNew;
            
            CCLOG("Particle system: out of memory");
            return;
        }
        
        _totalParticles = tp;
        
        // Init particles
        if (_batchNode)
        {
            for (int i = 0; i < _totalParticles; i++)
            {
                mParticleData.atlasIndex[i] = i;
            }
        }
        
        initIndices();
        if (Configuration::getInstance()->supportsShareableVAO())
        {
            setupVBOandVAO();
        }
        else
        {
            setupVBO();
        }
        
        // fixed http://www.cocos2d-x.org/issues/3990
        // Updates texture coords.
        updateTexCoords();
    }
    else
    {
        _totalParticles = tp;
    }
    
    // fixed issue #5762
    // reset the emission rate
    setEmissionRate(_totalParticles / _life);
    
    resetSystem();
}



void ParticleSystemQuadSOA::setBatchNode(ParticleBatchNode * batchNode)
{
    if( _batchNode != batchNode )
    {
        ParticleBatchNode* oldBatch = _batchNode;
        
            
        _batchNode = batchNode; // weak reference
        
        if( batchNode )
        {
            //each particle needs a unique index
            for (int i = 0; i < _totalParticles; i++)
            {
                mParticleData.atlasIndex[i] = i;
            }
        }
        //ParticleSystem::setBatchNode(batchNode);
        
        // NEW: is self render ?
        if( ! batchNode )
        {
            allocMemory();
            initIndices();
            setTexture(oldBatch->getTexture());
            if (Configuration::getInstance()->supportsShareableVAO())
            {
                setupVBOandVAO();
            }
            else
            {
                setupVBO();
            }
        }
        // OLD: was it self render ? cleanup
        else if( !oldBatch )
        {
            // copy current state to batch
            V3F_C4B_T2F_Quad *batchQuads = _batchNode->getTextureAtlas()->getQuads();
            V3F_C4B_T2F_Quad *quad = &(batchQuads[_atlasIndex] );
            memcpy( quad, _quads, _totalParticles * sizeof(_quads[0]) );
            
            CC_SAFE_FREE(_quads);
            CC_SAFE_FREE(_indices);
            
            glDeleteBuffers(2, &_buffersVBO[0]);
            memset(_buffersVBO, 0, sizeof(_buffersVBO));
            if (Configuration::getInstance()->supportsShareableVAO())
            {
                glDeleteVertexArrays(1, &_VAOname);
                GL::bindVAO(0);
                _VAOname = 0;
            }
        }
    }
}

ParticleSystemQuadSOA * ParticleSystemQuadSOA::create() {
    ParticleSystemQuadSOA *particleSystemQuad = new (std::nothrow) ParticleSystemQuadSOA();
    if (particleSystemQuad && particleSystemQuad->init())
    {
        particleSystemQuad->autorelease();
        return particleSystemQuad;
    }
    CC_SAFE_DELETE(particleSystemQuad);
    return nullptr;
}

std::string ParticleSystemQuadSOA::getDescription() const
{
    return StringUtils::format("<ParticleSystemQuadSOA | Tag = %d, Total Particles = %d>", _tag, _totalParticles);
}

NS_CC_END


