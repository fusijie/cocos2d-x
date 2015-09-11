//
//  CCParticleSystemQuadNeon.h
//  cocos2d_libs
//
//  Created by RUKD on 15/3/30.
//
//

#ifndef __cocos2d_libs__CCParticleSystemQuadSOA__
#define __cocos2d_libs__CCParticleSystemQuadSOA__

#include "2d/CCParticleSystemQuad.h"
//#include "2d/CCParticleSystemQuadSIMD.h"

NS_CC_BEGIN

class SpriteFrame;
class EventCustom;

class CC_DLL ParticleDataSOA
{
public:
    float* posx;
    float* posy;
    float* startPosX;
    float* startPosY;
    
    
    float* colorR;
    float* colorG;
    float* colorB;
    float* colorA;
    
    float* deltaColorR;
    float* deltaColorG;
    float* deltaColorB;
    float* deltaColorA;
    
    float* size;
    float* deltaSize;
    float* rotation;
    float* deltaRotation;
    float* timeToLive;
    unsigned int* atlasIndex;
    
    //! Mode A: gravity, direction, radial accel, tangential accel
    struct{
        float* dirX;
        float* dirY;
        float* radialAccel;
        float* tangentialAccel;
    } modeA;
    
    //! Mode B: radius mode
    struct{
        float* angle;
        float* degreesPerSecond;
        float* radius;
        float* deltaRadius;
    } modeB;
    unsigned int maxCount;
    ParticleDataSOA();
    bool init(int count);
    void release();
    inline unsigned int getMaxCount(){
        return maxCount;
    }
    
    inline void swapParticle(int p1,int p2)
    {
        posx[p1] = posx[p2];
        posy[p1] = posy[p2];
        startPosX[p1] = startPosX[p2];
        startPosY[p1] = startPosY[p2];
        
        colorR[p1] = colorR[p2];
        colorG[p1] = colorG[p2];
        colorB[p1] = colorB[p2];
        colorA[p1] = colorA[p2];
        
        deltaColorR[p1] = deltaColorR[p2];
        deltaColorG[p1] = deltaColorG[p2];
        deltaColorB[p1] = deltaColorB[p2];
        deltaColorA[p1] = deltaColorA[p2];
        
        size[p1] = size[p2];
        deltaSize[p1] = deltaSize[p2];
        
        rotation[p1] = rotation[p2];
        deltaRotation[p1] = deltaRotation[p2];
        
        timeToLive[p1] = timeToLive[p2];
        
        atlasIndex[p1] = atlasIndex[p2];
        
        modeA.dirX[p1] = modeA.dirX[p2];
        modeA.dirY[p1] = modeA.dirY[p2];
        modeA.radialAccel[p1] = modeA.radialAccel[p2];
        modeA.tangentialAccel[p1] = modeA.tangentialAccel[p2];
        
        modeB.angle[p1] = modeB.angle[p2];
        modeB.degreesPerSecond[p1] = modeB.degreesPerSecond[p2];
        modeB.radius[p1] = modeB.radius[p2];
        modeB.deltaRadius[p1] = modeB.deltaRadius[p2];
        
    }
};

/**
 * @addtogroup particle_nodes
 * @{
 */

/** @brief ParticleSystemQuad is a subclass of ParticleSystem
 
 It includes all the features of ParticleSystem.
 
 Special features and Limitations:
 - Particle size can be any float number.
 - The system can be scaled
 - The particles can be rotated
 - It supports subrects
 - It supports batched rendering since 1.1
 @since v0.8
 */
class CC_DLL ParticleSystemQuadSOA : public ParticleSystemQuad
{
public:
    
    /** creates a Particle Emitter */
    static ParticleSystemQuadSOA * create();
    /** creates a Particle Emitter with a number of particles */
    static ParticleSystemQuadSOA * createWithTotalParticles(int numberOfParticles);
    /** creates an initializes a ParticleSystemQuad from a plist file.
     This plist files can be created manually or with Particle Designer:
     */
    static ParticleSystemQuadSOA * create(const std::string& filename);
    /** creates a Particle Emitter with a dictionary */
    static ParticleSystemQuadSOA * create(ValueMap &dictionary);
    
    /** Sets a new SpriteFrame as particle.
     WARNING: this method is experimental. Use setTextureWithRect instead.
     @since v0.99.4
     */
    void setDisplayFrame(SpriteFrame *spriteFrame);
    
    /** Sets a new texture with a rect. The rect is in Points.
     @since v0.99.4
     * @js NA
     * @lua NA
     */
    void setTextureWithRect(Texture2D *texture, const Rect& rect);
    
    /** listen the event that renderer was recreated on Android/WP8
     * @js NA
     * @lua NA
     */
    void listenRendererRecreated(EventCustom* event);
    
    /**
     * @js NA
     * @lua NA
     */
    virtual void setTexture(Texture2D* texture) override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void updateQuadWithParticle(tParticle* particle, const Vec2& newPosition) override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void postStep() override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    
    /**
     * @js NA
     * @lua NA
     */
    virtual void setBatchNode(ParticleBatchNode* batchNode) override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void setTotalParticles(int tp) override;
    
    virtual std::string getDescription() const override;
public:
    //neon overide
    virtual bool addParticle();
    virtual bool addParticles(int count);
    virtual bool initParticleByIndex(int index);
    virtual void resetSystem();
    
    virtual void update(float dt) override;
    void updateVertexData();
    
    
CC_CONSTRUCTOR_ACCESS:
    /**
     * @js ctor
     */
    ParticleSystemQuadSOA();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ParticleSystemQuadSOA();
    
    // Overrides
    /**
     * @js NA
     * @lua NA
     */
    virtual bool initWithTotalParticles(int numberOfParticles) override;
    
protected:

    bool allocMemory();
    
    ParticleDataSOA    mParticleData;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(ParticleSystemQuadSOA);
};

// end of particle_nodes group
/// @}

NS_CC_END

#endif /* defined(__cocos2d_libs__CCParticleSystemQuadSOA__) */
