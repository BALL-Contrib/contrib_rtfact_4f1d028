/* 
 *  PacketGeneric.hpp
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
 *  Created on: 2008-11-03 01:12:24 +0100
 *  Author(s): Iliyan Georgiev, Byelozyorov Sergiy
 */
 
 #ifndef RTFACT__GENERALPACKET_HPP
#define RTFACT__GENERALPACKET_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__PACKET_GENERIC_ARITH_OP(aOperator)                             \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aPacket) const                                           \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aPacket.containers[i]; \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Packet<1, Scalar>& aPacket) const                                \
    {                                                                          \
        t_This ret;                                                            \
        const Container tempPacket = Container::replicate(aPacket);            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator tempPacket;            \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Packet<4, Scalar>& aPacket) const                                \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aPacket;               \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aPacket)                                                 \
    {                                                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= aPacket.containers[i];                  \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Packet<1, Scalar>& aPacket)                                      \
    {                                                                          \
        const Container tempPacket = Container::replicate(aPacket);            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= tempPacket;                             \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Packet<4, Scalar>& aPacket)                                      \
    {                                                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= aPacket;                                \
        }                                                                      \
        return *this;                                                          \
    }

#define RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(aOperator)                      \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        t_This ret;                                                            \
        const Container tempPacket = Container::replicate(aValue);             \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator tempPacket;            \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        const Container tempPacket = Container::replicate(aValue);             \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= tempPacket;                             \
        }                                                                      \
        return *this;                                                          \
    }

#define RTFACT__PACKET_GENERIC_UNARY_ARITH_OP(aOperator)                       \
    RTFACT_PACKET_INLINE t_This operator aOperator() const                     \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = aOperator containers[i];                       \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__PACKET_GENERIC_COMPARISON_OP(aOperator)                        \
    RTFACT_PACKET_INLINE const Mask<SIZE> operator aOperator(                  \
        const t_This& aPacket) const                                           \
    {                                                                          \
        Mask<SIZE> ret;                                                        \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret(i) = containers[i] aOperator aPacket.containers[i];            \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const Mask<SIZE> operator aOperator(                  \
        const Packet<1, Scalar>& aPacket) const                                \
    {                                                                          \
        Mask<SIZE> ret;                                                        \
        const Container tempPacket = Container::replicate(aPacket);            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret(i) = containers[i] aOperator tempPacket;                       \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const Mask<SIZE> operator aOperator(                  \
        const Packet<4, Scalar>& aPacket) const                                \
    {                                                                          \
        Mask<SIZE> ret;                                                        \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret(i) = containers[i] aOperator aPacket;                          \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(aOperator)                 \
    RTFACT_PACKET_INLINE const Mask<SIZE> operator aOperator(                  \
        const Scalar aValue) const                                             \
    {                                                                          \
        Mask<SIZE> ret;                                                        \
        const Container tempPacket = Container::replicate(aValue);             \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret(i) = containers[i] aOperator tempPacket;                       \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(aOperator)                      \
    template<                                                                  \
        uint taSize,                                                           \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Packet<taSize, tScalar> operator aOperator(     \
        const tScalar aValue,                                                  \
        const Packet<taSize, tScalar>& aPacket)                                \
    {                                                                          \
        Packet<taSize, tScalar> ret;                                           \
        const typename Packet<taSize, tScalar>::Container tempPacket =         \
            Packet<taSize, tScalar>::Container::replicate(aValue);             \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i) = tempPacket aOperator aPacket(i);                          \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(aOperator)                 \
    template<                                                                  \
        uint taSize,                                                           \
        class tScalar>                                                         \
    RTFACT_PACKET_INLINE const Mask<taSize> operator aOperator(                \
        const tScalar aValue,                                                  \
        const Packet<taSize, tScalar>& aPacket)                                \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        const typename Packet<taSize, tScalar>::Container tempPacket =         \
            Packet<taSize, tScalar>::Container::replicate(aValue);             \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i) = tempPacket aOperator aPacket(i);                          \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__PACKET_GENERIC_ARITH_OP_VEC3F__DECLARE(aOperator)              \
    RTFACT_PACKET_INLINE const Vec3f<taSize> operator aOperator(               \
        const Vec3f<taSize>& aVec) const;                                      \
    RTFACT_PACKET_INLINE const Vec3f<4> operator aOperator(                    \
        const Vec3f<4>& aVec) const;                                           \
    RTFACT_PACKET_INLINE const Vec3f<1> operator aOperator(                    \
        const Vec3f<1>& aVec) const;

#pragma endregion

template<
    uint taSize,
    class tScalar>
class Packet
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    // Size can only be greater or equal than 16 and a multiple of 4
    RTFACT_PACKET_STATIC_ASSERT(( (taSize >= 16) && (taSize % 4 == 0) ));
    RTFACT_PACKET_STATIC_ASSERT_TYPE_IS_INT_OR_FLOAT(tScalar);

    // Uncomment to restrict the SIZE to a power of 4 and greater than or equal to 16
    //RTFACT_PACKET_STATIC_ASSERT(( taSize >= 16 && ((taSize & (taSize - 1)) == 0) && (taSize & 0x55555555) ));

    typedef Packet<taSize, tScalar> t_This;

public:

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef tScalar                                         Scalar;
    typedef typename Detail::SIMDContainer<Scalar>::type    Data;
    typedef Packet<4, Scalar>                               Container;

    Container containers[CONTAINER_COUNT];

    #pragma region Constants

    RTFACT_PACKET_DECLARE_CONSTS

    static RTFACT_PACKET_INLINE const t_This C_0()
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::C_0();
        }

        return ret;
    }

    #pragma endregion

    #pragma region Constructors

    #pragma endregion

    #pragma region Accessors

    RTFACT_PACKET_INLINE const Scalar operator[](
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex < SIZE);

        //#ifdef RTFACT_DEBUG
        //#   pragma message("SIMD type layout assumption!")
        //#endif
        //return containers[aIndex / 4][aIndex % 4];
        return reinterpret_cast<const Scalar*>(this)[aIndex];
    }

    RTFACT_PACKET_INLINE Scalar& operator[](
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex < SIZE);

        //#ifdef RTFACT_DEBUG
        //#   pragma message("SIMD type layout assumption!")
        //#endif
        //return containers[aIndex / 4][aIndex % 4];
        return reinterpret_cast<Scalar*>(this)[aIndex];
    }

    RTFACT_PACKET_INLINE const Container& operator()(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex < CONTAINER_COUNT);

        return containers[aIndex];
    }

    RTFACT_PACKET_INLINE Container& operator()(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex < CONTAINER_COUNT);

        return containers[aIndex];
    }

    #pragma endregion

    #pragma region Utility methods

    RTFACT_PACKET_INLINE const Scalar min() const
    {
        Data minData = Data::min(containers[0], containers[1]);

        for(uint i = 1; i < CONTAINER_COUNT - 1; ++i)
        {
            minData = Data::min(containers[i], containers[i + 1]);
        }

        return minData.min4();
    }

    RTFACT_PACKET_INLINE const Scalar max() const
    {
        Data minData = Data::max(containers[0], containers[1]);

        for(uint i = 1; i < CONTAINER_COUNT - 1; ++i)
        {
            minData = Data::max(containers[i], containers[i + 1]);
        }

        return minData.max4();
    }

    RTFACT_PACKET_INLINE const bool containsSameValue() const
    {
        typename Mask<SIZE>::Data mask(Mask<SIZE>::Data::C_T_T_T_T);

        RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(i, 1, SIZE)
        {
            mask &= (containers[0].data == containers[i].data);
        }

        return (mask.getI32Mask() == Mask<SIZE>::Data::C_T_T_T_T_I32_MASK &&
                containers[0].data[0] == containers[0].data[1] &&
                containers[0].data[1] == containers[0].data[2] &&
                containers[0].data[2] == containers[0].data[3]);
    }
    
    RTFACT_PACKET_INLINE const BitMask<SIZE> getSignBitMask() const
    {
        BitMask<SIZE> ret;
        ret.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.setContainer(i, containers[i].data.getI32Mask());
        }

        return ret;
    }

    RTFACT_PACKET_INLINE Scalar addHoriz() const
    {
        Scalar ret = static_cast<Scalar>(0);
        
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret += containers[i].addHoriz();
        }

        return ret;
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValues(
        tIterator aBegin,
        tIterator aEnd,
        const Scalar aInvalidValue = -1) const
    {
        tIterator begin = aBegin;

        for(uint i = 0; i < SIZE; ++i, ++begin)
        {
            *begin = operator[](i);
        }

        for(begin = aBegin; begin != aEnd; ++begin)
        {
            if(*begin != aInvalidValue)
            {
                for(tIterator tempBegin = begin + 1; tempBegin != aEnd; ++tempBegin)
                {
                    if(*tempBegin == *begin)
                    {
                        *tempBegin = aInvalidValue;
                    }
                }
            }
        }

        tIterator newEnd = aBegin;

        for(tIterator tempBegin = aBegin; tempBegin != aEnd; ++tempBegin)
        {
            if(*tempBegin != aInvalidValue)
            {
                *newEnd = *tempBegin;

                ++newEnd;
            }
        }

        return newEnd;
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValues(
        tIterator aBegin,
        tIterator aEnd,
        const Mask<SIZE>& aMask,
        const Scalar aInvalidValue = -1) const
    {
        tIterator begin = aBegin;

        // compact array, discarding inactive elements
        for(uint i = 0; i < SIZE; ++i)
        {
            if(aMask[i])
            {
                *begin = operator[](i);

                ++begin;
            }
        }

        tIterator end = begin;

        for(begin = aBegin; begin != end; ++begin)
        {
            if(*begin != aInvalidValue)
            {
                for(tIterator tempBegin = begin + 1; tempBegin != end; ++tempBegin)
                {
                    if(*tempBegin == *begin)
                    {
                        *tempBegin = aInvalidValue;
                    }
                }
            }
        }

        tIterator newEnd = aBegin;

        for(tIterator tempBegin = aBegin; tempBegin != end; ++tempBegin)
        {
            if(*tempBegin != aInvalidValue)
            {
                *newEnd = *tempBegin;

                ++newEnd;
            }
        }

        return newEnd;
    }

    template<
        class tIterator>
    RTFACT_PACKET_INLINE tIterator getUniqueValuesNew(
        tIterator aBegin,
        tIterator aEnd,
        const Mask<SIZE>& aMask,
        const Scalar aInvalidValue = -1) const
    {
        t_This tempThis = *this;
        tIterator end = aBegin;

        for(uint i = 0; i < SIZE; ++i)
        {
            *end = tempThis[i];

            if(aMask[i] && *end != aInvalidValue)
            {
                for(uint j = i + 1; j <SIZE; ++j)
                {
                    tempThis[j] = (tempThis[j] == *end) ?
                        aInvalidValue :
                        tempThis[j];
                }

                ++end;
            }
        }

        return end;
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Container& aPacket)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = aPacket;
        }

        return ret;
    }

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Data& aData)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i].data = aData;
        }

        return ret;
    }

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Scalar aValue)
    {
        return replicate(Container::replicate(aValue));
    }

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Packet<1, Scalar>& aPacket)
    {
        return replicate(Container::replicate(aPacket.data));
    }

    static RTFACT_PACKET_INLINE const t_This& replicate(
        const t_This& aPacket)
    {
        return aPacket;
    }

    #pragma endregion

    #pragma region Operators

    RTFACT__PACKET_GENERIC_ARITH_OP(+)
    RTFACT__PACKET_GENERIC_ARITH_OP(-)
    RTFACT__PACKET_GENERIC_ARITH_OP(*)
    RTFACT__PACKET_GENERIC_ARITH_OP(/)
    RTFACT__PACKET_GENERIC_ARITH_OP(|)
    RTFACT__PACKET_GENERIC_ARITH_OP(&)
    RTFACT__PACKET_GENERIC_ARITH_OP(^)

    RTFACT__PACKET_GENERIC_UNARY_ARITH_OP(-)
    RTFACT__PACKET_GENERIC_UNARY_ARITH_OP(+)

    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(+)
    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(-)
    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(*)
    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(/)
    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(|)
    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(&)
    RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR(^)

    RTFACT__PACKET_GENERIC_COMPARISON_OP(> )
    RTFACT__PACKET_GENERIC_COMPARISON_OP(>=)
    RTFACT__PACKET_GENERIC_COMPARISON_OP(< )
    RTFACT__PACKET_GENERIC_COMPARISON_OP(<=)
    RTFACT__PACKET_GENERIC_COMPARISON_OP(==)
    RTFACT__PACKET_GENERIC_COMPARISON_OP(!=)

    RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(> )
    RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(>=)
    RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(< )
    RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(<=)
    RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(==)
    RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR(!=)

    RTFACT__PACKET_GENERIC_ARITH_OP_VEC3F__DECLARE(*)
    RTFACT__PACKET_GENERIC_ARITH_OP_VEC3F__DECLARE(/)

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(*)
RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(/)
RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(+)
RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(-)
RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(|)
RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(&)
RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC(^)

RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(> )
RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(>=)
RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(< )
RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(<=)
RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(==)
RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC(!=)

RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(+)
RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(-)
RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(*)
RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(/)
RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(|)
RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(&)
RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE(^)

RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(+)
RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(-)
RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(*)
RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(/)
RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(|)
RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(&)
RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE(^)

RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(> )
RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(>=)
RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(< )
RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(<=)
RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(==)
RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE(!=)

RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(> )
RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(>=)
RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(< )
RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(<=)
RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(==)
RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE(!=)

RTFACT_PACKET_4_ARITH_OP_PACKET_GENERIC__DEFINE(+)
RTFACT_PACKET_4_ARITH_OP_PACKET_GENERIC__DEFINE(-)
RTFACT_PACKET_4_ARITH_OP_PACKET_GENERIC__DEFINE(*)
RTFACT_PACKET_4_ARITH_OP_PACKET_GENERIC__DEFINE(/)

RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(> )
RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(>=)
RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(< )
RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(<=)
RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(==)
RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE(!=)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__PACKET_GENERIC_ARITH_OP
#undef RTFACT__PACKET_GENERIC_ARITH_OP_SCALAR
#undef RTFACT__PACKET_GENERIC_UNARY_ARITH_OP
#undef RTFACT__PACKET_GENERIC_COMPARISON_OP
#undef RTFACT__PACKET_GENERIC_COMPARISON_OP_SCALAR
#undef RTFACT__SCALAR_ARITH_OP_PACKET_GENERIC
#undef RTFACT__SCALAR_COMPARISON_OP_PACKET_GENERIC

#undef RTFACT__PACKET_1_ARITH_OP_PACKET_GENERIC__DEFINE
#undef RTFACT_PACKET_1_COMPARISON_OP_PACKET_GENERIC__DEFINE

#undef RTFACT__PACKET_1_ARITH_OP_PACKET_4__DEFINE
#undef RTFACT_PACKET_1_COMPARISON_OP_PACKET_4__DEFINE

#undef RTFACT_PACKET_4_ARITH_OP_PACKET_GENERIC__DEFINE
#undef RTFACT_PACKET_4_COMPARISON_OP_PACKET_GENERIC_DEFINE

#undef RTFACT__PACKET_GENERIC_ARITH_OP_VEC3F__DECLARE

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__GENERALPACKET_HPP
