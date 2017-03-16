/* 
 *  Vec3fGeneric.hpp
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
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__GENERALVEC3F_HPP
#define RTFACT__GENERALVEC3F_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__VEC3F_GENERIC_ARITH_OP(aOperator)                              \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const t_This& aVec) const                                              \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aVec.containers[i];    \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aVec;                  \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        return operator aOperator(Container::replicate(aVec));                 \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const t_This& aVec)                                                    \
    {                                                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= aVec.containers[i];                     \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const Vec3f<4>& aVec)                                                  \
    {                                                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            containers[i] aOperator##= aVec;                                   \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const Vec3f<1>& aVec)                                                  \
    {                                                                          \
        return operator aOperator##= (Container::replicate(aVec));             \
    }

#define RTFACT__VEC3F_GENERIC_UNARY_ARITH_OP(aOperator)                        \
    RTFACT_VEC3F_INLINE const t_This operator aOperator() const                \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = aOperator containers[i];                       \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__VEC3F_GENERIC_COMPARISON_OP(aOperator)                         \
    RTFACT_VEC3F_INLINE const Mask<SIZE> operator aOperator(                   \
        const t_This& aVec) const                                              \
    {                                                                          \
        Mask<SIZE> ret;                                                        \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aVec.containers[i];    \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const Mask<SIZE> operator aOperator(                   \
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        Mask<SIZE> ret;                                                        \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aVec;                  \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const Mask<SIZE> operator aOperator(                   \
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        return operator aOperator (Container::replicate(aVec));                \
    }

#define RTFACT__VEC3F_GENERIC_ARITH_OP_SCALAR(aOperator)                       \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const float aValue) const                                              \
    {                                                                          \
        t_This ret;                                                            \
        const typename Container::FloatPacket tempPacket =                     \
            Container::FloatPacket::replicate(aValue);                         \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator tempPacket;            \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const float aValue)                                                    \
    {                                                                          \
        t_This ret;                                                            \
        const typename Container::FloatPacket tempPacket =                     \
            Container::FloatPacket::replicate(aValue);                         \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] aOperator##= tempPacket;                         \
        }                                                                      \
        return *this;                                                          \
    }

#define RTFACT__VEC3F_GENERIC_ARITH_OP_PACKET(aOperator)                       \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const FloatPacket& aPacket) const                                      \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aPacket(i);            \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const Packet<4, float>& aPacket) const                                 \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] = containers[i] aOperator aPacket;               \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const Packet<1, float>& aPacket) const                                 \
    {                                                                          \
        return operator aOperator(Container::FloatPacket::replicate(aPacket)); \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const FloatPacket& aPacket)                                            \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] aOperator##= aPacket(i);                         \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const Packet<4, float>& aPacket)                                       \
    {                                                                          \
        t_This ret;                                                            \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                              \
        {                                                                      \
            ret.containers[i] aOperator##= aPacket;                            \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const Packet<1, float>& aPacket)                                       \
    {                                                                          \
        return operator aOperator##=                                           \
            (Container::FloatPacket::replicate(aPacket));                      \
    }

#define RTFACT__SCALAR_ARITH_OP_VEC3F_GENERIC(aOperator)                       \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> operator aOperator(                \
        const float aValue, const Vec3f<taSize>& aVec)                         \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        const typename Packet<taSize, float>::Container tempPacket =           \
            Packet<taSize, float>::Container::replicate(aValue);               \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i) = tempPacket aOperator aVec(i);                             \
        }                                                                      \
        return ret;                                                            \
    }

#pragma endregion

#pragma region Class definition

template<
    uint taSize>
class Vec3f
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    // Size can only be greater or equal than 16 and a multiple of 4
    RTFACT_VEC3F_STATIC_ASSERT(( (taSize >= 16) && (taSize % 4 == 0) ));

    // Uncomment to restrict the SIZE to a power of 4 and greater than or equal to 16
    //RTFACT_VEC3F_STATIC_ASSERT(( taSize >= 16 && ((taSize & (taSize - 1)) == 0) && (taSize & 0x55555555) ));

    typedef Vec3f<taSize> t_This;

public:

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef Packet<taSize, float> FloatPacket;
    typedef Packet<taSize, int>   IntPacket;
    typedef float4                Data;
    typedef Vec3f<4>              Container;

    Container containers[CONTAINER_COUNT];

    #pragma region Constants

    RTFACT_VEC3F_DECLARE_CONSTS

    static RTFACT_VEC3F_INLINE const t_This C_0_0_0()
    {
        t_This ret;

        ret.containers[0] = Container::C_0_0_0();

        for(uint i = 1; i < CONTAINER_COUNT; ++i)
        {
            ret.containers[i] = ret.containers[0];
        }

        return ret;
    }

    #pragma endregion

    #pragma region Constructors
    
    RTFACT_VEC3F_INLINE Vec3f()
    {}

    RTFACT_VEC3F_INLINE Vec3f(
        const FloatPacket& aX,
        const FloatPacket& aY,
        const FloatPacket& aZ)
    {
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].x = aX(i);
            containers[i].y = aY(i);
            containers[i].z = aZ(i);
        }
    }

    #pragma endregion

    #pragma region Accessors

    #define RTFACT__GEN_COMPONENT_ACCESSORS(aComponent, aComponentCapital, aOffset)\
        RTFACT_VEC3F_INLINE const FloatPacket get##aComponentCapital() const   \
        {                                                                      \
            FloatPacket ret;                                                   \
                                                                               \
            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                          \
            {                                                                  \
                ret(i).data = containers[i].aComponent.data;                   \
            }                                                                  \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE void set##aComponentCapital(                       \
            const FloatPacket& aPacket)                                        \
        {                                                                      \
            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)                          \
            {                                                                  \
                containers[i].aComponent.data = aPacket(i).data;               \
            }                                                                  \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE const float aComponent##At(                        \
            uint aIndex) const                                                 \
        {                                                                      \
            RTFACT_VEC3F_ASSERT(aIndex < SIZE);                                \
                                                                               \
            /*(aIndex >> 2) * 12 + (aIndex % 4) + aOffset*/                    \
            return containers[aIndex / 4].aComponent##At(aIndex % 4);          \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE float& aComponent##At(                             \
            uint aIndex)                                                       \
        {                                                                      \
            RTFACT_VEC3F_ASSERT(aIndex < SIZE);                                \
                                                                               \
            return containers[aIndex / 4].aComponent##At(aIndex % 4);          \
        }

    RTFACT__GEN_COMPONENT_ACCESSORS(x, X, 0)
    RTFACT__GEN_COMPONENT_ACCESSORS(y, Y, 4)
    RTFACT__GEN_COMPONENT_ACCESSORS(z, Z, 8)

    #undef RTFACT__GEN_COMPONENT_ACCESSORS

    RTFACT_VEC3F_INLINE const float& get(
        const uint aDimension,
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aDimension < 3);
        RTFACT_VEC3F_ASSERT(aIndex < SIZE);

        return containers[aIndex / 4].get(aDimension, aIndex % 4);
    }

    RTFACT_VEC3F_INLINE float& get(
        const uint aDimension,
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aDimension < 3);
        RTFACT_VEC3F_ASSERT(aIndex < SIZE);

        return containers[aIndex / 4].get(aDimension, aIndex % 4);
    }

    RTFACT_VEC3F_INLINE const FloatPacket get(
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex < 3);

        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret(i) = containers[i].get(aIndex);
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE void set(
        const uint aIndex,
        const FloatPacket& aPacket)
    {
        RTFACT_VEC3F_ASSERT(aIndex < 3);

        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].get(aIndex) = aPacket(i);
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const Vec3f<1> operator[](
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex < SIZE);

        return Vec3f<1>(xAt(aIndex), yAt(aIndex), zAt(aIndex));
    }

    RTFACT_VEC3F_INLINE const Container& operator()(
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex < CONTAINER_COUNT);

        return containers[aIndex];
    }

    RTFACT_VEC3F_INLINE Container& operator()(
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aIndex < CONTAINER_COUNT);

        return containers[aIndex];
    }

    #pragma endregion
    
    #pragma region Utility methods

    #pragma region Min/max

    RTFACT_VEC3F_INLINE const IntPacket maxDim() const
    {
        IntPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret(i) = (containers[i].x > containers[i].y).iif(
                (containers[i].x > containers[i].z).iif(IntPacket::C_0(),
                                                        IntPacket::C_2),
                (containers[i].y > containers[i].z).iif(IntPacket::C_1,
                                                        IntPacket::C_2));
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket min() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret(i) = containers[i].min();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket max() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret(i) = containers[i].max();
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::min(aVec1.containers[i],
                                               aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        Container tempVec = Container::replicate(aVec1);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::min(tempVec, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const Vec3f<4>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::min(aVec1, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return min(aVec2, aVec1);
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const t_This& aVec1,
        const Vec3f<4>& aVec2)
    {
        return min(aVec2, aVec1);
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::max(aVec1.containers[i],
                                               aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        Container tempVec = Container::replicate(aVec1);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::max(tempVec, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const Vec3f<4>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::max(aVec1, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return max(aVec2, aVec1);
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const t_This& aVec1,
        const Vec3f<4>& aVec2)
    {
        return max(aVec2, aVec1);
    }

    RTFACT_VEC3F_INLINE void setMin(
        const t_This& aVec)
    { 
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setMin(aVec.containers[i]);
        }
    }

    RTFACT_VEC3F_INLINE void setMin(
        const Vec3f<1>& aVec)
    {
        Container tempVec = Container::replicate(aVec);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setMin(tempVec);
        }
    }

    RTFACT_VEC3F_INLINE void setMin(
        const Vec3f<4>& aVec)
    { 
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setMin(aVec);
        }
    }

    RTFACT_VEC3F_INLINE void setMax(
        const t_This& aVec)
    { 
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setMax(aVec.containers[i]);
        }
    }

    RTFACT_VEC3F_INLINE void setMax(
        const Vec3f<1>& aVec)
    {
        Container tempVec = Container::replicate(aVec);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setMax(tempVec);
        }
    }

    RTFACT_VEC3F_INLINE void setMax(
        const Vec3f<4>& aVec)
    { 
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setMax(aVec);
        }
    }

    #pragma endregion

    #pragma region Miscellaneous

    RTFACT_VEC3F_INLINE const FloatPacket length() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].length();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket lengthSqr() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].lengthSqr();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const t_This normalize() const
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].normalize();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE void setNormalized()
    {
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].setNormalized();
        }
    }

    RTFACT_VEC3F_INLINE const FloatPacket average() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].average();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket luminance() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].luminance();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const t_This reciprocal() const
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].reciprocal();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket addVertical() const
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = containers[i].addVertical();
        }

        return ret;
    }

    RTFACT_VEC3F_INLINE const Vec3f<1> addHoriz() const
    {
        Vec3f<1> ret = containers[0].addHoriz();

        RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(i, 1, SIZE)
        {
            ret += containers[i].addHoriz();
        }

        return ret;
    }

    #pragma endregion

    #pragma region Dot product

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::dot(aVec1.containers[i],
                                               aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        FloatPacket ret;

        Container tempVec = Container::replicate(aVec1);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::dot(tempVec, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const Vec3f<4>& aVec1,
        const t_This& aVec2)
    {
        FloatPacket ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::dot(aVec1, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return dot(aVec2, aVec1);
    }

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const t_This& aVec1,
        const Vec3f<4>& aVec2)
    {
        return dot(aVec2, aVec1);
    }

    #pragma endregion

    #pragma region Product

    static RTFACT_VEC3F_INLINE t_This product(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::product(aVec1.containers[i],
                                                   aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        Container tempVec = Container::replicate(aVec1);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::product(tempVec,
                                                   aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const Vec3f<4>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::product(aVec1, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return product(aVec2, aVec1);
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const t_This& aVec1,
        const Vec3f<4>& aVec2)
    {
        return product(aVec2, aVec1);
    }

    #pragma endregion

    #pragma region Cross product

    static RTFACT_VEC3F_INLINE t_This cross(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::cross(aVec1.containers[i],
                                                 aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        Container tempVec = Container::replicate(aVec1);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::cross(tempVec, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const Vec3f<4>& aVec1,
        const t_This& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::cross(aVec1, aVec2.containers[i]);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        t_This ret;

        Container tempVec = Container::replicate(aVec2);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::cross(aVec1.containers[i], tempVec);
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const t_This& aVec1,
        const Vec3f<4>& aVec2)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = Container::cross(aVec1.containers[i], aVec2);
        }

        return ret;
    }

    #pragma endregion

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_VEC3F_INLINE const t_This replicate(
        const float aX,
        const float aY,
        const float aZ)
    {
        return replicate(Container::replicate(aX, aY, aZ));
    }

    static RTFACT_VEC3F_INLINE const t_This replicate(
        const Vec3f<1>& aVec)
    {
        return replicate(Container::replicate(aVec));
    }

    static RTFACT_VEC3F_INLINE const t_This replicate(
        const Vec3f<4>& aVec)
    {
        t_This ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            ret.containers[i] = aVec;
        }

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This& replicate(
        const t_This& aVec)
    {
        return aVec;
    }

    #pragma endregion

    #pragma region Operators

    RTFACT__VEC3F_GENERIC_ARITH_OP(+)
    RTFACT__VEC3F_GENERIC_ARITH_OP(-)
    RTFACT__VEC3F_GENERIC_ARITH_OP(*)
    RTFACT__VEC3F_GENERIC_ARITH_OP(/)

    RTFACT__VEC3F_GENERIC_UNARY_ARITH_OP(-)

    RTFACT__VEC3F_GENERIC_COMPARISON_OP(>)
    RTFACT__VEC3F_GENERIC_COMPARISON_OP(>=)
    RTFACT__VEC3F_GENERIC_COMPARISON_OP(<)
    RTFACT__VEC3F_GENERIC_COMPARISON_OP(<=)
    RTFACT__VEC3F_GENERIC_COMPARISON_OP(==)
    RTFACT__VEC3F_GENERIC_COMPARISON_OP(!=)

    RTFACT__VEC3F_GENERIC_ARITH_OP_SCALAR(*)
    RTFACT__VEC3F_GENERIC_ARITH_OP_SCALAR(/)

    RTFACT__VEC3F_GENERIC_ARITH_OP_PACKET(*)
    RTFACT__VEC3F_GENERIC_ARITH_OP_PACKET(/)

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_ARITH_OP_VEC3F_GENERIC(*)
RTFACT__SCALAR_ARITH_OP_VEC3F_GENERIC(/)

RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC__DEFINE(+)
RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC__DEFINE(-)
RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC__DEFINE(*)
RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC__DEFINE(/)

RTFACT__VEC3F_1_ARITH_OP_VEC3F_4__DEFINE(+)
RTFACT__VEC3F_1_ARITH_OP_VEC3F_4__DEFINE(-)
RTFACT__VEC3F_1_ARITH_OP_VEC3F_4__DEFINE(*)
RTFACT__VEC3F_1_ARITH_OP_VEC3F_4__DEFINE(/)

RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(>)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(>=)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(<)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(<=)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(==)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(!=)

RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(>)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(>=)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(<)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(<=)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(==)
RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(!=)

RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DEFINE(*)
RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DEFINE(/)

RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DEFINE(+)
RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DEFINE(-)
RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DEFINE(*)
RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DEFINE(/)

RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(>)
RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(>=)
RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(<)
RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(<=)
RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(==)
RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(!=)

RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DEFINE(*)
RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DEFINE(/)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__VEC3F_GENERIC_ARITH_OP
#undef RTFACT__VEC3F_GENERIC_UNARY_ARITH_OP
#undef RTFACT__VEC3F_GENERIC_COMPARISON_OP
#undef RTFACT__VEC3F_GENERIC_ARITH_OP_PACKET
#undef RTFACT__VEC3F_GENERIC_ARITH_OP_SCALAR
#undef RTFACT__SCALAR_ARITH_OP_VEC3F_GENERIC

#undef RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC__DEFINE
#undef RTFACT__VEC3F_1_ARITH_OP_VEC3F_4__DEFINE

#undef RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE
#undef RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE

#undef RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DEFINE
#undef RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__GENERALVEC3F_HPP
