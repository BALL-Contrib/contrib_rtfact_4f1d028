/* 
 *  Mask1.hpp
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
 *  Created on: 2008-11-09 23:28:58 +0100
 *  Author(s): Iliyan Georgiev, Georgiev Iliyan
 */
 
 #ifndef RTFACT__SINGLEPACKETMASK_HPP
#define RTFACT__SINGLEPACKETMASK_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__MASK_1_BOOL_OP(aOperator)                                      \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aMask) const                                             \
    {                                                                          \
        return t_This(data aOperator aMask.data);                              \
    }

#define RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE(aOperator)              \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize> operator aOperator(                \
        const Mask<taSize>& aMask) const;

#define RTFACT__MASK_1_BOOL_OP_MASK_GENERIC__DEFINE(aOperator)                 \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize>                                    \
    Mask<1>::operator aOperator(                                               \
        const Mask<taSize>& aMask) const                                       \
    {                                                                          \
        typedef typename Mask<taSize>::Data t_Data;                            \
        Mask<taSize> ret;                                                      \
        const t_Data tempData = data ? t_Data::C_1_1_1_1 : t_Data::C_0_0_0_0();\
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = tempData aOperator aMask(i).data;                    \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__MASK_1_BOOL_OP_MASK_4__DEFINE(aOperator)                       \
    template<>                                                                 \
    RTFACT_PACKET_INLINE const Mask<4>                                         \
    Mask<1>::operator aOperator<4>(                                            \
        const Mask<4>& aMask) const                                            \
    {                                                                          \
        typedef Mask<4>::Data t_Data;                                          \
        return Mask<4>(                                                        \
            (data ? t_Data::C_1_1_1_1 : t_Data::C_0_0_0_0()) aOperator         \
            aMask.data);                                                       \
    }

#define RTFACT__MASK_1_BOOL_ASSIGN_OP(aOperator)                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aMask)                                                   \
    {                                                                          \
        data aOperator##= aMask.data;                                          \
        return *this;                                                          \
    }

#define RTFACT__MASK_1_BOOL_OP_SCALAR(aOperator)                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        return t_This(data aOperator aValue);                                  \
    }

#define RTFACT__MASK_1_BOOL_ASSIGN_OP_SCALAR(aOperator)                        \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        data aOperator##= aValue;                                              \
        return *this;                                                          \
    }

#define RTFACT__SCALAR_BOOL_OP_MASK_1(aOperator)                               \
    RTFACT_PACKET_INLINE const Mask<1> operator aOperator(                     \
        const Mask<1>::Scalar aValue,                                          \
        const Mask<1> aMask)                                                   \
    {                                                                          \
        return Mask<1>(aValue aOperator aMask.data);                           \
    }

#pragma endregion

template<>
class Mask<1>
{
    typedef Mask<1> t_This;

public:

    enum
    {
        SIZE = 1,
        SIZE_SQRT = 1,
        CONTAINER_COUNT = 1
    };

    typedef int    Scalar;
    typedef Scalar Data;
    typedef t_This Container;

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

    RTFACT_MASK_DECLARE_CONSTS

    static RTFACT_PACKET_INLINE const t_This C_FALSE()
    {
        return static_cast<Scalar>(0);
    }

    #pragma endregion

    #pragma region Constructors

    Mask()
    {}

    RTFACT_PACKET_INLINE Mask(
        const Scalar aData
    ) :
        data(aData)
    {}

    RTFACT_PACKET_INLINE explicit Mask(
        const BitMask<1>& aMask
    ) :
        data(aMask.getContainer(0))
    {}

    RTFACT_PACKET_INLINE explicit Mask(
        const ActiveMask<1>& aMask);

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

    RTFACT_PACKET_INLINE const BitMask<1> getBitMask() const
    {
        return BitMask<1>(data);
    }

    RTFACT_PACKET_INLINE const uint getIntMask() const
    {
        return static_cast<uint>(data);
    }

    RTFACT_PACKET_INLINE bool isTrue() const
    {
        return data != 0;
    }

    RTFACT_PACKET_INLINE bool isFalse() const
    {
        return data == 0;
    }

    RTFACT_PACKET_INLINE void setTrue()
    {
        data = 0xFFFFFFFF;
    }

    RTFACT_PACKET_INLINE void setFalse()
    {
        data = 0;
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Scalar aValue)
    {
        return t_This(aValue);
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
    RTFACT_PACKET_INLINE const Packet<1, tScalar>& iif(
        const Packet<1, tScalar>& aPacket1,
        const Packet<1, tScalar>& aPacket2) const;

    RTFACT_PACKET_INLINE const Vec3f<1>& iif(
        const Vec3f<1>& aVec1,
        const Vec3f<1>& aVec2) const;

    RTFACT_PACKET_INLINE const Vec2f<1>& iif(
        const Vec2f<1>& aVec1,
        const Vec2f<1>& aVec2) const;

    #pragma endregion

    #pragma region Operators

    RTFACT__MASK_1_BOOL_OP        (|)
    RTFACT__MASK_1_BOOL_ASSIGN_OP (|)
    RTFACT__MASK_1_BOOL_OP        (&)
    RTFACT__MASK_1_BOOL_ASSIGN_OP (&)
    RTFACT__MASK_1_BOOL_OP        (^)
    RTFACT__MASK_1_BOOL_ASSIGN_OP (^)
    RTFACT__MASK_1_BOOL_OP        (==)
    RTFACT__MASK_1_BOOL_OP        (!=)

    RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE(|)
    RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE(&)
    RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE(^)
    RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE(==)
    RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE(!=)

    RTFACT__MASK_1_BOOL_OP_SCALAR        (|)
    RTFACT__MASK_1_BOOL_ASSIGN_OP_SCALAR (|)
    RTFACT__MASK_1_BOOL_OP_SCALAR        (&)
    RTFACT__MASK_1_BOOL_ASSIGN_OP_SCALAR (&)
    RTFACT__MASK_1_BOOL_OP_SCALAR        (^)
    RTFACT__MASK_1_BOOL_ASSIGN_OP_SCALAR (^)
    RTFACT__MASK_1_BOOL_OP_SCALAR        (==)
    RTFACT__MASK_1_BOOL_OP_SCALAR        (!=)

    RTFACT_PACKET_INLINE const t_This operator~() const
    {
        return t_This(!data);
    }

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_BOOL_OP_MASK_1(|)
RTFACT__SCALAR_BOOL_OP_MASK_1(&)
RTFACT__SCALAR_BOOL_OP_MASK_1(^)
RTFACT__SCALAR_BOOL_OP_MASK_1(==)
RTFACT__SCALAR_BOOL_OP_MASK_1(!=)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__MASK_1_BOOL_OP
#undef RTFACT__MASK_1_BOOL_OP_GENERAL_MASK_4__DECLARE
#undef RTFACT__MASK_1_BOOL_ASSIGN_OP
#undef RTFACT__MASK_1_BOOL_OP_SCALAR
#undef RTFACT__MASK_1_BOOL_ASSIGN_OP_SCALAR
#undef RTFACT__SCALAR_BOOL_OP_MASK_1

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__SINGLEPACKETMASK_HPP
