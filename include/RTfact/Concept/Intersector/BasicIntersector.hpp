/* 
 *  BasicIntersector.hpp
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
 *  Created on: 2008-06-27 14:47:39 +0200
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__BASICINTERSECTOR_HPP
#define RTFACT__BASICINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Utils/SurfaceIntersection.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tPrimitive>
class BasicIntersector :
    public Intersector
{
public:

    template<
        uint taSize>
    class Result :
        public SurfaceIntersection<taSize>
    {
        typedef SurfaceIntersection<taSize> t_Base;

    protected:

        tPrimitive* primitiveBase;

    public:

        typedef tPrimitive Primitive;

        using t_Base::SIZE;
        using t_Base::SIZE_SQRT;
        using t_Base::mask;
        using t_Base::dist;
        using t_Base::appearanceID;
        using t_Base::geomNormal;
        using t_Base::u;
        using t_Base::v;
        using t_Base::normal;
        using t_Base::texCoord1;
        using t_Base::texCoord2;
        using t_Base::vertexColor;

        Packet<taSize, int> primitiveOffset;

        void initBase(
            tPrimitive& aPrimitiveBase)
        {
            primitiveBase = &aPrimitiveBase;
        }

        ptrdiff_t getOffset(
            const tPrimitive& aPrimitive) const
        {
            return &aPrimitive - primitiveBase;
        }

        tPrimitive& getPrimitive(
            const int aOffset) const
        {
            return *(primitiveBase + aOffset);
        }
    };

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_FORCE_INLINE void initResult(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aMask,
        Result<taSize>& oResult)
    {
        oResult.mask = Mask<taSize>::C_FALSE();
        oResult.cap = Mask<taSize>::C_FALSE();
        oResult.dist = aRayPacket.tMax;
    }

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        tPrimitive& aPrimitive,
        Result<taSize>& oResult);

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_FORCE_INLINE void finalizeResult(
        const RayPacket<taSize>&,
        Result<taSize>&)
    {}
};

RTFACT_NAMESPACE_END

#endif // RTFACT__BASICINTERSECTOR_HPP
