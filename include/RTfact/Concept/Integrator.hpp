/* 
 *  Integrator.hpp
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
 
 #ifndef RTFACT__INTEGRATOR_HPP
#define RTFACT__INTEGRATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/PixelSampler.hpp>
#include <RTfact/Concept/Scene.hpp>
#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Concept/Camera.hpp>
#include <RTfact/Utils/RayPacket.hpp>
#include <RTfact/Utils/FramebufferUtils.hpp>

RTFACT_NAMESPACE_BEGIN

class Integrator
{
public:

    void init(
        Scene&,
        Primitive&,
        Intersector&,
        const Camera&)
    {}

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTEGRATOR_INLINE void evaluate(
        const PixelSampler::Sample<taSize>& aSample,
        const RayPacket<taSize>& aRayPacket,
        Scene& aScene,
        Primitive& aPrimitive,
        Intersector& aIntersector,
        FBData<taSize>& oResult);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__INTEGRATOR_HPP
