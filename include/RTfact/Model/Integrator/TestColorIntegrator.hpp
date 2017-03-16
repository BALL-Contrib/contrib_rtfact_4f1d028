/*
 *  TestColorIntegrator.hpp
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
 *  Created on: 2008-10-03 11:54:00 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev
 */

 #ifndef RTFACT_TESTCOLORINTEGRATOR_HPP_INCLUDED
#define RTFACT_TESTCOLORINTEGRATOR_HPP_INCLUDED

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/Integrator/SimpleIntegrator.hpp>

RTFACT_NAMESPACE_BEGIN

class TestColorIntegrator :
    public SimpleIntegrator
{
    enum
    {
        INTERSECTION_DATA =
            IsectData::DISTANCE |
            IsectData::GEOM_NORMAL
    };

public:

    template<
        bool taCommonOrg,
        uint taSize,
        class tShadingInfoScene,
        class tPrimitive,
        class tIntersector>
    RTFACT_INTEGRATOR_INLINE void evaluate(
        const PixelSampler::Sample<taSize>& sample,
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const tShadingInfoScene& aScene,
        tPrimitive& aPrimitive,
        tIntersector& aIntersector,
        Vec3f<taSize>& oResult) const
    {
        typedef Vec3f<taSize>                             t_Vec3f;
        typedef typename t_Vec3f::Container               t_Vec3fC;
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typename tIntersector::template Result<taSize> intersection;

        //START_TIMER(2);

        Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            aIntersector, aRayPacket, aRayMask, aPrimitive, intersection);

        //STOP_TIMER(2);
        t_Vec3fC min_color = t_Vec3fC::replicate(Vec3f<1>(0,1,0));
        t_Vec3fC max_color = t_Vec3fC::replicate(Vec3f<1>(1,0,0));
        t_PacketC t_values;
        t_PacketC max_value = t_PacketC::replicate(2);
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            t_values = intersection.testData(i) / max_value;
            t_values = (t_values > t_PacketC::C_2 ).iif( t_PacketC::C_2, t_values);

            intersection.normal(i) =
                (Math::dot(intersection.normal(i), aRayPacket.dir(i)) >
                   t_PacketC::C_0()).iif( intersection.normal(i),
                                         -intersection.normal(i));


            oResult(i) = intersection.mask(i).iif(
                t_Vec3fC::C_1_1_1* Math::abs(Math::dot(intersection.normal(i),
                                              aRayPacket.dir(i))),
                t_Vec3fC::replicate(Vec3f<1>(0.4,0.4,0.4)));
            oResult(i)*=( min_color * Math::min(t_PacketC::C_2 - t_values, t_PacketC::C_1)
                + max_color * Math::min(t_values, t_PacketC::C_1));

        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT_SIMPLEINTEGRATOR_HPP_INCLUDED
