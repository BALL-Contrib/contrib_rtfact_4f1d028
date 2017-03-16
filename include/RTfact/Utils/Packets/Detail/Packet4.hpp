/* 
 *  Packet4.hpp
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
 *  Created on: 2008-11-03 01:12:24 +0100
 *  Author(s): Iliyan Georgiev, Georgiev Iliyan
 */
 
 #ifndef RTFACT__QUADPACKET_HPP
#define RTFACT__QUADPACKET_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__PACKET_4_ARITH_OP(aOperator)                                   \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aPacket) const                                           \
    {                                                                          \
        return t_This(data aOperator aPacket.data);                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Packet<1, Scalar>& aPacket) const                                \
    {                                                                          \
        return t_This(data aOperator Data::replicate(aPacket.data));           \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aPacket)                                                 \
    {                                                                          \
        data aOperator##= aPacket.data;                                        \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Packet<1, Scalar>& aPacket)                                      \
    {                                                                          \
        data aOperator##= Data::replicate(aPacket.data);                       \
        return *this;                                                          \
    }

#define RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(aOperator)           \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Packet<taSize, Scalar> operator aOperator(      \
        const Packet<taSize, Scalar>& aPacket) const;

#define RTFACT_PACKET_4_ARITH_OP_PACKET_GENERIC__DEFINE(aOperator)             \
    template<                                                                  \
        class tScalar>                                                         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Packet<taSize, tScalar>                         \
    Packet<4, tScalar>::operator aOperator(                                    \
        const Packet<taSize, tScalar>& aPacket) const                          \
    {                                                                          \
        Packet<taSize, tScalar> ret;                                           \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = data aOperator aPacket(i).data;                      \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__PACKET_4_ARITH_OP_SCALAR(aOperator)                            \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        return t_This(data aOperator Data::replicate(aValue));                 \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        data aOperator##= Data::replicate(aValue);                             \
        return *this;                                                          \
    }

#define RTFACT__PACKET_4_UNARY_ARITH_OP(aOperator)                             \
    RTFACT_PACKET_INLINE const t_This operator aOperator() const               \
    {                                                                          \
        return t_This( aOperator data);                                        \
    }

#define RTFACT__PACKET_4_COMPARISON_OP(aOperator)                              \
    RTFACT_PACKET_INLINE const Mask<4> operator aOperator(                     \
        const t_This& aPacket) const                                           \
    {                                                                          \
        return Mask<4>(data aOperator aPacket.data);                           \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const Mask<4> operator aOperator(                     \
        const Packet<1, Scalar>& aPacket) const                                \
    {                                                                          \
        return Mask<4>(data aOperator Data::replicate(aPacket.data));          \
    }

#define RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(aOperator)       \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize> operator aOperator(                \
        const Packet<taSize, Scalar>& aPacket) const;

#define RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(aOperator)         \
    template<                                                                  \
        class tScalar>                                                         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize>                                    \
    Packet<4, tScalar>::operator aOperator(                                    \
        const Packet<taSize, tScalar>& aPacket) const                          \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = data aOperator aPacket(i).data;                      \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__PACKET_4_COMPARISON_OP_SCALAR(aOperator)                       \
    RTFACT_PACKET_INLINE const Mask<4> operator aOperator(                     \
        const Scalar aValue) const                                             \
    {                                                                          \
        return Mask<4>(data aOperator Data::replicate(aValue));                \
    }

#define RTFACT__SCALAR_ARITH_OP_PACKET_4(aOperator)                            \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Packet<4, tScalar> operator aOperator(          \
        const tScalar aValue,                                                  \
        const Packet<4, tScalar>& aPacket)                                     \
    {                                                                          \
        return Packet<4, tScalar>(                                             \
            Packet<4, tScalar>::Data::replicate(aValue) aOperator              \
            aPacket.data);                                                     \
    }

#define RTFACT__SCALAR_COMPARISON_OP_PACKET_4(aOperator)                       \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Mask<4> operator aOperator(                     \
        const tScalar aValue,                                                  \
        const Packet<4, tScalar>& aPacket)                                     \
    {                                                                          \
        return Mask<4>(                                                        \
            Packet<4, tScalar>::Container::replicate(aValue) aOperator         \
            aPacket);                                                     \
    }

#define RTFACT__PACKET_4_ARITH_OP_VEC3F__DECLARE(aOperator)                    \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Vec3f<taSize> operator aOperator(               \
        const Vec3f<taSize>& aVec) const;                                      \
    RTFACT_PACKET_INLINE const Vec3f<4> operator aOperator (                   \
        const Vec3f<1>& aVec) const;

#pragma endregion

template<
    class tScalar>
class Packet<4, tScalar>
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    // Only int and float are allowed as instantiation types
    RTFACT_PACKET_STATIC_ASSERT_TYPE_IS_INT_OR_FLOAT(tScalar);

    typedef Packet<4, tScalar> t_This;

public:

    enum
    {
        SIZE = 4,
        SIZE_SQRT = 2,
        CONTAINER_COUNT = 1
    };
    
    typedef tScalar                                      Scalar;
    typedef typename Detail::SIMDContainer<Scalar>::type Data;
    typedef t_This                                       Container;

    Data data;

    #pragma region Constants

    RTFACT_PACKET_DECLARE_CONSTS
    RTFACT_PACKET_4_DECLARE_CONSTS

    static RTFACT_PACKET_INLINE const t_This C_0()
    {
        return t_This(Data::C_0_0_0_0());
    }

    #pragma endregion

    #pragma region Constructors

    Packet()
    {}

    RTFACT_PACKET_INLINE explicit Packet(
        const Data& aData
    ) : 
        data(aData)
    {}

    RTFACT_PACKET_INLINE Packet(
        const Scalar aValue1,
        const Scalar aValue2,
        const Scalar aValue3,
        const Scalar aValue4
    ) :
        data(Data::getInstance(aValue1, aValue2, aValue3, aValue4))
    {}

    #pragma endregion

    #pragma region Accessors

    RTFACT_PACKET_INLINE const Scalar& operator[](
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex < SIZE);

        return data[aIndex];
    }

    RTFACT_PACKET_INLINE Scalar& operator[](
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex < SIZE);

        return data[aIndex];
    }

    RTFACT_PACKET_INLINE const Container& operator()(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_PACKET_INLINE Container& operator()(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        return *this;
    }

    #pragma endregion

    #pragma region Utility methods

    RTFACT_PACKET_INLINE const Scalar min() const
    {
        return data.min4();
    }

    RTFACT_PACKET_INLINE const Scalar max() const
    {
        return data.max4();
    }

    RTFACT_PACKET_INLINE const bool containsSameValue() const
    {
        return (data[0] == data[1] && data[1] == data[2] && data[2] == data[3]);
    }

    RTFACT_PACKET_INLINE const BitMask<4> getSignBitMask() const
    {
        return BitMask<4>(data.getI32Mask());
    }

    RTFACT_PACKET_INLINE const uint getSignIntMask() const
    {
        return static_cast<uint>(data.getI32Mask());
    }

    template<
        uint ta0,
        uint ta1,
        uint ta2,
        uint ta3>
    RTFACT_PACKET_INLINE const t_This shuffle() const
    {
        return t_This(data.template shuffle<ta0, ta1, ta2, ta3>());
    }

    RTFACT_PACKET_INLINE Scalar addHoriz() const
    {
        return data.addHoriz().template at<0>();
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValues(
        tIterator aBegin,
        tIterator aEnd,
        const Scalar aInvalidValue = -1) const
    {
        *aBegin++ = data[0];

        if(data[1] != data[0])
        {
            *aBegin++ = data[1];
        }

        if(data[2] != data[0] && data[2] != data[1])
        {
            *aBegin++ = data[2];
        }

        if(data[3] != data[0] && data[3] != data[1] && data[3] != data[2])
        {
            *aBegin++ = data[3];
        }

        return aBegin;
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValues(
        tIterator aBegin,
        tIterator aEnd,
        const Mask<4>& aMask,
        const Scalar aInvalidValue = -1) const
    {
        if(aMask[0])
        {
            *aBegin++ = data[0];
        }

        if(aMask[1] && (!aMask[0] || data[1] != data[0]))
        {
            *aBegin++ = data[1];
        }

        if(aMask[2] && (!aMask[0] || data[2] != data[0]) && (!aMask[1] || data[2] != data[1]))
        {
            *aBegin++ = data[2];
        }

        if(aMask[3] && (!aMask[0] || data[3] != data[0]) && (!aMask[1] || data[3] != data[1]) && (!aMask[2] || data[3] != data[2]))
        {
            *aBegin++ = data[3];
        }

        return aBegin;
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Data& aData)
    {
        return t_This(aData);
    }

    static RTFACT_PACKET_INLINE t_This replicate(
        Scalar aValue)
    {
        return t_This(Data::replicate(aValue));
    }

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Packet<1, Scalar>& aPacket)
    {
        return replicate(aPacket.data);
    }

    static RTFACT_PACKET_INLINE const t_This& replicate(
        const t_This& aPacket)
    {
        return aPacket;
    }

    #pragma endregion

    #pragma region Operators

    RTFACT__PACKET_4_ARITH_OP(+)
    RTFACT__PACKET_4_ARITH_OP(-)
    RTFACT__PACKET_4_ARITH_OP(*)
    RTFACT__PACKET_4_ARITH_OP(/)
    RTFACT__PACKET_4_ARITH_OP(|)
    RTFACT__PACKET_4_ARITH_OP(&)
    RTFACT__PACKET_4_ARITH_OP(^)

    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(+)
    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(-)
    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(*)
    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(/)
    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(|)
    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(&)
    RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE(^)

    RTFACT__PACKET_4_UNARY_ARITH_OP(-)
    RTFACT__PACKET_4_UNARY_ARITH_OP(+)

    RTFACT__PACKET_4_ARITH_OP_SCALAR(*)
    RTFACT__PACKET_4_ARITH_OP_SCALAR(/)
    RTFACT__PACKET_4_ARITH_OP_SCALAR(+)
    RTFACT__PACKET_4_ARITH_OP_SCALAR(-)
    RTFACT__PACKET_4_ARITH_OP_SCALAR(|)
    RTFACT__PACKET_4_ARITH_OP_SCALAR(&)
    RTFACT__PACKET_4_ARITH_OP_SCALAR(^)

    RTFACT__PACKET_4_COMPARISON_OP(> )
    RTFACT__PACKET_4_COMPARISON_OP(>=)
    RTFACT__PACKET_4_COMPARISON_OP(< )
    RTFACT__PACKET_4_COMPARISON_OP(<=)
    RTFACT__PACKET_4_COMPARISON_OP(==)
    RTFACT__PACKET_4_COMPARISON_OP(!=)

    RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(> )
    RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(>=)
    RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(< )
    RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(<=)
    RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(==)
    RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE(!=)

    RTFACT__PACKET_4_COMPARISON_OP_SCALAR(> )
    RTFACT__PACKET_4_COMPARISON_OP_SCALAR(>=)
    RTFACT__PACKET_4_COMPARISON_OP_SCALAR(< )
    RTFACT__PACKET_4_COMPARISON_OP_SCALAR(<=)
    RTFACT__PACKET_4_COMPARISON_OP_SCALAR(==)
    RTFACT__PACKET_4_COMPARISON_OP_SCALAR(!=)

    RTFACT__PACKET_4_ARITH_OP_VEC3F__DECLARE(*)
    RTFACT__PACKET_4_ARITH_OP_VEC3F__DECLARE(/)

    #pragma endregion

    #pragma region Vec2f arithmetic operator declarations

    const Vec2f<4> operator * (
        const Vec2f<4>& aVec) const;

    const Vec2f<4> operator * (
        const Vec2f<1>& aVec) const;

    const Vec2f<4> operator / (
        const Vec2f<4>& aVec) const;

    const Vec2f<4> operator / (
        const Vec2f<1>& aVec) const;

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_ARITH_OP_PACKET_4(*)
RTFACT__SCALAR_ARITH_OP_PACKET_4(/)
RTFACT__SCALAR_ARITH_OP_PACKET_4(+)
RTFACT__SCALAR_ARITH_OP_PACKET_4(-)
RTFACT__SCALAR_ARITH_OP_PACKET_4(|)
RTFACT__SCALAR_ARITH_OP_PACKET_4(&)
RTFACT__SCALAR_ARITH_OP_PACKET_4(^)

RTFACT__SCALAR_COMPARISON_OP_PACKET_4(> )
RTFACT__SCALAR_COMPARISON_OP_PACKET_4(>=)
RTFACT__SCALAR_COMPARISON_OP_PACKET_4(< )
RTFACT__SCALAR_COMPARISON_OP_PACKET_4(<=)
RTFACT__SCALAR_COMPARISON_OP_PACKET_4(==)
RTFACT__SCALAR_COMPARISON_OP_PACKET_4(!=)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__PACKET_4_ARITH_OP
#undef RTFACT__PACKET_4_ARITH_OP_PACKET_GENERIC__DECLARE
#undef RTFACT__PACKET_4_ARITH_OP_SCALAR
#undef RTFACT__PACKET_4_UNARY_ARITH_OP
#undef RTFACT__PACKET_4_COMPARISON_OP
#undef RTFACT__PACKET_4_COMPARISON_OP_PACKET_GENERIC_DECLARE
#undef RTFACT__PACKET_4_COMPARISON_OP_SCALAR
#undef RTFACT__SCALAR_ARITH_OP_PACKET_4
#undef RTFACT__SCALAR_COMPARISON_OP_PACKET_4
#undef RTFACT__PACKET_4_ARITH_OP_VEC3F__DECLARE

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__QUADPACKET_HPP
