/* 
 *  Mask4.hpp
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
 
 #ifndef RTFACT__QUADPACKETMASK_HPP
#define RTFACT__QUADPACKETMASK_HPP

RTFACT_NAMESPACE_BEGIN

namespace Detail {

    extern const RTFACT_ALIGN_SIMD int4 int4Masks[16];

} // namespace Detail

#pragma region Operator overloading macros

#define RTFACT__MASK_4_BOOL_OP(aOperator)                                      \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const t_This& aMask) const                                             \
    {                                                                          \
        return t_This(data aOperator aMask.data);                              \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Mask<1>& aMask) const                                            \
    {                                                                          \
        return Mask<4>(data aOperator Data::replicate(aMask.data));            \
    }

#define RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL(aOperator)                   \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize> operator aOperator(                \
        const Mask<taSize>& aMask) const;

#define RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DEFINE(aOperator)                 \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_PACKET_INLINE const Mask<taSize>                                    \
    Mask<4>::operator aOperator(                                               \
        const Mask<taSize>& aMask) const                                       \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = data aOperator aMask(i).data;                        \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__MASK_4_BOOL_ASSIGN_OP(aOperator)                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const t_This& aMask)                                                   \
    {                                                                          \
        data aOperator##= aMask.data;                                          \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Mask<1>& aMask)                                                  \
    {                                                                          \
        data aOperator##= Data::replicate(aMask.data);                         \
        return *this;                                                          \
    }

#define RTFACT__MASK_4_BOOL_OP_SCALAR(aOperator)                               \
    RTFACT_PACKET_INLINE const t_This operator aOperator(                      \
        const Scalar aValue) const                                             \
    {                                                                          \
        return t_This(data aOperator Data::replicate(aValue));                 \
    }

#define RTFACT__MASK_4_BOOL_ASSIGN_OP_SCALAR(aOperator)                        \
    RTFACT_PACKET_INLINE t_This& operator aOperator##=(                        \
        const Scalar aValue)                                                   \
    {                                                                          \
        data aOperator##= Data::replicate(aValue);                             \
        return *this;                                                          \
    }

#define RTFACT__SCALAR_BOOL_OP_MASK_4(aOperator)                               \
    RTFACT_PACKET_INLINE const Mask<4> operator aOperator(                     \
        const Mask<4>::Scalar aValue,                                          \
        const Mask<4>& aMask)                                                  \
    {                                                                          \
        return Mask<4>(                                                        \
            Mask<4>::Data::replicate(aValue) aOperator aMask.data);            \
    }

#define RTFACT__PACKET_4_UNARY_OP(aOperator)                                   \
    RTFACT_PACKET_INLINE const t_This operator aOperator () const              \
    {                                                                          \
        return t_This(aOperator data);                                         \
    }

#pragma endregion

template<>
class Mask<4>
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    typedef Mask<4> t_This;

public:

    enum
    {
        SIZE = 4,
        SIZE_SQRT = 2,
        CONTAINER_COUNT = 1
    };

    typedef int    Scalar;
    typedef int4   Data;
    typedef t_This Container;

    Data data;

    #pragma region Constants

    RTFACT_MASK_DECLARE_CONSTS
    RTFACT_MASK_4_DECLARE_CONSTS

    static RTFACT_PACKET_INLINE const t_This C_FALSE()
    {
        return t_This(Data::C_0_0_0_0());
    }

    #pragma endregion

    #pragma region Constructors

    Mask()
    {}

    RTFACT_PACKET_INLINE explicit Mask(
        const Scalar aValue1,
        const Scalar aValue2,
        const Scalar aValue3,
        const Scalar aValue4
    ) :
        data(Data::getInstance(aValue1, aValue2, aValue3, aValue4))
    {}

    RTFACT_PACKET_INLINE explicit Mask(
        const Data& aData
    ) :
        data(aData)
    {}

    RTFACT_PACKET_INLINE explicit Mask(
        const uint a4Bits)
    {
        RTFACT_PACKET_ASSERT(a4Bits >> 4 == 0);

        data = Detail::int4Masks[a4Bits];
    }

    RTFACT_PACKET_INLINE explicit Mask(
        const BitMask<4>& aBitMask)
    {
        Mask(aBitMask.getContainer(0));
    }

    RTFACT_PACKET_INLINE explicit Mask(
        const ActiveMask<4>& aMask);

    #pragma endregion

    #pragma region Accessors

    RTFACT_PACKET_INLINE const Scalar operator[](
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

    RTFACT_PACKET_INLINE const BitMask<4> getBitMask() const
    {
        return BitMask<4>(data.getI32Mask());
    }

    RTFACT_PACKET_INLINE const uint getIntMask() const
    {
        return static_cast<uint>(data.getI32Mask());
    }

    RTFACT_PACKET_INLINE const bool isTrue() const
    {
        return (data.getI32Mask() == Data::C_T_T_T_T_I32_MASK);
    }

    RTFACT_PACKET_INLINE const bool isFalse() const
    {
        return (data.getI32Mask() == Data::C_0_0_0_0_I32_MASK);
    }

    RTFACT_PACKET_INLINE void setTrue()
    {
        data = Data::C_T_T_T_T;
    }

    RTFACT_PACKET_INLINE void setFalse()
    {
        data = Data::C_0_0_0_0();
    }

    template<
        uint ta0,
        uint ta1,
        uint ta2,
        uint ta3>
    RTFACT_PACKET_INLINE const t_This shuffle() const
    {
        return t_This(data.shuffle<ta0, ta1, ta2, ta3>());
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Data& aData)
    {
        return t_This(aData);
    }

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Scalar aValue)
    {
        return t_This(aValue ? Data::C_T_T_T_T : Data::C_0_0_0_0());
    }

    static RTFACT_PACKET_INLINE const t_This replicate(
        const Mask<1>& aMask)
    {
        return t_This(aMask ? Data::C_T_T_T_T : Data::C_0_0_0_0());
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
    RTFACT_PACKET_INLINE const Packet<4, tScalar> iif(
        const Packet<4, tScalar>& aPacket1,
        const Packet<4, tScalar>& aPacket2) const;

    RTFACT_PACKET_INLINE const Vec3f<4> iif(
        const Vec3f<4>& aVec1,
        const Vec3f<4>& aVec2) const;

    RTFACT_PACKET_INLINE const Vec2f<4> iif(
        const Vec2f<4>& aVec1,
        const Vec2f<4>& aVec2) const;

    #pragma endregion

    #pragma region Operators

    RTFACT__MASK_4_BOOL_OP        (|)
    RTFACT__MASK_4_BOOL_ASSIGN_OP (|)
    RTFACT__MASK_4_BOOL_OP        (&)
    RTFACT__MASK_4_BOOL_ASSIGN_OP (&)
    RTFACT__MASK_4_BOOL_OP        (^)
    RTFACT__MASK_4_BOOL_ASSIGN_OP (^)
    RTFACT__MASK_4_BOOL_OP        (==)
    RTFACT__MASK_4_BOOL_OP        (!=)

    RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL(|)
    RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL(&)
    RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL(^)
    RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL(==)
    RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL(!=)

    RTFACT__MASK_4_BOOL_OP_SCALAR        (|)
    RTFACT__MASK_4_BOOL_ASSIGN_OP_SCALAR (|)
    RTFACT__MASK_4_BOOL_OP_SCALAR        (&)
    RTFACT__MASK_4_BOOL_ASSIGN_OP_SCALAR (&)
    RTFACT__MASK_4_BOOL_OP_SCALAR        (^)
    RTFACT__MASK_4_BOOL_ASSIGN_OP_SCALAR (^)
    RTFACT__MASK_4_BOOL_OP_SCALAR        (==)
    RTFACT__MASK_4_BOOL_OP_SCALAR        (!=)

    RTFACT__PACKET_4_UNARY_OP(~)

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_BOOL_OP_MASK_4(|)
RTFACT__SCALAR_BOOL_OP_MASK_4(&)
RTFACT__SCALAR_BOOL_OP_MASK_4(^)
RTFACT__SCALAR_BOOL_OP_MASK_4(==)
RTFACT__SCALAR_BOOL_OP_MASK_4(!=)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__MASK_4_BOOL_OP
#undef RTFACT__MASK_4_BOOL_OP_MASK_GENERIC__DECL
#undef RTFACT__MASK_4_BOOL_ASSIGN_OP
#undef RTFACT__MASK_4_BOOL_OP_SCALAR
#undef RTFACT__MASK_4_BOOL_ASSIGN_OP_SCALAR
#undef RTFACT__SCALAR_BOOL_OP_MASK_4
#undef RTFACT__MASK_4_BOOL_ASSIGN_OP_SCALAR

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__QUADPACKETMASK_HPP
