/*
 *  GlobalImportanceParticleFilter.hpp
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
 *  Created on: 2010-16-14 18:09 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */

#ifndef RTFACT__GLOBAL_IMPORTANCE_PARTICLE_FILTER_HPP
#define RTFACT__GLOBAL_IMPORTANCE_PARTICLE_FILTER_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Concept/ParticleFilter.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tpImportonStorage,
    class tpMutex>
class GlobalImportanceParticleFilter :
    public ParticleFilter<tpImportonStorage, tpMutex>
{
protected:
    typedef GlobalImportanceParticleFilter<tpImportonStorage, tpMutex> thisClass;
    RTFACT_PARAM_INIT(thisClass);
    
public:

    RTFACT_PARAM_GS_GS(AcceptanceEpsilon, float, 0.08f, "", "");
    RTFACT_PARAM_GS_GS(ImportonCount, uint, 30, "", "");
    RTFACT_PARAM_GS_GS(GBias, float, 0.f, "", "");

    GlobalImportanceParticleFilter() :
        mAcceptanceSampleIndex(0),
        mImportonSampleIndex(0)
    {
        initParams();
    }

    template <
        class tpParticle,
        class tpContext>
    RTFACT_PARTICLE_FILTER_INLINE bool acceptParticle(
        tpParticle& aParticle,
        tpContext& aContext)
    {
        if((mAcceptanceEpsilon >= 1.f) || (this->importonStorage.numParticles() == 0))
        {
            return true;
        }

        const float avgContribution = calcAveragePtoPContribution(aContext, aParticle);

        float acceptanceProbability = Math::min(
            avgContribution / this->stats.desiredAvgContrib + mAcceptanceEpsilon, 1.f);

        RTFACT_PARTICLE_FILTER_ASSERT(!Math::isNaN(acceptanceProbability));

        if(Math::haltonFaure(mAcceptanceSampleIndex++, 1) < acceptanceProbability)
        {
            aParticle.setIntensity(aParticle.getIntensity() / acceptanceProbability);
            return true;
        }
        return false;
    }

    template<
        class tpCamera,
        class tpContext>
    RTFACT_PARTICLE_FILTER_INLINE void startFrame(
        tpCamera& aCamera,
        tpContext& aContext,
        const uint aIteration)
    {

#ifdef RTFACT__DEBUG_IMPORTONS
        TimeMeasurer m;
        m.start();
#endif

        typename tpCamera::Params camParams;
        aCamera.getParams(camParams);

        const float resX = static_cast<float>(camParams.resX);
        const float resY = static_cast<float>(camParams.resY);        
        if(aIteration > 0)
        {
            //const float weight = 1.f / (aIteration + 1);
            const float weight = 1.f / aIteration;
            const float weight1 = 1.f - weight;

            //typename tpCamera::Params camParams;
            //aCamera.getParams(camParams);

            //const float resX = static_cast<float>(camParams.resX);
            //const float resY = static_cast<float>(camParams.resY);

            const float frameAvgPixelLum =
                    this->stats.getTotalFrameLuminance() / (resX * resY / aContext.particleGenerator.getLightPathCount());
            this->stats.avgPixelLuminance = Math::max(weight1 * this->stats.avgPixelLuminance + weight * frameAvgPixelLum, 0.0001f);

            this->stats.desiredAvgContrib = this->stats.avgPixelLuminance / aContext.particleGenerator.statistics.avgNumInFrame;            
        }
        else
        //if(aIteration == 0)        
        {
            mImportonSampleIndex = 0;
            mAcceptanceSampleIndex = 0;
            this->stats.clear();
        }
        this->stats.setTotalFrameLuminance(0.f);

        if(mAcceptanceEpsilon < 1.f)
        {
            this->importonStorage.clear();
            this->importonStorage.reserveSize(mImportonCount);

            for(uint i = 0; i < mImportonCount; i++)
            {
                ++mImportonSampleIndex;

                PixelSampler::Sample<1> sample;

                sample.imageX = Math::haltonFaure(mImportonSampleIndex, 5) * resX;
                sample.imageY = Math::haltonFaure(mImportonSampleIndex, 6) * resY;

                Ray ray = aCamera.generateRay(sample);
                ray.rayType = RayType::ImportonRay;
                Vec3f1 dummyResult;
                ActiveMask<1> dummyMask;
                dummyMask.setTrueAndUpdateActiveContainers();
                aContext.template trace<true, false>(ray, dummyMask, dummyResult);
            }
        }

#ifdef RTFACT__DEBUG_IMPORTONS
        m.stop();
        RTFACT_LOG_LINE_F("Stored %1% importons in %2% seconds",
            aContext.particleFilter.importonStorage.numParticles() % (m.getTotalTime() / 1000.0));
#endif
    }

    template<
        class tpCamera,
        class tpContext>
    RTFACT_PARTICLE_FILTER_INLINE void endFrame(
        tpCamera& aCamera,
        tpContext& aContext,
        const uint aIteration)
    {
        //
    }

protected:

        uint mAcceptanceSampleIndex;
        uint mImportonSampleIndex;

        template<
        class tpContext,
        class tpParticle>
    float calcPtoPContribution(
        tpContext& aContext,
        const tpParticle& aP1,
        const tpParticle& aP2) const
    {
        const Vec3f1 dirToP2 = aP2.getPosition() - aP1.getPosition();
        const float distance2 = dirToP2.lengthSqr();
        const float distance = Math::sqrt(distance2);
        const Vec3f1 dirToP2Norm = dirToP2 / distance;

        const float dotP1Norm =
            Math::max(Math::dot(dirToP2Norm, aP1.getNormal()), 0.f);

        const float dotP2Norm =
            Math::max(-Math::dot(dirToP2Norm, aP2.getNormal()), 0.f);

        const float geomTerm = dotP1Norm * dotP2Norm / (mGBias + distance2);

        float contribution = 0.f;

        if(geomTerm > 0.f)
        {
            Ray ray(
                aP1.getPosition(), dirToP2Norm,
                distance, RTFACT_RAY_EPSILON);
            
            ActiveMask<1> dummyMask;
            dummyMask.setTrueAndUpdateActiveContainers();
            ActiveMask<1> shadowMask;
            Vec3f1 shadowIntensity;            
            aContext.template getLightContribution<true>(
                ray, dummyMask, shadowMask, shadowIntensity);
            //typename tpContext::Intersector::template Result<1> intersection;
            //aContext.template intersect<IsectData::NONE, true>(ray, mask, intersection);
            //if(intersection.mask.isFalse())
            if(shadowMask.isFalse())
            {
                contribution = (geomTerm * aP1.getIntensity() * aP2.getIntensity()).luminance();
            }
        }
        return contribution;
    }

    template<
        class tpContext,
        class tpParticle>
    float calcAveragePtoPContribution(
        tpContext& aContext,
        const tpParticle& aParticle)
    {
        float contribution = 0.f;

        typename tpImportonStorage::tParticle importon;
        typename tpImportonStorage::tParticleQuery iqHandle;
        this->importonStorage.initParticleQuery(iqHandle);
        while(this->importonStorage.getNextParticle(iqHandle, importon))
        {
            contribution += calcPtoPContribution(aContext, aParticle, importon);
        }

        return contribution / this->importonStorage.numParticles();
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__GLOBAL_IMPORTANCE_PARTICLE_FILTER_HPP
