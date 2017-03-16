/* 
 *  ProjectionTriangleIntersector.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__PROJECTIONTRIANGLEINTERSECTOR_HPP
#define RTFACT__PROJECTIONTRIANGLEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>
#include <RTfact/Utils/Containers/Vector.hpp>

#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Model/Scene/BasicScene.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>

RTFACT_NAMESPACE_BEGIN

class ProjectionTriangleIntersector :
    public BasicIntersector<Triangle>
{
    struct RTFACT_ALIGN_SIMD FaceAccel
    {
        RTFACT_SIMD_ALIGNED_NEW_OPERATORS

        float nUK; // normal[u] / normal[k]
        float nVK; // normal[v] / normal[k]
        float nD;  // constant of plane equation
        int k;     // projection dimension

        // line equation for line AC
        float bNU;
        float bNV;
        float bD;
        float nK;

        // line equation for line AB
        float cNU;
        float cNV;
        float cD;
        int appearanceID;
    };

    Triangle* mElementBase;
    Vector<FaceAccel> mAccels;

public:

    template<
        class tAppearanceConfig,
        class tLightConfig>
    void init(
        BasicScene<tAppearanceConfig, tLightConfig, Triangle>& aScene)
    {
        Intersector::init(aScene);

        const size_t faceCount = aScene.primitives.size();

        mElementBase = &(aScene.primitives[0]);

        mAccels.resize(faceCount);

        static const uint mod3[] = {0, 1, 2, 0, 1};

        #define RTFACT__K k
        #define RTFACT__U mod3[k + 1]
        #define RTFACT__V mod3[k + 2]

        for(uint i = 0; i < faceCount; ++i)
        {
            const Vec3f<1> a = aScene.primitives[i].vertices[0];
            const Vec3f<1> b = aScene.primitives[i].vertices[1];
            const Vec3f<1> c = aScene.primitives[i].vertices[2];
            const uint appearanceID = aScene.primitives[i].appearanceID;
            const Vec3f<1> normal = Math::cross(b - a, c - a).normalize();

            #define RTFACT__NDIM(aIndex) Math::abs(normal.data[aIndex])

            const uint k = (RTFACT__NDIM(0) > RTFACT__NDIM(1)) ?
                ((RTFACT__NDIM(0) > RTFACT__NDIM(2)) ? 0 : 2) :
                ((RTFACT__NDIM(1) > RTFACT__NDIM(2)) ? 1 : 2);

            #undef RTFACT__NDIM

            FaceAccel& accel = mAccels[i];
            float denom;

            accel.k   = k;
            accel.nUK = normal.data[RTFACT__U] / normal.data[RTFACT__K];
            accel.nVK = normal.data[RTFACT__V] / normal.data[RTFACT__K];
            accel.nD  = a.data[RTFACT__U] * accel.nUK +
                        a.data[RTFACT__V] * accel.nVK + a.data[RTFACT__K];

            accel.cNU =   b.data[RTFACT__V] - a.data[RTFACT__V];
            accel.cNV = - b.data[RTFACT__U] + a.data[RTFACT__U];
            accel.cD  = -(a.data[RTFACT__U] * accel.cNU +
                          a.data[RTFACT__V] * accel.cNV);

            denom = 1.f / (c.data[RTFACT__U] * accel.cNU +
                           c.data[RTFACT__V] * accel.cNV + accel.cD);

            accel.cNU *= denom;
            accel.cNV *= denom;
            accel.cD  *= denom;

            accel.bNU =   c.data[RTFACT__V] - a.data[RTFACT__V];
            accel.bNV = - c.data[RTFACT__U] + a.data[RTFACT__U];
            accel.bD  = -(a.data[RTFACT__U] * accel.bNU +
                          a.data[RTFACT__V] * accel.bNV);

            denom = 1.f / (b.data[RTFACT__U] * accel.bNU +
                           b.data[RTFACT__V] * accel.bNV + accel.bD);

            accel.bNU *= denom;
            accel.bNV *= denom;
            accel.bD  *= denom;

            accel.nK = normal.data[RTFACT__K];
            accel.appearanceID = appearanceID;
        }

        #undef RTFACT__K
        #undef RTFACT__U
        #undef RTFACT__V
    }

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const Triangle& aTriangle, 
        Result<taSize>& oResult) const
    {
        static const uint mod3[] = {0, 1, 2, 0, 1};

        typedef Packet<taSize, float>            t_FPacket;
        typedef typename t_FPacket::Container    t_FPacketC;
        typedef Packet<taSize, int>              t_IPacket;
        typedef typename t_IPacket::Container    t_IPacketC;
        typedef Vec3f<taSize>                    t_Vec3f;
        typedef typename t_Vec3f::Container      t_Vec3fC;
        typedef Mask<taSize>                     t_Mask;
        typedef typename Mask<taSize>::Container t_MaskC;

        const FaceAccel& faceAccel = mAccels[&aTriangle - mElementBase];

        #define RTFACT__K faceAccel.k
        #define RTFACT__U mod3[RTFACT__K + 1]
        #define RTFACT__V mod3[RTFACT__K + 2]

        const t_FPacketC nUK = t_FPacketC::replicate(faceAccel.nUK);
        const t_FPacketC nVK = t_FPacketC::replicate(faceAccel.nVK);
        const t_FPacketC nD  = t_FPacketC::replicate(faceAccel.nD);
        const t_FPacketC bNU = t_FPacketC::replicate(faceAccel.bNU);
        const t_FPacketC bNV = t_FPacketC::replicate(faceAccel.bNV);
        const t_FPacketC bD  = t_FPacketC::replicate(faceAccel.bD);
        const t_FPacketC cNU = t_FPacketC::replicate(faceAccel.cNU);
        const t_FPacketC cNV = t_FPacketC::replicate(faceAccel.cNV);
        const t_FPacketC cD  = t_FPacketC::replicate(faceAccel.cD);

        t_Vec3fC normal;

        if(taIsectData)
        {
            normal.get(RTFACT__K) = t_FPacketC::C_1;
            normal.get(RTFACT__U) = nUK;
            normal.get(RTFACT__V) = nVK;
            normal *= faceAccel.nK;
        }

        t_IPacketC appearanceID = t_IPacketC::replicate(aTriangle.appearanceID);

        t_FPacketC fPart;

        if(taCommonOrg)
        {
            fPart = nD - aRayPacket.org(0).get(RTFACT__K) -
                    nUK * aRayPacket.org(0).get(RTFACT__U) -
                    nVK * aRayPacket.org(0).get(RTFACT__V);
        }

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            const t_FPacketC nd =
                Math::reciprocal(aRayPacket.dir(i).get(RTFACT__K) +
                                 nUK * aRayPacket.dir(i).get(RTFACT__U) +
                                 nVK * aRayPacket.dir(i).get(RTFACT__V));

            if(!taCommonOrg)
            {
                fPart = nD - aRayPacket.org(i).get(RTFACT__K) -
                        nUK * aRayPacket.org(i).get(RTFACT__U) -
                        nVK * aRayPacket.org(i).get(RTFACT__V);
            }

            const t_FPacketC f = nd * fPart;

            t_MaskC mask = aRayMask(i) &
                                 (f >= t_FPacketC::C_RAY_EPS) &
                                 (f < oResult.dist(i) &
                                 (f < aRayPacket.tMax(i)));

            if(mask.isFalse())
            {
                continue;
            }

            t_FPacketC hu, hv;

            if(taCommonOrg)
            {
                hu = aRayPacket.org(0).get(RTFACT__U) +
                     f * aRayPacket.dir(i).get(RTFACT__U);
                hv = aRayPacket.org(0).get(RTFACT__V) +
                     f * aRayPacket.dir(i).get(RTFACT__V);                
            }
            else
            {
                hu = aRayPacket.org(i).get(RTFACT__U) +
                     f * aRayPacket.dir(i).get(RTFACT__U);
                hv = aRayPacket.org(i).get(RTFACT__V) +
                     f * aRayPacket.dir(i).get(RTFACT__V);                
            }

            const t_FPacketC lambda = hu * bNU + hv * bNV + bD; 
            const t_FPacketC mue = hu * cNU + hv * cNV + cD; 

            mask &= (lambda >= t_FPacketC::C_0()) &
                    (mue >= t_FPacketC::C_0()) &
                    (mue + lambda <= t_FPacketC::C_1);

            oResult.mask(i) |= mask;

            if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
            {
                if(!mask.isFalse())
                {
                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DISTANCE))
                    {
                        oResult.dist(i)   = mask.iif(f, oResult.dist(i));
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                    {
                        oResult.u(i)      = mask.iif(lambda, oResult.u(i));
                        oResult.v(i)      = mask.iif(mue, oResult.v(i));
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::APPEARANCE_ID))
                    {
                        oResult.appearanceID(i) = mask.iif(appearanceID, oResult.appearanceID(i));
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                    {
                        oResult.normal(i) = mask.iif(normal, oResult.normal(i));
                    }
                }
            }
        }

        #undef RTFACT__K
        #undef RTFACT__U
        #undef RTFACT__V
    }

    template<
        umask taIsectData,
        bool taCommonOrg>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<1>& aRayPacket,
        const ActiveMask<1>& aRayMask,
        const Triangle& aTriangle, 
        Result<1>& oResult) const
    {
        static const uint mod3[] = {0, 1, 2, 0, 1};

        const FaceAccel& faceAccel = mAccels[&aTriangle - mElementBase];
        
        #define RTFACT__K faceAccel.k
        #define RTFACT__U mod3[RTFACT__K + 1]
        #define RTFACT__V mod3[RTFACT__K + 2]

        const float nd = Math::reciprocal(aRayPacket.dir.get(RTFACT__K) +
                         faceAccel.nUK * aRayPacket.dir.get(RTFACT__U) +
                         faceAccel.nVK * aRayPacket.dir.get(RTFACT__V));

        const float f = nd * (faceAccel.nD - aRayPacket.org.get(RTFACT__K)
                                - faceAccel.nUK * aRayPacket.org.get(RTFACT__U)
                                - faceAccel.nVK * aRayPacket.org.get(RTFACT__V));

        bool mask = (f >= RTFACT_RAY_EPSILON) & (f < oResult.dist);

        if(!mask)
        {
            return;
        }

        const float hu = aRayPacket.org.get(RTFACT__U) +
                         f * aRayPacket.dir.get(RTFACT__U);

        const float hv = aRayPacket.org.get(RTFACT__V) +
                         f * aRayPacket.dir.get(RTFACT__V);    

        const float lambda = hu * faceAccel.bNU +
                             hv * faceAccel.bNV +
                             faceAccel.bD; 

        const float mue = hu * faceAccel.cNU +
                          hv * faceAccel.cNV + faceAccel.cD; 

        mask &= (lambda >= 0.f) & (mue >= 0.f) & (mue + lambda <= 1.f);
        
        if(taIsectData)
        {
            if(mask)
            {
                Vec3f<1> normal;
                normal.get(RTFACT__K) = 1.f;
                normal.get(RTFACT__U) = faceAccel.nUK;
                normal.get(RTFACT__V) = faceAccel.nVK;
                normal *= faceAccel.nK;

                oResult.mask       = RTFACT_MASK_TRUE;
                oResult.dist       = f;
                oResult.u          = lambda;
                oResult.v          = mue;
                oResult.appearanceID = aTriangle.appearanceID;
                oResult.normal     = normal;
            }
        }
        else
        {
            oResult.mask |= mask & (f < aRayPacket.tMax);
        }

        #undef RTFACT__K
        #undef RTFACT__U
        #undef RTFACT__V
    }
};

#undef RTFACT__ASSERT_RENDER_STRUCT_VALID

RTFACT_NAMESPACE_END

#endif // RTFACT__PROJECTIONTRIANGLEINTERSECTOR_HPP
