/*
 *  Packet.hpp
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

#ifndef RTFACT__PACKET_HPP
#define RTFACT__PACKET_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/type_traits/is_same.hpp>

#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/identity.hpp>
#include <boost/preprocessor/empty.hpp>

#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/push_back.hpp>
#include <boost/preprocessor/seq/filter.hpp>

#include <RTfact/Utils/Packets/SSEUtils.hpp>
#include <RTfact/Utils/Packets/Detail/Constants.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region Assertion macro definitions

#define RTFACT_PACKET_ASSERT(x)        RTFACT_ASSERT(x)
#define RTFACT_PACKET_STATIC_ASSERT(x) RTFACT_STATIC_ASSERT(x)

#define RTFACT_PACKET_STATIC_ASSERT_TYPE_IS_INT_OR_FLOAT(aType)                \
    RTFACT_PACKET_STATIC_ASSERT((boost::is_same<aType, int>::value ||          \
                               boost::is_same<aType, float>::value))

#pragma endregion // Assertion macro definitions

#pragma region Helper meta-classes

namespace Detail
{
    template<
        class tScalar>
    struct SIMDContainer;

    template<>
    struct SIMDContainer<float>
    {
        typedef float4 type;
    };

    template<>
    struct SIMDContainer<int>
    {
        typedef int4 type;
    };
}

template<
    uint taSize1,
    uint taSize2>
struct PacketOpResultSize;

template<
    uint taSize>
struct PacketOpResultSize<taSize, taSize>
{
    RTFACT_STATIC_CONSTANT(value, taSize);
};

template<>
struct PacketOpResultSize<1, 1>
{
    RTFACT_STATIC_CONSTANT(value, 1);
};

template<>
struct PacketOpResultSize<RTFACT_SIMD_WIDTH, 1>
{
    RTFACT_STATIC_CONSTANT(value, RTFACT_SIMD_WIDTH);
};

template<>
struct PacketOpResultSize<1, RTFACT_SIMD_WIDTH>
{
    RTFACT_STATIC_CONSTANT(value, RTFACT_SIMD_WIDTH);
};

template<>
struct PacketOpResultSize<RTFACT_SIMD_WIDTH, RTFACT_SIMD_WIDTH>
{
    enum { value = RTFACT_SIMD_WIDTH };
};

template<
    uint taSize>
struct PacketOpResultSize<1, taSize>
{
    enum { value = taSize };
};

template<
    uint taSize>
struct PacketOpResultSize<taSize, 1>
{
    enum { value = taSize };
};

template<
    uint taSize>
struct PacketOpResultSize<RTFACT_SIMD_WIDTH, taSize>
{
    enum { value = taSize };
};

template<
    uint taSize>
struct PacketOpResultSize<taSize, RTFACT_SIMD_WIDTH>
{
    enum { value = taSize };
};

#pragma endregion // Helper meta-classes

#pragma region Constants utility macros

#define RTFACT_MASK_TRUE  0xFFFFFFFF
#define RTFACT_MASK_FALSE 0

#define RTFACT__MASK_SPECS (1)(RTFACT_SIMD_WIDTH)

#define RTFACT__SEQ_INCLUDES(aElem, aSeq)                                      \
    BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_PUSH_BACK(                 \
        BOOST_PP_SEQ_FILTER(BOOST_PP_EQUAL_D, aElem, aSeq), _)), 1)

#define RTFACT_PACKET_PUT_TEMPLATE(aSize, aSpecSeq)                            \
    BOOST_PP_IF(RTFACT__SEQ_INCLUDES(aSize, aSpecSeq),                         \
                    BOOST_PP_EMPTY,                                            \
                    BOOST_PP_IDENTITY(template<>))()

#define RTFACT_PACKET_PUT_TYPENAME(aSize, aSpecSeq)                            \
    BOOST_PP_IF(RTFACT__SEQ_INCLUDES(aSize, aSpecSeq),                         \
                    BOOST_PP_IDENTITY(typename),                               \
                    BOOST_PP_EMPTY)()

#define RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(aName)                     \
    static const t_This aName;                                                 \
    static const t_This getConstant_##aName(const Scalar aValue)               \
    {                                                                          \
        return replicate(aValue);                                              \
    }

#define RTFACT_PACKET_DECLARE_SPECIAL_CONST_AND_INITIALIZER(aName, aValue)     \
    static const t_This aName;                                                 \
    static const t_This getConstant_##aName()                                  \
    {                                                                          \
        return replicate(static_cast<Scalar>(aValue));                         \
    }

#define RTFACT_PACKET_INIT_CONST(aSize, aBaseType, aName, aValue)              \
    template<> const Packet<aSize, aBaseType> Packet<aSize, aBaseType>::aName  \
        = Packet<aSize, aBaseType>::getConstant_##aName(aValue);

#define RTFACT_PACKET_INIT_SPECIAL_CONST(aSize, aBaseType, aName)              \
    template<> const Packet<aSize, aBaseType> Packet<aSize, aBaseType>::aName  \
        = Packet<aSize, aBaseType>::getConstant_##aName();

#define RTFACT_MASK_INIT_CONST(aSize, aName, aValue)                           \
    RTFACT_PACKET_PUT_TEMPLATE(aSize, RTFACT__MASK_SPECS)                      \
    const Mask<aSize> Mask<aSize>::aName                                       \
        = Mask<aSize>::getConstant_##aName(aValue);

#define RTFACT_PACKET_4_INIT_CONSTS                                            \
    RTFACT_QUAD_FLOAT_PACKET_INIT_CONSTS                                       \
    RTFACT_QUAD_INT_PACKET_INIT_CONSTS

#define RTFACT_PACKET_INIT_CONSTS(aSize)                                       \
    RTFACT_FLOAT_PACKET_INIT_CONSTS(aSize)                                     \
    RTFACT_INT_PACKET_INIT_CONSTS(aSize)                                       \
    BOOST_PP_IF(BOOST_PP_EQUAL(aSize, RTFACT_SIMD_WIDTH),                      \
                    RTFACT_PACKET_4_INIT_CONSTS,                               \
                    BOOST_PP_EMPTY())

#pragma endregion // Constants utility macros

#pragma region Loops utility macros

#define RTFACT_PACKET_FOR_ALL_INIT(aIndexVar, aInit, aSize)                    \
    for(uint aIndexVar = 0; aIndexVar < aSize; ++aIndexVar)

#define RTFACT_PACKET_FOR_ALL(aIndexVar, aSize)                                \
    RTFACT_PACKET_FOR_ALL_INIT(aIndexVar, 0, aSize)

#define RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(aIndexVar, aInit, aSize)         \
    for(uint aIndexVar = aInit;                                                \
        aIndexVar < Packet<aSize, float>::CONTAINER_COUNT; ++aIndexVar)

#define RTFACT_PACKET_FOR_ALL_ITEMS_IN_CONTAINER_INIT(aIndexVar, aInit, aSize)          \
    for(uint aIndexVar = aInit;                                                         \
    aIndexVar < Packet<aSize, float>::Container::SIZE; ++aIndexVar)

#define RTFACT_PACKET_FOR_ALL_ITEMS_IN_CONTAINER(aIndexVar, aSize)              \
    RTFACT_PACKET_FOR_ALL_ITEMS_IN_CONTAINER_INIT(aIndexVar, 0, aSize)

#define RTFACT_PACKET_FOR_ALL_CONTAINERS(aIndexVar, aSize)                     \
    RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(aIndexVar, 0, aSize)

#define RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(aIt, aActiveMask)              \
    for(uint __iter = 0,                                                       \
            aIt = aActiveMask.getActiveContainerAtPrivate(0);                  \
        __iter < aActiveMask.getActiveContainerEndIndex();                     \
        aIt = aActiveMask.getActiveContainerAtPrivate(++__iter))

#define RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS_INIT(aIt, aInit, aActiveMask)  \
    for(uint __iter = aInit,                                                   \
            aIt = aActiveMask.getActiveContainerAtPrivate(aInit);              \
        __iter < aActiveMask.getActiveContainerEndIndex();                     \
        aIt = aActiveMask.getActiveContainerAtPrivate(++__iter))

#define RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS_INIT(aIt, aInit, aActiveMask)  \
    for(uint __iter = aInit,                                                   \
            aIt = aActiveMask.getActiveContainerAtPrivate(aInit);              \
        __iter < aActiveMask.getActiveContainerEndIndex();                     \
        aIt = aActiveMask.getActiveContainerAtPrivate(++__iter))


#pragma endregion // Loops utility macros

#pragma region Forward declarations

template<
    uint taSize>
class Mask;

template<
    uint taSize>
class ActiveMask;

template<
    uint taSize>
class Vec2f;

template<
    uint taSize>
class Vec3f;

template<
    uint taSize,
    class tScalar>
class Packet;

#pragma endregion // Forward declarations

RTFACT_NAMESPACE_END

#include <RTfact/Utils/Math/CompileTimeMathUtils.hpp>

#include <RTfact/Utils/Packets/BitMask.hpp>

#include <RTfact/Utils/Packets/Detail/Mask1.hpp>
#include <RTfact/Utils/Packets/Detail/Mask4.hpp>
#include <RTfact/Utils/Packets/Detail/MaskGeneric.hpp>

#include <RTfact/Utils/Packets/Detail/ActiveMask.hpp>

#include <RTfact/Utils/Packets/Detail/Packet1.hpp>
#include <RTfact/Utils/Packets/Detail/Packet4.hpp>
#include <RTfact/Utils/Packets/Detail/PacketGeneric.hpp>


RTFACT_NAMESPACE_BEGIN

template<
    uint taSize>
template<
    class tScalar>
RTFACT_PACKET_INLINE const Packet<taSize, tScalar> Mask<taSize>::iif(
    const Packet<taSize, tScalar>& aPacket1,
    const Packet<taSize, tScalar>& aPacket2) const
{
    Packet<taSize, tScalar> ret;

    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
    {
        ret(i).data = containers[i].iif(aPacket1(i), aPacket2(i));
    }

    return ret;
}

template<
    class tScalar>
RTFACT_PACKET_INLINE const Packet<4, tScalar> Mask<4>::iif(
    const Packet<4, tScalar>& aPacket1,
    const Packet<4, tScalar>& aPacket2) const
{
    return Packet<4, tScalar>(data.blend(aPacket1.data, aPacket2.data));
}

template<
    class tScalar>
RTFACT_PACKET_INLINE const Packet<1, tScalar>& Mask<1>::iif(
    const Packet<1, tScalar>& aPacket1,
    const Packet<1, tScalar>& aPacket2) const
{
    return (data ? aPacket1 : aPacket2);
}

typedef Packet<4, float> FPacketC;
typedef Packet<4, int>   IPacketC;
typedef Mask<4>          MaskC;
typedef BitMask<4>       BitMaskC;

typedef Packet<1, float> FPacket1;
typedef Packet<1, int>  IPacket1;

RTFACT_NAMESPACE_END

#include <RTfact/Utils/Math/Detail/PacketMathUtils.hpp>

#pragma region Undefine hepler macros

#undef RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER
#undef RTFACT_PACKET_DECLARE_CONSTS
#undef RTFACT_MASK_DECLARE_CONSTS
#undef RTFACT_PACKET_4_DECLARE_CONSTS

#pragma endregion // Undefine hepler macros

#endif // RTFACT__PACKET_HPP
