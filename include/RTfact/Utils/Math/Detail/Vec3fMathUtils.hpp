/* 
 *  Vec3fMathUtils.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__VEC3FMATHUTILS_HPP
#define RTFACT__VEC3FMATHUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Math {

template<
    uint taSize1,
    uint taSize2>
RTFACT_MATH_INLINE const Vec3f<PacketOpResultSize<taSize1, taSize2>::value> cross(
    const Vec3f<taSize1>& aVec1,
    const Vec3f<taSize2>& aVec2)
{
    return Vec3f<taSize1>::cross(aVec1, aVec2);
}

template<
    uint taSize1,
    uint taSize2>
RTFACT_MATH_INLINE const Vec3f<PacketOpResultSize<taSize1, taSize2>::value> product(
    const Vec3f<taSize1>& aVec1,
    const Vec3f<taSize2>& aVec2)
{
    return Vec3f<taSize1>::product(aVec1, aVec2);
}

template<
    uint taSize1,
    uint taSize2>
RTFACT_MATH_INLINE const Packet<PacketOpResultSize<taSize1, taSize2>::value, float>
dot(
    const Vec3f<taSize1>& aVec1,
    const Vec3f<taSize2>& aVec2)
{
    return Vec3f<taSize1>::dot(aVec1, aVec2);
}

template<
    uint taSize>
RTFACT_MATH_INLINE void getFrame(
    const Vec3f<taSize>& aVec1,
    Vec3f<taSize>& oVec2,
    Vec3f<taSize>& oVec3)
{
    RTFACT_PROGRAM_ERROR("A generic Math::getFrame is not implemented");
}

template<>
RTFACT_MATH_INLINE void getFrame<1>(
    const Vec3f<1>& aVec1,
    Vec3f<1>& oVec2,
    Vec3f<1>& oVec3)
{
    if(Math::abs(aVec1.xf) > Math::abs(aVec1.yf))
    {
        const float invLen = Math::reciprocal(aVec1.xf * aVec1.xf +
                                              aVec1.zf * aVec1.zf);

        oVec2.xf = -aVec1.zf * invLen;
        oVec2.yf = 0.f;
        oVec2.zf = aVec1.xf * invLen;
    }
    else
    {
        const float invLen = Math::reciprocal(aVec1.y * aVec1.y +
                                              aVec1.z * aVec1.z);

        oVec2.xf = 0.f;
        oVec2.yf = aVec1.zf * invLen;
        oVec2.zf = -aVec1.yf * invLen;
    }

    oVec3 = Math::cross(aVec1, oVec2);
}

#pragma region zeroClampDot

RTFACT_MATH_INLINE float zeroClampDot(
    const RTfact::Vec3f1& aVec1,
    const RTfact::Vec3f1& aVec2)
{
    return Math::max(0.f, RTfact::Vec3f1::ContainerData::dot3(aVec1.data, aVec2.data));
}

RTFACT_MATH_INLINE const Packet<4, float> zeroClampDot(
    const Vec3f<4>& aVec1,
    const Vec3f<4>& aVec2)
{
    return Math::max(Packet<4, float>::C_0(), aVec1.x * aVec2.x + aVec1.y * aVec2.y + aVec1.z * aVec2.z);
}

RTFACT_MATH_INLINE const Packet<4, float> zeroClampDot(
    const Vec3f1& aVec1,
    const Vec3f<4>& aVec2)
{
    return zeroClampDot(Vec3f<4>::replicate(aVec1), aVec2);
}

RTFACT_MATH_INLINE const Packet<4, float> zeroClampDot(
    const Vec3f<4>& aVec1,
    const Vec3f1& aVec2)
{
    return zeroClampDot(aVec1, Vec3f<4>::replicate(aVec2));
}

#pragma endregion // zeroClampDot

#pragma region absDot

RTFACT_MATH_INLINE float absDot(
    const Vec3f1& aVec1,
    const Vec3f1& aVec2)
{
    return Math::abs(Vec3f1::ContainerData::dot3(aVec1.data, aVec2.data));
}

RTFACT_MATH_INLINE const Packet<4, float> absDot(
    const Vec3f<4>& aVec1,
    const Vec3f<4>& aVec2)
{
    return Math::abs(aVec1.x * aVec2.x + aVec1.y * aVec2.y + aVec1.z * aVec2.z);
}

RTFACT_MATH_INLINE const Packet<4, float> absDot(
    const Vec3f1& aVec1,
    const Vec3f<4>& aVec2)
{
    return absDot(Vec3f<4>::replicate(aVec1), aVec2);
}

RTFACT_MATH_INLINE const Packet<4, float> absDot(
    const Vec3f<4>& aVec1,
    const Vec3f1& aVec2)
{
    return absDot(aVec1, Vec3f<4>::replicate(aVec2));
}

#pragma endregion // absDot

} // namespace Math
RTFACT_NAMESPACE_END

#endif // RTFACT__VEC3FMATHUTILS_HPP
