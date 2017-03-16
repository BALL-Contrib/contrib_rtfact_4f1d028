/* 
 *  Vec3f.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev
 */
 
#ifndef RTFACT__VEC3F_HPP
#define RTFACT__VEC3F_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/CompileTimeMathUtils.hpp>
#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Detail/Constants.hpp>

#pragma region Assertion macro definitions

#define RTFACT_VEC3F_ASSERT(x)        RTFACT_PACKET_ASSERT(x)
#define RTFACT_VEC3F_STATIC_ASSERT(x) RTFACT_PACKET_STATIC_ASSERT(x)

#pragma endregion // Assertion macro definitions

#pragma region Constants utility macros

#define RTFACT__VEC3F_SPECS (1)(4)

#define RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(aName)                      \
    static const t_This aName;                                                 \
    static const t_This getConstant_##aName(float aX, float aY, float aZ)      \
    {                                                                          \
        return replicate(Vec3f<1>(aX, aY, aZ));                                \
    }

#define RTFACT_VEC3F_INIT_CONST(aSize, aName, aX, aY, aZ)                      \
    RTFACT_PACKET_PUT_TEMPLATE(aSize, RTFACT__VEC3F_SPECS)                     \
    const Vec3f<aSize> Vec3f<aSize>::aName                                     \
        = Vec3f<aSize>::getConstant_##aName(aX, aY, aZ); 
    

#pragma endregion // Constants utility macros

#include <RTfact/Utils/Packets/Detail/Vec3f1.hpp>
#include <RTfact/Utils/Packets/Detail/Vec3f4.hpp>
#include <RTfact/Utils/Packets/Detail/Vec3fGeneric.hpp>

#include <RTfact/Utils/Math/Detail/Vec3fMathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region Packet ternary operator definitions

template<
    uint taSize>
RTFACT_PACKET_INLINE const Vec3f<taSize> Mask<taSize>::iif(
    const Vec3f<taSize>& aVec1,
    const Vec3f<taSize>& aVec2) const
{
    Vec3f<taSize> ret;

    for(uint i = 0; i < CONTAINER_COUNT; ++i)
    {
        ret(i) = containers[i].iif(aVec1(i), aVec2(i));
    }

    return ret;
}

RTFACT_PACKET_INLINE const Vec3f<4> Mask<4>::iif(
    const Vec3f<4>& aVec1,
    const Vec3f<4>& aVec2) const
{
    return Vec3f<4>(iif(aVec1.x, aVec2.x),
                    iif(aVec1.y, aVec2.y),
                    iif(aVec1.z, aVec2.z));
}

RTFACT_PACKET_INLINE const Vec3f<1>& Mask<1>::iif(
    const Vec3f<1>& aVec1,
    const Vec3f<1>& aVec2) const
{
    return (data ? aVec1 : aVec2);
}

#pragma endregion

#pragma region Packet-Vec3f operator definitions

#define RTFACT__FLOAT_PACKET_ARITH_OP_VEC3F__DEFINE(aOperator)                 \
    template<>                                                                 \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Vec3f<taSize> Packet<1, float>::operator aOperator(\
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        const typename Vec3f<taSize>::Data tempData =                          \
            Vec3f<taSize>::Data::replicate(data);                              \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x.data = tempData aOperator aVec(i).x.data;                 \
            ret(i).y.data = tempData aOperator aVec(i).y.data;                 \
            ret(i).z.data = tempData aOperator aVec(i).z.data;                 \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    template<>                                                                 \
    RTFACT_PACKET_INLINE const Vec3f<4> Packet<1, float>::operator aOperator<4>(\
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        Vec3f<4> ret;                                                          \
        const Vec3f<4>::Data tempData = Vec3f<4>::Data::replicate(data);       \
        ret.x.data = tempData aOperator aVec.x.data;                           \
        ret.y.data = tempData aOperator aVec.y.data;                           \
        ret.z.data = tempData aOperator aVec.z.data;                           \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    template<>                                                                 \
    RTFACT_PACKET_INLINE const Vec3f<1> Packet<1, float>::operator aOperator<1>(\
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        return Vec3f<1>(Vec3f<1>::ContainerData::replicate(data) aOperator     \
                        aVec.data);                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Vec3f<taSize> Packet<4, float>::operator aOperator(\
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x.data = data aOperator aVec(i).x.data;                     \
            ret(i).y.data = data aOperator aVec(i).y.data;                     \
            ret(i).z.data = data aOperator aVec(i).z.data;                     \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
    template<>                                                                 \
    template<>                                                                 \
    RTFACT_PACKET_INLINE const Vec3f<4> Packet<4, float>::operator aOperator<4>(\
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        Vec3f<4> ret;                                                          \
        ret.x.data = data aOperator aVec.x.data;                               \
        ret.y.data = data aOperator aVec.y.data;                               \
        ret.z.data = data aOperator aVec.z.data;                               \
        return ret;                                                            \
    }                                                                          \
    template<>                                                                 \
    RTFACT_PACKET_INLINE const Vec3f<4> Packet<4, float>::operator aOperator(  \
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        Vec3f<4> ret;                                                          \
        ret.x.data = data aOperator Data::replicate(aVec.xf);                  \
        ret.y.data = data aOperator Data::replicate(aVec.yf);                  \
        ret.z.data = data aOperator Data::replicate(aVec.zf);                  \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<                                                                  \
        uint taSize,                                                           \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Vec3f<taSize>                                   \
    Packet<taSize, tScalar>::operator aOperator(                               \
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x.data = containers[i].data aOperator aVec(i).x.data;       \
            ret(i).y.data = containers[i].data aOperator aVec(i).y.data;       \
            ret(i).z.data = containers[i].data aOperator aVec(i).z.data;       \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<                                                                  \
        uint taSize,                                                           \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Vec3f<4>                                        \
    Packet<taSize, tScalar>::operator aOperator(                               \
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x.data = containers[i].data aOperator aVec.x.data;          \
            ret(i).y.data = containers[i].data aOperator aVec.y.data;          \
            ret(i).z.data = containers[i].data aOperator aVec.z.data;          \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<                                                                  \
        uint taSize,                                                           \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Vec3f<1>                                        \
    Packet<taSize, tScalar>::operator aOperator(                               \
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        const typename Vec3f<taSize>::Data tempDataX =                         \
            Vec3f<taSize>::Data::replicate(aVec.xf);                           \
        const typename Vec3f<taSize>::Data tempDataY =                         \
            Vec3f<taSize>::Data::replicate(aVec.yf);                           \
        const typename Vec3f<taSize>::Data tempDataZ =                         \
            Vec3f<taSize>::Data::replicate(aVec.zf);                           \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x.data = containers[i].data aOperator tempDataX;            \
            ret(i).y.data = containers[i].data aOperator tempDataY;            \
            ret(i).z.data = containers[i].data aOperator tempDataZ;            \
        }                                                                      \
        return ret;                                                            \
    }

RTFACT__FLOAT_PACKET_ARITH_OP_VEC3F__DEFINE(*)
RTFACT__FLOAT_PACKET_ARITH_OP_VEC3F__DEFINE(/)

#pragma endregion // Packet-Vec3f operator definitions

typedef Vec3f<4> Vec3fC;

RTFACT_NAMESPACE_END

#pragma region Undefine helper macros

#undef RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER
#undef RTFACT_VEC3F_DECLARE_CONSTS

#undef RTFACT_VEC3F_ASSERT
#undef RTFACT_VEC3F_STATIC_ASSERT

#undef RTFACT__FLOAT_PACKET_ARITH_OP_VEC3F__DEFINE

#pragma endregion // Undefine helper macros

#endif // RTFACT__VEC3F_HPP
