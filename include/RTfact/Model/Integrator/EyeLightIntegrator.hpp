/* 
 *  EyeLightIntegrator.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__EYELIGHTINTEGRATOR_HPP
#define RTFACT__EYELIGHTINTEGRATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Array.hpp>

#include <RTfact/Concept/Integrator.hpp>
#include <RTfact/Model/EnvironmentTexture/Float3ConstantEnvironmentTexture.hpp>

RTFACT_NAMESPACE_BEGIN

class EyeLightIntegrator :
    public Integrator
{
    enum
    {
        INTERSECTION_DATA =
            IsectData::DISTANCE |
            IsectData::NORMAL |
            IsectData::TEX_COORDS |
            IsectData::APPEARANCE_ID
    };

public:

    template<
        bool taCommonOrg,
        uint taSize,
        class tShadingInfoScene,
        class tPrimitive,
        class tIntersector>
    RTFACT_INTEGRATOR_INLINE void evaluate(
        const PixelSampler::Sample<taSize>&,
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const tShadingInfoScene& aScene,
        tPrimitive& aPrimitive,
        tIntersector& aIntersector,
        FBData<taSize>& oResult)
    {
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef Packet<taSize, float>         t_FPacket;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef Mask<taSize>                  t_Mask;
        typedef ActiveMask<taSize>            t_ActiveMask;
        typedef BitMask<taSize>               t_BitMask;

        RTFACT_INTEGRATOR_ASSERT(
            RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::COLOR));

        typename tIntersector::template Result<taSize> intersection;

        //START_TIMER(0);

        Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            aIntersector, aRayPacket, aRayMask, aPrimitive, intersection);

        //STOP_TIMER(0);

        t_Vec3f rayOppDir;

        //START_TIMER(1);


        // invert the normals
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, intersection.mask)
        {
            rayOppDir(i) = -aRayPacket.dir(i);

            intersection.normal(i) = 
                (Math::dot(intersection.normal(i), rayOppDir(i)) >
                    t_FPacketC::C_0()).iif(
                        intersection.normal(i),
                        -intersection.normal(i));
        }

        //intersection.normal = -intersection.normal;

        //START_TIMER(0);

        t_Vec3f reflection;

        Array<taSize, int> uniqueMats;
        typename Array<taSize, int>::iterator uniqueMatEnd =
            intersection.appearanceID.getUniqueValues(
                uniqueMats.begin(), uniqueMats.end(), intersection.mask);

        for(typename Array<taSize, int>::iterator materialId = uniqueMats.begin();
            materialId != uniqueMatEnd; ++materialId)
        {
            t_IPacketC materialIdC = t_IPacketC::replicate(*materialId);

            t_ActiveMask materialMask;

            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
            {
                materialMask(i) =
                    intersection.mask(i) & (intersection.appearanceID(i) == materialIdC);
            }

            materialMask.updateActiveContainers();

            //START_TIMER(0);

            aScene.appearances[*materialId]->evaluate<taSize>(
                rayOppDir, rayOppDir, intersection, materialMask, reflection);

            //STOP_TIMER(0);
        }

        //STOP_TIMER(0);

        //STOP_TIMER(1);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, intersection.mask)
        {
            oResult.color(i) = intersection.mask(i).iif(
                reflection(i) * Math::dot(intersection.normal(i), rayOppDir(i)),
                oResult.color(i));
        }

        intersection.mask = (~intersection.mask) & aRayMask;
        intersection.mask.updateActiveContainers();

        aScene.environmentTexture.template sample<false, taCommonOrg>(
            aRayPacket, intersection.mask, oResult.color);

        if(RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::DISTANCE))
        {
            oResult.distance = intersection.dist;
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__EYELIGHTINTEGRATOR_HPP
