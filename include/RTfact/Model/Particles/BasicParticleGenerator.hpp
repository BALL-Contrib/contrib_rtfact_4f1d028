/*
 *  BasicParticleGenerator.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2010-12-12 21:05 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */

#ifndef RTFACT__BASIC_PARTICLE_GENERATOR_HPP
#define RTFACT__BASIC_PARTICLE_GENERATOR_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Concept/ParticleGenerator.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tpParticleStorage>
class BasicParticleGenerator :
    public ParticleGenerator<tpParticleStorage>
{
    RTFACT_PARAM_INIT(BasicParticleGenerator);

protected:
    uint mVPLSampleIndex;

public:

    RTFACT_PARAM_GS_GS(LightPathCount, uint, 10, "", "");
    RTFACT_PARAM_GS_GS(MaxPathLength, uint, 10, "", "");
    RTFACT_PARAM_GS_GS(ClampThreshold, float, 1.f, "", "");
    RTFACT_PARAM_GS_GS(ClampFactor, float, 1.f, "", "");
    RTFACT_PARAM_GS_GS(ClampStartValue, float, 1.f, "", "");
    RTFACT_PARAM_GS_GS(ParticlesGenerated, bool, false, "", "");

    BasicParticleGenerator()
    {
        initParams();
        mParticlesGenerated = false;
    }

    template<
        class tpCamera,
        class tpContext>
    RTFACT_PARTICLE_GENERATOR_INLINE void generateParticles(
        tpCamera& aCamera,
        tpContext& aContext,
        const uint aIteration)
    {
        mParticlesGenerated = true;

        if(aIteration == 0)
        {
            this->statistics.clear();
            mVPLSampleIndex = 0;
        }

        //mVPLNormalization = 0.f;
        //this->statistics.energyNormalizationFactor = 0.f;
        this->statistics.clampThreshold = Math::max(mClampThreshold,
            mClampStartValue * (aIteration * mClampFactor + 1.f));

        this->particleStorage.clear();
        this->particleStorage.reserveSize(mLightPathCount * mMaxPathLength);

        const size_t lightCount = aContext.scene.lights.size();

        //FPacket1 dummyDistance;
        ActiveMask<1> dummyMask;
        Vec3f1 dummyPoint;
        Vec3f1 dummyResult;

#ifdef RTFACT__DEBUG_PARTICLES
        TimeMeasurer m;
        m.start();
#endif
        if(lightCount > 0)
        {
            for(uint i = 0; i < mLightPathCount; ++i)
            {
                //++mVPLNormalization;
                //this->statistics.energyNormalizationFactor += 1.f;
                ++mVPLSampleIndex;
                uint primeNumberIndex = 20;

                // pick a light source
                const uint lightIndex = static_cast<uint>(
                    Math::haltonFaure(mVPLSampleIndex, primeNumberIndex) *
                    static_cast<float>(lightCount));

                ++primeNumberIndex;

                const Traits<Light>::Ptr& light = aContext.scene.lights[lightIndex];

                // query the light source for a position and an emission direction
                Vec3f1 lightPosition, lightDirection, radiance;
                FPacket1 lightProb;
                dummyMask.setTrueAndUpdateActiveContainers();

                Vec2f1 dirSamples = Math::haltonFaure2D(mVPLSampleIndex,
                                                primeNumberIndex, primeNumberIndex + 1);
                Vec2f1 posSamples = Math::haltonFaure2D(mVPLSampleIndex,
                                                primeNumberIndex + 2, primeNumberIndex + 3);

                light->sample(posSamples,
                              dirSamples,
                              dummyMask,
                              lightPosition,
                              lightDirection,
                              radiance,
                              lightProb);

                primeNumberIndex += 4;

                Vec3f1 cumulativeFactor = radiance * (static_cast<float>(lightCount) / lightProb);

                Ray ray(lightPosition, lightDirection, RTFACT_RAY_EPSILON);
                ray.rayType = RayType::ParticleRay;
                ray.depth = 0;
                ray.maxDepth = mMaxPathLength;
                ray.randomSeed = mVPLSampleIndex;
                ray.primeBase = primeNumberIndex;
                ray.cumulativeFactor = cumulativeFactor;
                dummyMask.setTrueAndUpdateActiveContainers();

                aContext.template trace<true, false>(ray, dummyMask, dummyResult);
            }
        }

        //mTotalVPLCount += aContext.particleStorage.numParticles();
        this->statistics.numInFrame = this->particleStorage.numParticles();
        this->statistics.numInAccuPeriod += this->statistics.numInFrame;

        //if(aIteration > 0)
        //{
            const float weight = 1.f / (aIteration + 1);
            const float weight1 = 1.f - weight;

            //mAvgVPLCount = weight1 * mAvgVPLCount + weight * aContext.particleStorage.numParticles();
            //mAvgVPLCount = Math::max(mAvgVPLCount, 0.01f);
            this->statistics.avgNumInFrame =
                Math::max(weight1 * this->statistics.avgNumInFrame + weight * this->particleStorage.numParticles(),
                          0.01f);
        //}

#ifdef RTFACT__DEBUG_PARTICLES
        m.stop();
        RTFACT_LOG_LINE_F("Stored %1% particles in %2% seconds", this->statistics.numInFrame % (m.getTotalTime() / 1000.0));
        RTFACT_LOG_LINE_F("Total particle count: %1%", this->statistics.numInAccuPeriod);
#endif
    }

    RTFACT_PARTICLE_GENERATOR_INLINE size_t getTotalParticleCount()
    {
        size_t totalParticleCount = this->statistics.numInAccuPeriod;
        return totalParticleCount;
    }

    RTFACT_PARTICLE_GENERATOR_INLINE void releaseParticles()
    {
        mParticlesGenerated = false;
    }

};

RTFACT_NAMESPACE_END

#endif // RTFACT__BASIC_PARTICLE_GENERATOR_HPP
