/* 
 *  BoundingSphere.hpp
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
 *  Created on: 2010-12-12 16:00:25 +0100
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */

#ifndef RTFACT__BSPHERE_HPP
#define RTFACT__BSPHERE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>

#define RTFACT__BSPHERE_INLINE RTFACT_FORCE_INLINE

RTFACT_NAMESPACE_BEGIN

struct BoundingSphere
{
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    Vec3f1 center;
    float radius;

    RTFACT__BSPHERE_INLINE BoundingSphere()
    {}

    RTFACT__BSPHERE_INLINE BoundingSphere(
        const Vec3f1& aCenter,
        const float aRadius
    ) :
        center(aCenter),
        radius(aRadius)
    {}

    RTFACT__BSPHERE_INLINE float getArea() const
    {
        return RTFACT_4_PI * radius * radius;
    }

    RTFACT__BSPHERE_INLINE float getVolume() const
    {
        return (RTFACT_4_PI / 3.f) * radius * radius * radius;
    }

    static RTFACT__BSPHERE_INLINE const BoundingSphere empty()
    {
        return BoundingSphere(Vec3f1::C_0_0_0(), 0.f);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__BSPHERE_HPP
