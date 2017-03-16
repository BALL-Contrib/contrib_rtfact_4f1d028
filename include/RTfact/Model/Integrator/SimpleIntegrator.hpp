/* 
 *  SimpleIntegrator.hpp
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
 
 #ifndef RTFACT__SIMPLEINTEGRATOR_HPP
#define RTFACT__SIMPLEINTEGRATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Integrator.hpp>

RTFACT_NAMESPACE_BEGIN

class SimpleIntegrator :
    public Integrator
{
    enum
    {
        INTERSECTION_DATA =
            IsectData::DISTANCE |
            IsectData::GEOM_NORMAL /*|
            IsectData::VERTEX_COLOR*/
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
        FBData<taSize>& oResult) const
    {
        typedef Vec3f<taSize>                             t_Vec3f;
        typedef typename t_Vec3f::Container               t_Vec3fC;
        typedef typename Packet<taSize, float>::Container t_PacketC;

        typename tIntersector::template Result<taSize> intersection;

        //START_TIMER(0);

        Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            aIntersector, aRayPacket, aRayMask, aPrimitive, intersection);           

        //STOP_TIMER(0);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            oResult.color(i) = intersection.mask(i).iif(
                /*intersection.vertexColor(i)*/ t_Vec3fC::C_1_1_1 *
                    Math::abs(Math::dot(intersection.normal(i), aRayPacket.dir(i))),
                t_Vec3fC::C_0_0_0());
        }

        if(RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::DISTANCE))
        {
            oResult.distance = intersection.dist;
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SIMPLEINTEGRATOR_HPP
