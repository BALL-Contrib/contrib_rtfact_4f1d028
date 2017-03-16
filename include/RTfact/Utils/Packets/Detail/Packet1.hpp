/* 
 *  Packet1.hpp
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
 *  Created on: 2008-11-03 01:12:24 +0100
 *  Author(s): Iliyan Georgiev, Dmitri Rubinstein
 */
 
 #ifndef RTFACT__SINGLEPACKET_HPP
#define RTFACT__SINGLEPACKET_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__PACKET_1_ARITH_OP(aOperator)                                   \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aPacket) const                                           \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = data aOperator aPacket.data;                                \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aPacket)                                                 \
    {                                                                          \
        data aOperator##= aPacket.data;                                        \
        return *this;                                                          \
    }

#define RTFACT__PACKET_1_BITWISE_OP(aOperator)                                 \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aPacket) const                                           \
    {                                                                          \
        t_This ret;                                                            \
        const int temp = *reinterpret_cast<const int*>(&data) aOperator        \
                         *reinterpret_cast<const int*>(&aPacket.data);         \
        ret.data = *reinterpret_cast<const Scalar*>(&temp);                    \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aPacket)                                                 \
    {                                                                          \
        const int temp = *reinterpret_cast<const int*>(&data) aOperator        \
                         *reinterpret_cast<const int*>(&aPacket.data);         \
        data = *reinterpret_cast<const Scalar*>(&temp);                        \
        return *this;                                                          \
    }

#define RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(aOperator)         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Packet<taSize, Scalar> operator aOperator(      \
        const Packet<taSize, Scalar>& aPacket) const;                          \
    RTFACT_PACKET_INLINE const Packet<4, Scalar> operator aOperator(           \
        const Packet<4, Scalar>& aPacket) const;

#define RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(aOperator)            \
    template<                                                                  \
        class tScalar>                                                         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Packet<taSize, tScalar>                         \
    Packet<1, tScalar>::operator aOperator(                                    \
        const Packet<taSize, tScalar>& aPacket) const                          \
    {                                                                          \
        Packet<taSize, tScalar> ret;                                           \
        const typename Packet<taSize, tScalar>::Data tempData =                \
            Packet<taSize, tScalar>::Data::replicate(data);                    \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = tempData aOperator aPacket(i).data;                  \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(aOperator)                  \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Packet<4, tScalar>                              \
    Packet<1, tScalar>::operator aOperator(                                    \
        const Packet<4, tScalar>& aPacket) const                               \
    {                                                                          \
        return Packet<4, tScalar>(                                             \
            Packet<4, tScalar>::Data::replicate(data) aOperator                \
            aPacket.data);                                                     \
    }

#define RTFACT__PACKET_1_ARITH_OP_SCALAR(aOperator)                            \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = data aOperator aValue;                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        data aOperator##= aValue;                                              \
        return *this;                                                          \
    }

#define RTFACT__PACKET_1_BITWISE_OP_SCALAR(aOperator)                          \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        t_This ret;                                                            \
        const int temp = *reinterpret_cast<const int*>(&data) aOperator        \
                         *reinterpret_cast<const int*>(&aValue);               \
        ret.data = *reinterpret_cast<const Scalar*>(&temp);                    \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        const int temp = *reinterpret_cast<const int*>(&data) aOperator        \
                         *reinterpret_cast<const int*>(&aValue);               \
        data = *reinterpret_cast<const Scalar*>(&temp);                        \
        return *this;                                                          \
    }

#define RTFACT__PACKET_1_UNARY_ARITH_OP(aOperator)                             \
    RTFACT_PACKET_INLINE const t_This operator aOperator() const               \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = aOperator data;                                             \
        return ret;                                                            \
    }

#define RTFACT__PACKET_1_COMPARISON_OP(aOperator)                              \
    RTFACT_PACKET_INLINE const Mask<1> operator aOperator(                     \
        const t_This& aPacket) const                                           \
    {                                                                          \
        return Mask<1>(data aOperator aPacket.data);                           \
    }

#define RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(aOperator)    \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize> operator aOperator(                \
        const Packet<taSize, Scalar>& aPacket) const;                          \
    RTFACT_PACKET_INLINE const Mask<4> operator aOperator(                     \
        const Packet<4, Scalar>& aPacket) const;

#define RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(aOperator)        \
    template<                                                                  \
        class tScalar>                                                         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize>                                    \
    Packet<1, tScalar>::operator aOperator(                                    \
        const Packet<taSize, tScalar>& aPacket) const                          \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        const typename Packet<taSize, tScalar>::Data tempData =                \
            Packet<taSize, tScalar>::Data::replicate(data);                    \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = tempData aOperator aPacket(i).data;                  \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(aOperator)              \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Mask<4>                                         \
    Packet<1, tScalar>::operator aOperator(                                    \
        const Packet<4, tScalar>& aPacket) const                               \
    {                                                                          \
        return Mask<4>(                                                        \
            Packet<4, tScalar>::Data::replicate(data) aOperator                \
            aPacket.data);                                                     \
    }

#define RTFACT__PACKET_1_COMPARISON_OP_SCALAR(aOperator)                       \
    RTFACT_PACKET_INLINE const Mask<1> operator aOperator(                     \
        const Scalar aValue) const                                             \
    {                                                                          \
        return Mask<1>(data aOperator aValue);                                 \
    }

#define RTFACT__SCALAR_ARITH_OP_PACKET_1(aOperator)                            \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Packet<1, tScalar> operator aOperator(          \
        const tScalar aValue,                                                  \
        const Packet<1, tScalar>& aPacket)                                     \
    {                                                                          \
        Packet<1, tScalar> ret;                                                \
        ret.data = aValue aOperator aPacket.data;                              \
        return ret;                                                            \
    }

#define RTFACT__SCALAR_BITWISE_OP_PACKET_1(aOperator)                          \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Packet<1, tScalar> operator aOperator(          \
        const tScalar aValue,                                                  \
        const Packet<1, tScalar>& aPacket)                                     \
    {                                                                          \
        Packet<1, tScalar> ret;                                                \
        const int temp = *reinterpret_cast<const int*>(&aValue) aOperator      \
                         *reinterpret_cast<const int*>(&aPacket.data);         \
        ret.data = *reinterpret_cast<const tScalar*>(&temp);                   \
        return ret;                                                            \
    }

#define RTFACT__SCALAR_COMPARISON_OP_PACKET_1(aOperator)                       \
    template<                                                                  \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Mask<1> operator aOperator(                     \
        const tScalar aValue,                                                  \
        const Packet<1, tScalar>& aPacket)                                     \
    {                                                                          \
        return Mask<1>(aValue aOperator aPacket.data);                         \
    }

#define RTFACT__PACKET_1_ARITH_OP_VEC3F__DECLARE(aOperator)                    \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Vec3f<taSize> operator aOperator(               \
        const Vec3f<taSize>& aVec) const;

#pragma endregion

template<
    class tScalar>
class Packet<1, tScalar>
{
    // Only int and float are allowed as instantiation types
    RTFACT_PACKET_STATIC_ASSERT_TYPE_IS_INT_OR_FLOAT(tScalar);

    typedef Packet<1, tScalar> t_This;

public:

    enum
    {
        SIZE = 1,
        SIZE_SQRT = 1,
        CONTAINER_COUNT = 1
    };

    typedef tScalar Scalar;
    typedef Scalar  Data;
    typedef t_This  Container;

    Data data;

    RTFACT_PACKET_INLINE t_This& operator=(
        const Scalar aValue)
    {
        data = aValue;

        return *this;
    }

    RTFACT_PACKET_INLINE operator const Scalar&() const
    {
        return data;
    }

    RTFACT_PACKET_INLINE operator Scalar&()
    {
        return data;
    }

    #pragma region Constants

    RTFACT_PACKET_DECLARE_CONSTS

    static RTFACT_PACKET_INLINE const t_This C_0()
    {
        t_This ret;

        ret.data = static_cast<Scalar>(0);

        return ret;
    }

    #pragma endregion

    #pragma region Accessors

    RTFACT_PACKET_INLINE const Scalar operator[](
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        return data;
    }

    RTFACT_PACKET_INLINE Scalar& operator[](
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        return data;
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
        return data;
    }

    RTFACT_PACKET_INLINE const Scalar max() const
    {
        return data;
    }

    RTFACT_PACKET_INLINE const bool containsSameValue() const
    {
        return true;
    }

    RTFACT_PACKET_INLINE const BitMask<1> getSignBitMask() const
    {
        return BitMask<1>(data < 0);
    }

    RTFACT_PACKET_INLINE const uint getSignIntMask() const
    {
        return (data < 0);
    }

    RTFACT_PACKET_INLINE Scalar addHoriz() const
    {
        return data;
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValues(
        tIterator aBegin,
        tIterator aEnd,
        const Scalar = 1) const
    {
        *aBegin++ = data;

        return aBegin;
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValues(
        tIterator aBegin,
        tIterator aEnd,
        const Mask<1>& aMask,
        const Scalar = 1) const
    {
        if(aMask.isTrue())
        {
            *aBegin++ = data;
        }

        return aBegin;
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Scalar aValue)
    {
        t_This ret;

        ret.data = aValue;

        return ret;
    }

    static RTFACT_PACKET_INLINE const t_This& replicate(
        const t_This& aPacket)
    {
        return aPacket;
    }

    #pragma endregion

    #pragma region Operators

    RTFACT__PACKET_1_ARITH_OP(+)
    RTFACT__PACKET_1_ARITH_OP(-)
    RTFACT__PACKET_1_ARITH_OP(*)
    RTFACT__PACKET_1_ARITH_OP(/)

    RTFACT__PACKET_1_BITWISE_OP(|)
    RTFACT__PACKET_1_BITWISE_OP(&)
    RTFACT__PACKET_1_BITWISE_OP(^)

    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(+)
    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(-)
    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(*)
    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(/)
    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(|)
    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(&)
    RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(^)

    RTFACT__PACKET_1_UNARY_ARITH_OP(-)
    RTFACT__PACKET_1_UNARY_ARITH_OP(+)

    RTFACT__PACKET_1_ARITH_OP_SCALAR(*)
    RTFACT__PACKET_1_ARITH_OP_SCALAR(/)
    RTFACT__PACKET_1_ARITH_OP_SCALAR(+)
    RTFACT__PACKET_1_ARITH_OP_SCALAR(-)

    RTFACT__PACKET_1_BITWISE_OP_SCALAR(|)
    RTFACT__PACKET_1_BITWISE_OP_SCALAR(&)
    RTFACT__PACKET_1_BITWISE_OP_SCALAR(^)

    RTFACT__PACKET_1_COMPARISON_OP(> )
    RTFACT__PACKET_1_COMPARISON_OP(>=)
    RTFACT__PACKET_1_COMPARISON_OP(< )
    RTFACT__PACKET_1_COMPARISON_OP(<=)
    RTFACT__PACKET_1_COMPARISON_OP(==)
    RTFACT__PACKET_1_COMPARISON_OP(!=)

    RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(> )
    RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(>=)
    RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(< )
    RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(<=)
    RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(==)
    RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE(!=)

    RTFACT__PACKET_1_COMPARISON_OP_SCALAR(> )
    RTFACT__PACKET_1_COMPARISON_OP_SCALAR(>=)
    RTFACT__PACKET_1_COMPARISON_OP_SCALAR(< )
    RTFACT__PACKET_1_COMPARISON_OP_SCALAR(<=)
    RTFACT__PACKET_1_COMPARISON_OP_SCALAR(==)
    RTFACT__PACKET_1_COMPARISON_OP_SCALAR(!=)

    RTFACT__PACKET_1_ARITH_OP_VEC3F__DECLARE(*)
    RTFACT__PACKET_1_ARITH_OP_VEC3F__DECLARE(/)

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_ARITH_OP_PACKET_1(*)
RTFACT__SCALAR_ARITH_OP_PACKET_1(/)
RTFACT__SCALAR_ARITH_OP_PACKET_1(+)
RTFACT__SCALAR_ARITH_OP_PACKET_1(-)

RTFACT__SCALAR_BITWISE_OP_PACKET_1(|)
RTFACT__SCALAR_BITWISE_OP_PACKET_1(&)
RTFACT__SCALAR_BITWISE_OP_PACKET_1(^)

RTFACT__SCALAR_COMPARISON_OP_PACKET_1(> )
RTFACT__SCALAR_COMPARISON_OP_PACKET_1(>=)
RTFACT__SCALAR_COMPARISON_OP_PACKET_1(< )
RTFACT__SCALAR_COMPARISON_OP_PACKET_1(<=)
RTFACT__SCALAR_COMPARISON_OP_PACKET_1(==)
RTFACT__SCALAR_COMPARISON_OP_PACKET_1(!=)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__PACKET_1_ARITH_OP
#undef RTFACT__PACKET_1_ARITH_OP_GENERAL_PACKET_4__DECLARE
#undef RTFACT__PACKET_1_ARITH_OP_SCALAR
#undef RTFACT__PACKET_1_UNARY_ARITH_OP
#undef RTFACT__PACKET_1_COMPARISON_OP
#undef RTFACT__PACKET_1_COMPARISON_OP_GENERAL_PACKET_4__DECLARE
#undef RTFACT__PACKET_1_COMPARISON_OP_SCALAR
#undef RTFACT__SCALAR_ARITH_OP_PACKET_1
#undef RTFACT__SCALAR_COMPARISON_OP_PACKET_1
#undef RTFACT__PACKET_1_ARITH_OP_VEC3F__DECLARE

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__SINGLEPACKET_HPP
