/*
 *  InstanceIntersector.hpp
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
 *  Created on: 2008-07-07 15:21:54 +0200
 *  Author(s): Iliyan Georgiev, Felix Klein, Dmitri Rubinstein, Petr Kmoch
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

        ptrdiff_t getOffset(
            tInstance& aInstance) const
        {
            return &aInstance - instanceBase;
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

        RayPacket<taSize> newRayPacket = aRayPacket;

        if(taCommonOrg)
        {
            newRayPacket.org(0) =
               aInstance.mXfm.applyInverseToPointAffine(aRayPacket.org(0));
			   //aInstance.trans.applyInverseToPointAffine(aRayPacket.org(0));
        }

        t_Packet oldResultDist, scaleLength, scaleLengthRecip;
        t_Mask oldResultMask;

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            if(!taCommonOrg)
            {
                newRayPacket.org(i) =
                    aInstance.mXfm.applyInverseToPointAffine(aRayPacket.org(i));
					//aInstance.trans.applyInverseToPointAffine(aRayPacket.org(i));
            }

            newRayPacket.dir(i) =
                aInstance.mXfm.applyInverseToVectorAffine(aRayPacket.dir(i));
				//aInstance.trans.applyInverseToVectorAffine(aRayPacket.dir(i));

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
        }

        newRayPacket.template init<taCommonOrg>(
            aRayMask, aInstance.mStructure->getBounds());
		//    aRayMask, aInstance.structure->getBounds());

        tIntersector::template intersect<taIsectData, taCommonOrg>(
            newRayPacket,
            aRayMask,
            *(aInstance.mStructure),
			//*(aInstance.structure),
            oResult);

        t_IPacketC offsetC = t_IPacketC::replicate(RTFACT_DOWNSIZE_PACKET_CAST(int,oResult.getOffset(aInstance)));

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

        tIntersector::template finalizeResult<taIsectData, taSize>(
            aRayPacket, oResult);

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
        {
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

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, instanceMask)
                {
                    oResult.normal(i) = instanceMask(i).iif(
                        instance.mXfm.applyToNormalAffine(oResult.normal(i)),
//						instance.trans.applyToNormalAffine(oResult.normal(i)),
                        oResult.normal(i));

                    oResult.normal(i).setNormalized();

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                    {
                        oResult.geomNormal(i) = instanceMask(i).iif(
                            instance.mXfm.applyToNormalAffine(oResult.geomNormal(i)),
//							instance.trans.applyToNormalAffine(oResult.geomNormal(i)),
                            oResult.geomNormal(i));

                        oResult.geomNormal(i).setNormalized();
                    }
                }
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__INSTANCEINTERSECTOR_HPP
