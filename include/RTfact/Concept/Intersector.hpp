/* 
 *  Intersector.hpp
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
 
 #ifndef RTFACT__INTERSECTOR_HPP
#define RTFACT__INTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Concept/Scene.hpp>
#include <RTfact/Utils/RayPacket.hpp>

RTFACT_NAMESPACE_BEGIN

class Intersector
{
public:

    template<
        uint taSize>
    class Result;

    void init(
        Scene&)
    {}

    template<
        umask taIsectData,
        uint taSize>
    void initResult(
        const ActiveMask<taSize>& aMask,
        Result<taSize>& oResult)
    {}

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize>
    void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        Primitive& aPrimitive,
        Result<taSize>& oResult);

    template<
        umask taIsectData,
        uint taSize>
    void finalizeResult(
        const RayPacket<taSize>& aRayPacket,
        Result<taSize>& oResult)
    {}

    // convenience method
    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize,
        class tPrimitive,
        class tIntersector,
        class tResult>
    static RTFACT_FORCE_INLINE void intersect(
        tIntersector& aIntersector,
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        tPrimitive& aPrimitive,
        tResult& oResult)
    {
        aRayPacket.template init<taCommonOrg>(
            aRayMask,
            aPrimitive.getBounds());

        aIntersector.template initResult<taIsectData>(
            aRayPacket, aRayMask, oResult);

        //START_TIMER(2);

        aIntersector.template intersect<taIsectData, taCommonOrg>(
            aRayPacket, aRayMask, aPrimitive, oResult);

        //STOP_TIMER(2);

        oResult.mask.updateActiveContainers();

        aIntersector.template finalizeResult<taIsectData, taSize>(
            aRayPacket, oResult);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__INTERSECTOR_HPP
