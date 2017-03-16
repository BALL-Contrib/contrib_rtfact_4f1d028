/*
 *  NormalShader.hpp
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
 *  Created on: 2008-11-21 17:03:46 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */

#ifndef RTFACT__NORMALSHADER_HPP
#define RTFACT__NORMALSHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext>
class NormalShader :
    public SurfaceShader<tContext>
{
public:

    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        SurfaceIntersection<taSize>& aIntersection,
        tContext& aContext,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL) const
    {
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);

        typedef typename Vec3f<taSize>::Container t_Vec3fC;
        typedef typename Packet<taSize, float>::Container t_FPacketC;

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            //oResult(i) = aRayMask(i).iif(
            //    t_Vec3fC::C_1_1_1 * (Math::dot(-aIntersection.normal(i),
            //                                    aRayPacket.dir(i)) * 0.5f + 0.5f),
			//    oResult(i));
			oResult(i) = aRayMask(i).iif(
				(aIntersection.normal(i) * 0.5f) + (t_Vec3fC::C_1_1_1 * 0.5f),
				oResult(i));


            if (
                taStoreAlpha)
            {
                (*oResultAlpha)(i) = aRayMask(i).iif(t_FPacketC::C_1, (*oResultAlpha)(i));
            }

            /*oResult(i) = aRayMask(i).iif(
                t_Vec3fC(aIntersection.dist(i), aIntersection.dist(i),aIntersection.dist(i)),
                oResult(i));*/
        }
    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<1>& aRayPacket,
        const ActiveMask<1>& aRayMask,
        SurfaceIntersection<1>& aIntersection,
        tContext& aContext,
        Vec3f<1>& oResult,
        Packet<1, float>* oResultAlpha = RTFACT_NULL) const
    {}

        template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void attenuateLight(
        const ActiveMask<taSize>& aMask,
        SurfaceIntersection<taSize>& aIntersection,
        Mask<taSize>& oMask,
        Vec3f<taSize>& oIntensity) const
    {
        typedef typename Mask<taSize>::Container t_MaskC;

        const t_MaskC falseC = t_MaskC::C_FALSE();

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oMask(i) &= ~aMask(i);
        }
    }

private:

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__NORMALSHADER_HPP
