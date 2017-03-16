/* 
 *  MaskGeneric.hpp
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
 *  Created on: 2008-11-09 23:28:58 +0100
 *  Author(s): Iliyan Georgiev, Byelozyorov Sergiy
 */
 
 #ifndef RTFACT__GENERALPACKETMASK_HPP
#define RTFACT__GENERALPACKETMASK_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__MASK_GENERIC_BOOL_OP(aOperator)                                \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aMask) const                                             \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aMask.containers[i];   \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Mask<1>& aMask) const                                            \
    {                                                                          \
        t_This ret;                                                            \
        const Container tempMask = Container::replicate(aMask);                \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator tempMask;              \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Mask<4>& aMask) const                                            \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aMask;                 \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP(aOperator)                         \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aMask)                                                   \
    {                                                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= aMask.containers[i];                    \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Mask<1>& aMask)                                                  \
    {                                                                          \
        const Container tempMask = Container::replicate(aMask);                \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= tempMask;                               \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Mask<4>& aMask)                                                  \
    {                                                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= aMask;                                  \
        }                                                                      \
        return *this;                                                          \
    }

#define RTFACT__MASK_GENERIC_BOOL_OP_SCALAR(aOperator)                         \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        t_This ret;                                                            \
        const Container tempMask = Container::replicate(aValue);               \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator tempMask;              \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP_SCALAR(aOperator)                  \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        const Container tempMask = Container::replicate(aValue);               \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= tempMask;                               \
        }                                                                      \
        return *this;                                                          \
    }

#define RTFACT__SCALAR_BOOL_OP_MASK_GENERIC(aOperator)                         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize> operator aOperator(                \
        const typename Mask<taSize>::Scalar aValue,                            \
        const Mask<taSize>& aMask)                                             \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        const typename Mask<taSize>::Container tempMask =                      \
            Mask<taSize>::Container::replicate(aValue);                        \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret.containerAt(i) = tempMask aOperator aMask.containerAt(i);      \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__MASK_GENERIC_UNARY_OP(aOperator)                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator() const               \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = aOperator containers[i];                       \
        }                                                                      \
        return ret;                                                            \
    }

#pragma endregion

template<
    uint taSize>
class Mask
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    // Size can only be greater or equal than 16 and a multiple of 4
    RTFACT_PACKET_STATIC_ASSERT(( (taSize >= 16) && (taSize % 4 == 0) ));

    // Uncomment to restrict the SIZE to a power of 4 and greater than or equal to 16
    //RTFACT_PACKET_STATIC_ASSERT(( taSize >= 16 && ((taSize & (taSize - 1)) == 0) && (taSize & 0x55555555) ));

    typedef Mask<taSize> t_This;

public:

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef int     Scalar;
    typedef int4    Data;
    typedef Mask<4> Container;

    Container containers[CONTAINER_COUNT];

    #pragma region Constants

    RTFACT_MASK_DECLARE_CONSTS

    static RTFACT_PACKET_INLINE const t_This C_FALSE()
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::C_FALSE();
        }

        return ret;
    }

    #pragma endregion

    #pragma region Constructors

    RTFACT_PACKET_INLINE Mask()
    {}

    RTFACT_PACKET_INLINE explicit Mask(
        const BitMask<SIZE>& aMask)
    {
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i] = Container(aMask.getContainer(i));
        }
    }

    RTFACT_PACKET_INLINE explicit Mask(const ActiveMask<taSize>&);

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

    RTFACT_PACKET_INLINE const BitMask<SIZE> getBitMask() const
    {
        BitMask<SIZE> ret;
        ret.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.setContainer(i, containers[i].data.getI32Mask());
        }

        return ret;
    }

    RTFACT_PACKET_INLINE const bool isTrue() const
    {
        return getBitMask().isTrue();
    }

    RTFACT_PACKET_INLINE const bool isFalse() const
    {
        return getBitMask().isFalse();
    }

    RTFACT_PACKET_INLINE void setFalse()
    {
        containers[0].data = Data::C_0_0_0_0();

        RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(i, 1, SIZE)
        {
            containers[i].data = containers[0].data;
        }
    }

    RTFACT_PACKET_INLINE void setTrue()
    {
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].data = Data::C_T_T_T_T;
        }
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Container& aMask)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = aMask;
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
        const Mask<1>& aMask)
    {
        return replicate(Container::replicate(aMask.data));
    }

    static RTFACT_PACKET_INLINE const t_This& replicate(
        const t_This& aMask)
    {
        return aMask;
    }

    #pragma endregion

    #pragma region Ternary operator declarations

    template<
        class tScalar>
    RTFACT_PACKET_INLINE const Packet<taSize, tScalar> iif(
        const Packet<taSize, tScalar>& aPacket1,
        const Packet<taSize, tScalar>& aPacket2) const;

    RTFACT_PACKET_INLINE const Vec3f<taSize> iif(
        const Vec3f<taSize>& aVec1,
        const Vec3f<taSize>& aVec2) const;

    RTFACT_PACKET_INLINE const Vec2f<taSize> iif(
        const Vec2f<taSize>& aVec1,
        const Vec2f<taSize>& aVec2) const;

    #pragma endregion

    #pragma region Operators

    RTFACT__MASK_GENERIC_BOOL_OP        (|)
    RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP (|)
    RTFACT__MASK_GENERIC_BOOL_OP        (&)
    RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP (&)
    RTFACT__MASK_GENERIC_BOOL_OP        (^)
    RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP (^)
    RTFACT__MASK_GENERIC_BOOL_OP        (==)
    RTFACT__MASK_GENERIC_BOOL_OP        (!=)
    
    RTFACT__MASK_GENERIC_BOOL_OP_SCALAR        (|)
    RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP_SCALAR (|)
    RTFACT__MASK_GENERIC_BOOL_OP_SCALAR        (&)
    RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP_SCALAR (&)
    RTFACT__MASK_GENERIC_BOOL_OP_SCALAR        (^)
    RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP_SCALAR (^)
    RTFACT__MASK_GENERIC_BOOL_OP_SCALAR        (==)
    RTFACT__MASK_GENERIC_BOOL_OP_SCALAR        (!=)

    RTFACT__MASK_GENERIC_UNARY_OP(~)

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_BOOL_OP_MASK_GENERIC(|)
RTFACT__SCALAR_BOOL_OP_MASK_GENERIC(&)
RTFACT__SCALAR_BOOL_OP_MASK_GENERIC(^)
RTFACT__SCALAR_BOOL_OP_MASK_GENERIC(==)
RTFACT__SCALAR_BOOL_OP_MASK_GENERIC(!=)

RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE(|)
RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE(&)
RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE(^)
RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE(==)
RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE(!=)

RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE(|)
RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE(&)
RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE(^)
RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE(==)
RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE(!=)

RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE(|)
RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE(&)
RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE(^)
RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE(==)
RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE(!=)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__MASK_GENERIC_BOOL_OP
#undef RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP
#undef RTFACT__MASK_GENERIC_BOOL_OP_SCALAR
#undef RTFACT__MASK_GENERIC_BOOL_ASSIGN_OP_SCALAR
#undef RTFACT__SCALAR_BOOL_OP_MASK_GENERIC
#undef RTFACT__MASK_GENERIC_UNARY_OP

#undef RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE
#undef RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE
#undef RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__GENERALPACKETMASK_HPP
