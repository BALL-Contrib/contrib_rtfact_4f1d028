/*
 *  MultiShaderInstanceIntersector.hpp
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
 *  Created on: 2010-09-15 17:54:13 +0200
 *  Author(s): Felix Klein
 */

 #ifndef RTFACT__INSTANCEINTERSECTOR_HPP
#define RTFACT__INSTANCEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/RayPacket.hpp>
#include <RTfact/Utils/Containers/Array.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tInstance,
    class tIntersector>
class InstanceIntersector :
    public tIntersector
{
public:

    template<
        uint taSize>
    class Result :
        public tIntersector::template Result<taSize>
    {
        typedef typename tIntersector::template Result<taSize> t_Base;

    public:

        using t_Base::SIZE;
        using t_Base::mask;
        using t_Base::dist;
        using t_Base::u;
        using t_Base::v;
        using t_Base::appearanceID;
        using t_Base::geomNormal;
        using t_Base::normal;

        tInstance*         instanceBase;
        Packet<taSize, int> instanceOffsets;

        void initBase(
            tInstance& aInstanceBase)
        {
            instanceBase = &aInstanceBase;
        }

        int getOffset(
            tInstance& aInstance) const
        {
            return static_cast<int>(&aInstance - instanceBase);
        }
    };

    TimeMeasurer*
        intersectTime,
        *partATime,
        *partBTime,
        *partCTime;

    #ifdef RTFACT_INSTANT_INTERSECTOR_DEBUG

        int filterInstance;
        int filterGeometry;
        mutable int instanceHitCount;

    #endif

    InstanceIntersector()
    {
        intersectTime = new TimeMeasurer();
        partATime = new TimeMeasurer();
        partBTime = new TimeMeasurer();
        partCTime = new TimeMeasurer();

        #ifdef RTFACT_INSTANT_INTERSECTOR_DEBUG

            filterInstance = 0;
            filterGeometry = 0;
            instanceHitCount = 0;

        #endif
    }

    ~InstanceIntersector()
    {
        delete intersectTime;
        delete partATime;
        delete partBTime;
        delete partCTime;
    }

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        tInstance& aInstance,
        Result<taSize>& oResult,
        uint aI = 0) const
    {
        typedef Packet<taSize, float>         t_Packet;
        typedef Packet<taSize, int>           t_IPacket;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;
        typedef BitMask<taSize>               t_BitMask;
        typedef ActiveMask<taSize>            t_ActiveMask;
        typedef typename t_Packet::Container  t_PacketC;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        if(aRayPacket.rayType == RayType::ShadowRay && !aInstance.mDropShadow)
            return;

        RayPacket<taSize> newRayPacket = aRayPacket;

        if(taCommonOrg)
        {
            newRayPacket.org(0) =
               aInstance.mXfm.applyInverseToPointAffine(aRayPacket.org(0));
        }

        t_Packet oldResultDist, scaleLength, scaleLengthRecip;
        t_Mask oldResultMask;

        t_Packet tMaxCut, tMinCut;
        t_Packet tMaxOutPlane = -t_Packet::C_INFINITY;
        t_Packet tMinInPlane = t_Packet::C_INFINITY;

        t_IPacket kMaxOutCut = t_IPacket::replicate(-1);
        t_IPacket kMinInCut = t_IPacket::replicate(-1);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            if(!taCommonOrg)
            {
                newRayPacket.org(i) =
                    aInstance.mXfm.applyInverseToPointAffine(aRayPacket.org(i));
            }

            newRayPacket.dir(i) =
                aInstance.mXfm.applyInverseToVectorAffine(aRayPacket.dir(i));

            scaleLength(i) = newRayPacket.dir(i).length();
            scaleLengthRecip(i) = Math::reciprocal(scaleLength(i));

            newRayPacket.dir(i) *= scaleLengthRecip(i);

            newRayPacket.invDir(i) = newRayPacket.dir(i).reciprocal();

            newRayPacket.tMin(i) *= scaleLength(i);

            oldResultDist(i) = oResult.dist(i);
            oldResultMask(i) = oResult.mask(i);

            oResult.mask(i) = t_MaskC::C_FALSE();

            oResult.dist(i) =
                (aRayMask(i) & (oResult.dist(i) < t_PacketC::C_INFINITY)).iif(
                    scaleLength(i) * oResult.dist(i),
                    oResult.dist(i));

            // cut planes
            if(aInstance.mCutPlaneNorms.size()>0)
            {
                tMaxCut(i) = oResult.dist(i);
                tMinCut(i) = newRayPacket.tMin(i);

                for(int k = 0; k < aInstance.mCutPlaneNorms.size(); k++)
                {
                    RTFACT_PACKET_FOR_ALL(j, t_Vec3fC::SIZE)
                    {
                        // cut plane decision rule
                        float rayorg_plane = 0;
                        if(taCommonOrg) {
                            rayorg_plane = newRayPacket.org(0).x[j] * aInstance.mCutPlaneNorms[k].x +
                                           newRayPacket.org(0).y[j] * aInstance.mCutPlaneNorms[k].y +
                                           newRayPacket.org(0).z[j] * aInstance.mCutPlaneNorms[k].z + aInstance.mCutPlaneShifts[k];
                        } else {
                            rayorg_plane = newRayPacket.org(i).x[j] * aInstance.mCutPlaneNorms[k].x +
                                           newRayPacket.org(i).y[j] * aInstance.mCutPlaneNorms[k].y +
                                           newRayPacket.org(i).z[j] * aInstance.mCutPlaneNorms[k].z + aInstance.mCutPlaneShifts[k];
                        }

                        float raydir_dot_plane =  newRayPacket.dir(i).x[j] * aInstance.mCutPlaneNorms[k].x +
                                                  newRayPacket.dir(i).y[j] * aInstance.mCutPlaneNorms[k].y +
                                                  newRayPacket.dir(i).z[j] * aInstance.mCutPlaneNorms[k].z;
                        float t = -(rayorg_plane) / (raydir_dot_plane);

                        // most distant outward plane
                        if(raydir_dot_plane > 0 && tMaxOutPlane(i)[j] < t && t > 0)
                        {
                          tMaxOutPlane(i)[j] = t; //maxOut
                          kMaxOutCut(i)[j] = k;
                        }

                        // closest inward plane
                        if(raydir_dot_plane < 0 && tMinInPlane(i)[j] > t)
                        {
                          tMinInPlane(i)[j] = t; //minIn
                          kMinInCut(i)[j] = k;
                        }

                        // ray interval
                        if((rayorg_plane > 0) || (rayorg_plane == 0 && raydir_dot_plane < 0)) {
                            if(t >= 0)
                                if(t < tMaxCut(i)[j]) {
                                    tMaxCut(i)[j] = t;
                                }
                        } else  {
                            if(t < 0)
                                tMaxCut(i)[j] = 0;
                            else {
                                if(t >= tMinCut(i)[j]) {
                                    tMinCut(i)[j] = t;
                                }
                            }
                        }
                    }
                }
            }
        }

        Result<taSize> forwardResult = oResult;
        Result<taSize> backwardResult = oResult;

        t_IPacketC forwardOffsetC, backwardOffsetC;

        RayPacket<taSize> backwardRayPacket;

        if(aInstance.mCutPlaneNorms.size() > 0)
        {
            //if caps - intersection without cut
            if(aInstance.mCutPlaneCap)
            {
                //forward direction
                RayPacket<taSize> forwardRayPacket = newRayPacket;
                forwardRayPacket.tMin = Packet<taSize, float>::replicate(0);
                forwardResult.dist = t_Packet::C_INFINITY;
                forwardRayPacket.template init<taCommonOrg>(
                    aRayMask, aInstance.mStructure->getBounds());

                tIntersector::template intersect<taIsectData, taCommonOrg>(
                    forwardRayPacket,
                    aRayMask,
                    *(aInstance.mStructure),
                    forwardResult);

                //
                forwardOffsetC = t_IPacketC::replicate(forwardResult.getOffset(aInstance));
                tIntersector::template finalizeResult<IsectData::GEOM_NORMAL, taSize>(forwardRayPacket, forwardResult);

                //backward direction
                backwardRayPacket = newRayPacket;
                if(taCommonOrg)
                {
                    backwardRayPacket.org =
                      t_Vec3fC::replicate( backwardRayPacket.org(0) ) + tMaxOutPlane * backwardRayPacket.dir;
                }
                else
                {
                    backwardRayPacket.org =
                      backwardRayPacket.org + tMaxOutPlane*backwardRayPacket.dir;
                }
                backwardRayPacket.dir = -backwardRayPacket.dir;
                backwardRayPacket.invDir = -backwardRayPacket.invDir;
                backwardRayPacket.tMin = Packet<taSize, float>::replicate(0);
                backwardResult.dist = t_Packet::C_INFINITY;
                backwardRayPacket.tMax = t_Packet::C_INFINITY;
                backwardRayPacket.template init<false>(
                    aRayMask, aInstance.mStructure->getBounds());

                tIntersector::template intersect<taIsectData, false>(
                    backwardRayPacket,
                    aRayMask,
                    *(aInstance.mStructure),
                    backwardResult);

                //
                backwardOffsetC = t_IPacketC::replicate(backwardResult.getOffset(aInstance));
                tIntersector::template finalizeResult<IsectData::GEOM_NORMAL, taSize>(backwardRayPacket, backwardResult);
            }

            // set ray limits
            oResult.dist = tMaxCut;
            newRayPacket.tMin = tMinCut;
        }

        //
        newRayPacket.template init<taCommonOrg>(
            aRayMask, aInstance.mStructure->getBounds());

        tIntersector::template intersect<taIsectData, taCommonOrg>(
            newRayPacket,
            aRayMask,
            *(aInstance.mStructure),
            oResult);

        //
        t_IPacketC offsetC = t_IPacketC::replicate(oResult.getOffset(aInstance));
        if(aInstance.mCutPlaneNorms.size()>0)
        {
            if(aInstance.mCutPlaneCap)
            {
              tIntersector::template finalizeResult<IsectData::GEOM_NORMAL, taSize>(aRayPacket, oResult);
            }
        }

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            oResult.dist(i) = oResult.mask(i).iif(
                Math::reciprocal(scaleLength(i)) * oResult.dist(i),
                oldResultDist(i));

            if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
            {
                oResult.instanceOffsets(i) = oResult.mask(i).iif(
                    offsetC,
                    oResult.instanceOffsets(i));
            }

            if(aInstance.mCutPlaneNorms.size()>0)
            {
                //if caps
                if(aInstance.mCutPlaneCap)
                {
                    t_PacketC bz = backwardResult.mask(i).iif(Math::dot(backwardResult.geomNormal(i), backwardRayPacket.dir(i)), bz);
                    t_PacketC fz = forwardResult.mask(i).iif(Math::dot(forwardResult.geomNormal(i), aRayPacket.dir(i)), fz);
                    t_PacketC z = oResult.mask(i).iif(Math::dot(oResult.geomNormal(i), aRayPacket.dir(i)), z);

                    RTFACT_PACKET_FOR_ALL(j, t_Vec3fC::SIZE)
                    {
                        int k = -1;
                        // ray hits the instance
                        if(oResult.mask(i)[j])
                        {
                            if((0 < tMaxOutPlane(i)[j])
                               && (tMaxOutPlane(i)[j] < oResult.dist(i)[j]) // plane in front of hitpoint (ray hit the cap)
                              )
                            {
                                if(z[j] >= 0)
                                {
                                    k = kMaxOutCut(i)[j];
                                    oResult.dist(i)[j] = tMaxOutPlane(i)[j];
                                }
                            }

                            if(oResult.cap(i)[j] && oResult.dist(i)[j] < oldResultDist(i)[j])
                            {
                                oResult.cap(i)[j] = 0;
                            }
                        }
                        else if(tMinInPlane(i)[j] > 0) //ray miss the instance (it is cut) but might hit the cap
                        {
                            // shadow ray hit the cap
                            if(aRayPacket.rayType == RayType::ShadowRay && aInstance.mDropShadow)
                            {
                                //A (ray hit the object)

                                if( forwardResult.mask(i)[j]
                                    && (forwardResult.dist(i)[j] < oResult.dist(i)[j])
                                    && 0 < tMinInPlane(i)[j]
                                    && (
                                        //B (ray from the inside hits the part of the object)
                                        (tMinInPlane(i)[j] < oResult.dist(i)[j]) ||
                                        //D (ray from the inside hits another object)
                                        (tMinInPlane(i)[j] < forwardResult.dist(i)[j])
                                      )
                                    )
                                {
                                    if(fz[j] > 0)
                                    {
                                        k = kMinInCut(i)[j];
                                        oResult.dist(i)[j] = tMinInPlane(i)[j];

                                        oResult.instanceOffsets(i)[j] = forwardOffsetC[j];
                                        oResult.primitiveOffset(i)[j] = forwardResult.primitiveOffset(i)[j];
                                        oResult.appearanceID(i)[j] = forwardResult.appearanceID(i)[j];
                                    }
                                }

                                //C (ray from outside hits the part of the object inside)
                                if( backwardResult.mask(i)[j]
                                    && (tMaxOutPlane(i)[j] < oResult.dist(i)[j])
                                    && (backwardResult.dist(i)[j] < tMaxOutPlane(i)[j])
                                    && (kMaxOutCut(i)[j] >= 0)
                                    )
                                {
                                    if(bz[j] > 0)
                                    {
                                        k = kMaxOutCut(i)[j];
                                        oResult.dist(i)[j] = tMaxOutPlane(i)[j];

                                        oResult.instanceOffsets(i)[j] = backwardOffsetC[j];
                                        oResult.primitiveOffset(i)[j] = backwardResult.primitiveOffset(i)[j];
                                        oResult.appearanceID(i)[j] = backwardResult.appearanceID(i)[j];
                                    }
                                }
                            }
                            // primary ray hit the cap
                            else if(aRayPacket.rayType == RayType::ShadingRay)
                            {
                                if( (tMaxOutPlane(i)[j] < tMinInPlane(i)[j])  // non-empty interval condition
                                    && (0 < tMaxOutPlane(i)[j])               // ahead
                                    //&& (kMinInCut(i)[j] >= 0)                 // closed interval
                                    && backwardResult.mask(i)[j]              // backward ray hit the instance
                                    && (tMaxOutPlane(i)[j] < oResult.dist(i)[j])
                                    )
                                {
                                    // backward ray hit interior
                                    if(bz[j] > 0)
                                    {
                                        k = kMaxOutCut(i)[j];
                                        oResult.dist(i)[j] = tMaxOutPlane(i)[j];

                                        oResult.instanceOffsets(i)[j] = backwardOffsetC[j];
                                        oResult.primitiveOffset(i)[j] = backwardResult.primitiveOffset(i)[j];
                                        oResult.appearanceID(i)[j] = backwardResult.appearanceID(i)[j];
                                    }
                                }
                                //
                                else if((kMinInCut(i)[j] >= 0)
                                    && (tMinInPlane(i)[j] > 0)
                                    && (kMaxOutCut(i)[j] < 0)
                                    && forwardResult.mask(i)[j]
                                    && (forwardResult.dist(i)[j] > tMinInPlane(i)[j])
                                    )
                                {
                                    //forward ray hit outside surface and is in front of inward cutplane
                                    if(fz[j] > 0)
                                    {
                                        k = kMinInCut(i)[j];
                                        oResult.dist(i)[j] = tMinInPlane(i)[j];

                                        oResult.instanceOffsets(i)[j] = forwardOffsetC[j];
                                        oResult.primitiveOffset(i)[j] = forwardResult.primitiveOffset(i)[j];
                                        oResult.appearanceID(i)[j] = forwardResult.appearanceID(i)[j];
                                    }
                                }
                            }
                        }

                        if(k >= 0)
                        {
                            oResult.mask(i)[j] = 0xFFFFFFFF;
                            oResult.cap(i)[j] = 0xFFFFFFFF;

                            oResult.cutNormal(i).x[j] = aInstance.mCutPlaneNorms[k].x;
                            oResult.cutNormal(i).y[j] = aInstance.mCutPlaneNorms[k].y;
                            oResult.cutNormal(i).z[j] = aInstance.mCutPlaneNorms[k].z;
                            oResult.capColor(i).x[j] = aInstance.mCapColor.x;
                            oResult.capColor(i).y[j] = aInstance.mCapColor.y;
                            oResult.capColor(i).z[j] = aInstance.mCapColor.z;
                        }
                    }
                }
            }
            else
            {
                oResult.cap(i) = oResult.cap(i) & (oResult.dist(i) >= oldResultDist(i));
            }

            oResult.mask(i) |= oldResultMask(i);
        }

    }

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_FORCE_INLINE void finalizeResult(
        const RayPacket<taSize>& aRayPacket,
        Result<taSize>& oResult)
    {
        typedef typename Packet<taSize, int>::Container t_IPacketC;
        typedef ActiveMask<taSize> t_ActiveMask;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        tIntersector::template finalizeResult<taIsectData, taSize>(aRayPacket, oResult);

        Array<taSize, int> uniqueInstanceOffsets;
        typename Array<taSize, int>::iterator uniqueInstanceOffsetEnd =
            oResult.instanceOffsets.getUniqueValues(
                uniqueInstanceOffsets.begin(), uniqueInstanceOffsets.end(),
                oResult.mask);

        for(typename Array<taSize, int>::iterator instanceOffset =
                uniqueInstanceOffsets.begin();
            instanceOffset != uniqueInstanceOffsetEnd; ++instanceOffset)
        {
            t_IPacketC instanceOffsetC = t_IPacketC::replicate(*instanceOffset);
            t_ActiveMask instanceMask;
            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
            {
                instanceMask(i) = oResult.mask(i) &
                    (oResult.instanceOffsets(i) == instanceOffsetC);
            }
            instanceMask.updateActiveContainers();
            const tInstance& instance = *(oResult.instanceBase + *instanceOffset);

            //
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, instanceMask)
            {
                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                {
                    //
                    oResult.normal(i) = oResult.cap(i).iif(oResult.cutNormal(i), oResult.normal(i));
                    oResult.normal(i) = instanceMask(i).iif(
                        instance.mXfm.applyToNormalAffine(oResult.normal(i)),
                        oResult.normal(i));
                    oResult.normal(i).setNormalized();

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                    {
                        oResult.geomNormal(i) = instanceMask(i).iif(
                            instance.mXfm.applyToNormalAffine(oResult.geomNormal(i)),
                            oResult.geomNormal(i));

                        oResult.geomNormal(i).setNormalized();
                    }
                }
                // TODO Make appearanceID swapping more efficient?
                for(int j=0; j< oResult.appearanceID(i).SIZE; j++){
                    if(instanceMask(i)[j])
                        oResult.appearanceID(i)[j] = static_cast<int>(instance.mAppearanceMap[oResult.appearanceID(i)[j]]);
                }
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__INSTANCEINTERSECTOR_HPP
