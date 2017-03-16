/* 
 *  QuadraticLightAttenuation.hpp
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
 *  Created on: 2008-12-17 04:07:09 +0100
 *  Author(s): Iliyan Georgiev
 */
 
#ifndef RTFACT__QUADRATICLIGHTATTENUATION_HPP
#define RTFACT__QUADRATICLIGHTATTENUATION_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/LightAttenuation.hpp>

RTFACT_NAMESPACE_BEGIN

class QuadraticLightAttenuation :
    public LightAttenuation
{
    float   mMinimalDistance;

public:

    RTFACT_DEFINE_PROPERTY(float, MinimalDistance);

    QuadraticLightAttenuation() :
      mMinimalDistance(0.f)
    {
        //
    }       

    template<
        uint taSize>
    RTFACT_FORCE_INLINE const Packet<taSize, float> getFactor(
        const Packet<taSize, float>& aDistance) const
    {
        typedef Packet<taSize, float>   tFPacket;

        const tFPacket minDistC = tFPacket::replicate(mMinimalDistance);
        const tFPacket distSquared = aDistance * aDistance;
        const tFPacket distScale = Math::smoothStep(distSquared, minDistC, 1.4f * minDistC);        
        return Math::reciprocal(distSquared) * distScale;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__QUADRATICLIGHTATTENUATION_HPP
