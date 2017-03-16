/* 
 *  SurfaceShaderContext.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
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
 *  Created on: 2009-03-06 23:28:11 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Lukas Marsalek
 */
 
 #ifndef RTFACT__SURFACESHADERCONTEXT_HPP
#define RTFACT__SURFACESHADERCONTEXT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Array.hpp>
#include <RTfact/Utils/SurfaceIntersection.hpp>
#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Concept/Scene/ShadingInfoScene.hpp>
#include <RTfact/Concept/ParticleFilter.hpp>
#include <RTfact/Concept/ParticleGenerator.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tScene,
    class tPrimitive,
    class tIntersector,
    class tParticleGenerator,
    class tParticleFilter>
class SurfaceShaderContext
{
    enum
    {
        INTERSECTION_DATA =
            IsectData::DISTANCE |
            IsectData::NORMAL |
            IsectData::APPEARANCE_ID |
            IsectData::TEX_COORDS |
            IsectData::VERTEX_COLOR |
            IsectData::TANGENT_SPACE
    };

public:

    typedef tScene Scene;
    typedef tPrimitive Primitive;
    typedef tIntersector Intersector;
    typedef tParticleGenerator ParticleGenerator;
    typedef tParticleFilter ParticleFilter;

    typedef typename tParticleGenerator::tParticleStorage::tParticle tParticle;
    typedef typename tParticleGenerator::tParticleStorage::tParticleQuery tParticleQuery;

    typedef typename tParticleFilter::tImportonStorage::tParticle tImporton;    
    typedef typename tParticleFilter::tImportonStorage::tParticleQuery tImportonQuery;

    tScene&       scene;
    tPrimitive&   primitive;
    tIntersector& intersector;
    float         time; // in miliseconds
    union {
        void * p;
        int i;
        float f;
    } userData;
    tParticleGenerator& particleGenerator;
    tParticleFilter& particleFilter;

    SurfaceShaderContext(
        tScene& aScene,
        tPrimitive& aPrimitive,
        tIntersector& aIntersector,
        tParticleGenerator& aParticleGenerator,
        tParticleFilter& aParticleFilter
    ) :
        scene(aScene),
        primitive(aPrimitive),
        intersector(aIntersector),
        time(0),
        particleGenerator(aParticleGenerator),
        particleFilter(aParticleFilter)
    {
        userData.p = 0;
    }

    
    RTFACT_APPEARANCE_INLINE void assessParticle(
        tParticle& aParticle)
    {
        if(particleFilter.acceptParticle(aParticle, *this))
        {
            particleGenerator.particleStorage.insertParticle(aParticle);
        }
    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void shade(
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        typename tIntersector::template Result<taSize>& aIntersection,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL)
    {
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);

        typedef Packet<taSize, float>         t_FPacket;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef ActiveMask<taSize>      t_ActiveMask;

        Array<taSize, int> uniqueShaders;
        typename Array<taSize, int>::iterator uniqueShaderEnd =
            aIntersection.appearanceID.getUniqueValues(
            uniqueShaders.begin(), uniqueShaders.end(), aIntersection.mask);

        //STOP_TIMER(1);

        for(typename Array<taSize, int>::iterator shaderId = uniqueShaders.begin();
            shaderId != uniqueShaderEnd; ++shaderId)
        {
            t_IPacketC shaderIdC = t_IPacketC::replicate(*shaderId);

            t_ActiveMask shaderMask;

            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
            {
                shaderMask(i) = aIntersection.mask(i) &
                                (aIntersection.appearanceID(i) == shaderIdC);
            }

            shaderMask.updateActiveContainers();

            scene.appearances[*shaderId]->template shade<taCommonOrg, taStoreAlpha>(
                aRayPacket, shaderMask, aIntersection, *this, oResult, oResultAlpha);
        }

        aIntersection.mask = (~aIntersection.mask) & aRayMask;
        aIntersection.mask.updateActiveContainers();

        scene.environmentTexture.template sample<taStoreAlpha, taCommonOrg>(
            aRayPacket, aIntersection.mask, oResult, oResultAlpha);
    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void trace(
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL)
    {
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);

        typename tIntersector::template Result<taSize> intersection;

        RTfact::Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            intersector, aRayPacket, aRayMask, primitive, intersection);

        shade<taCommonOrg, taStoreAlpha>(aRayPacket, aRayMask, intersection, oResult, oResultAlpha);
    }

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void getLightContribution(
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        ActiveMask<taSize>& oMask,
        Vec3f<taSize>& oIntensity)
    {
        RTFACT_STATIC_CONSTANT(TRANSPARENCY_INTERSECTION_DATA,
            IsectData::APPEARANCE_ID |
            IsectData::DISTANCE |
            IsectData::TEX_COORDS);

        typedef Packet<taSize, float>         t_FPacket;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef ActiveMask<taSize>            t_ActiveMask;

        if(RTFACT_CHECK_UMASK(tScene::PROPERTIES, SceneProperties::HAS_TRANSPARENCY))
        {
            oMask = aRayMask;

            for(;;)
            {
                typename tIntersector::template Result<taSize> intersection;

                RTfact::Intersector::intersect<TRANSPARENCY_INTERSECTION_DATA, taCommonOrg>(
                    intersector, aRayPacket, oMask, primitive, intersection);

                if(intersection.mask.isFalse())
                {
                    break;
                }

                Array<taSize, int> uniqueShaders;
                typename Array<taSize, int>::iterator uniqueShaderEnd =
                    intersection.appearanceID.getUniqueValues(
                        uniqueShaders.begin(), uniqueShaders.end(), intersection.mask);

                for(typename Array<taSize, int>::iterator shaderId = uniqueShaders.begin();
                    shaderId != uniqueShaderEnd; ++shaderId)
                {
                    t_IPacketC shaderIdC = t_IPacketC::replicate(*shaderId);

                    t_ActiveMask shaderMask;

                    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                    {
                        shaderMask(i) = intersection.mask(i) &
                            (intersection.appearanceID(i) == shaderIdC);
                    }

                    shaderMask.updateActiveContainers();

                    scene.appearances[*shaderId]->attenuateLight(
                        shaderMask, intersection, oMask, oIntensity);
                }

                oMask.clearActiveContainerEndIndex();

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
                    // FIX: add check for cumulative alpha < epsilon
                    oMask.updateActiveContainerEndIndex(i);
                }

                if(aRayPacket.depth == 10 || oMask.isFalse())
                {
                    break;
                }

                ++aRayPacket.depth;

                aRayPacket.tMin = intersection.dist + t_FPacket::C_RAY_EPS;
            }
        }
        else
        {
            typename tIntersector::template Result<taSize> intersection;

            RTfact::Intersector::intersect<IsectData::NONE, taCommonOrg>(
                intersector, aRayPacket, aRayMask, primitive, intersection);

            oMask.clearActiveContainerEndIndex();

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                oMask(i) = aRayMask(i) & ~intersection.mask(i);
                oMask.updateActiveContainerEndIndex(i);
            }
        }
    }

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_FORCE_INLINE void intersect(
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        typename tIntersector::template Result<taSize>& oResult)
    {
        RTfact::Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            intersector, aRayPacket, aRayMask, primitive, oResult);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SURFACESHADERCONTEXT_HPP
