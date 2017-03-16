/*
 *  SurfaceShaderIntegrator.hpp
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
 *  Created on: 2008-08-07 18:24:48 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Lukas Marsalek, Felix Klein
 */

 #ifndef RTFACT__SURFACESHADERINTEGRATOR_HPP
#define RTFACT__SURFACESHADERINTEGRATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Integrator.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext>
class SurfaceShaderIntegrator :
    public Integrator
{
    enum
    {
        INTERSECTION_DATA =
        IsectData::DISTANCE |
        IsectData::NORMAL |
        IsectData::APPEARANCE_ID |
        IsectData::TEX_COORDS |
        IsectData::VERTEX_COLOR |
        IsectData::DERIVATIVES |
        IsectData::TANGENT_SPACE
    };

public:

    tContext mContext;

    SurfaceShaderIntegrator(
        const tContext& aContext
    ) :
        mContext(aContext)
    {}

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTEGRATOR_INLINE void evaluate(
        const PixelSampler::Sample<taSize>&,
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const Scene&,
        const Primitive&,
        const Intersector&,
        FBData<taSize>& oResult)
    {
        oResult.color = Vec3f<taSize>::C_0_0_0();

        typename tContext::Intersector::template Result<taSize> intersection;

        RTfact::Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            mContext.intersector, aRayPacket, aRayMask,
            mContext.primitive, intersection);

        if(RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::DISTANCE))
        {
            oResult.distance = intersection.dist;
        }

        if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::NORMAL))
        {
            oResult.normal = intersection.geomNormal;
        }

        if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::ALPHA))
        {
            mContext.template shade<taCommonOrg, true>(
                aRayPacket, aRayMask, intersection, oResult.color, &oResult.alpha);
        }
        else
        {
            mContext.template shade<taCommonOrg, false>(
                aRayPacket, aRayMask, intersection, oResult.color);
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SURFACESHADERINTEGRATOR_HPP
