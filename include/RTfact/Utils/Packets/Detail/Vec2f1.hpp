#ifndef RTFACT__VEC2F1_HPP
#define RTFACT__VEC2F1_HPP

#include <string>
#include <stdio.h>

RTFACT_NAMESPACE_BEGIN

template<>
class Vec2f<1>
{
    typedef Vec2f<1> t_This;

public:

    enum
    {
        SIZE = 1,
        SIZE_SQRT = 1,
        CONTAINER_COUNT = 1
    };

    typedef float    Data;
    typedef Vec2f<1> Container;

    float x;
    float y;

    #pragma region Constants

    //RTFACT_VEC2F_DECLARE_CONSTS

    static RTFACT_VEC2F_INLINE const Vec2f<1> C_0_0()
    {
        return Vec2f<1>(0.f, 0.f);
    }

    #pragma endregion

    #pragma region Constructors

    RTFACT_VEC2F_INLINE Vec2f()
    {}

    RTFACT_VEC2F_INLINE Vec2f(
        const float* aData
    ) :
        x(aData[0]),
        y(aData[1])
    {}

    RTFACT_VEC2F_INLINE Vec2f(
        const float aX,
        const float aY
    ) :
        x(aX),
        y(aY)
    {}

    #pragma endregion

    #pragma region Accessors

    RTFACT_VEC2F_INLINE float xAt(
        uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return x;
    }

    RTFACT_VEC2F_INLINE float& xAt(
        uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return x;
    }

    RTFACT_VEC2F_INLINE float yAt(
        uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return y;
    }

    RTFACT_VEC2F_INLINE float& yAt(
        uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return y;
    }

    RTFACT_VEC2F_INLINE const float& get(
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < 2);

        return *(&x + aIndex);
    }

    RTFACT_VEC2F_INLINE float& get(
        const uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < 2);

        return *(&x + aIndex);
    }

    RTFACT_VEC2F_INLINE void set(
        const uint aIndex,
        const Vec2f<1>& aVec)
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        x = aVec.x;
        y = aVec.y;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1>& operator[](
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator[](
        const uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_VEC2F_INLINE const Container& operator()(
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_VEC2F_INLINE Container& operator()(
        const uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex == 0);

        return *this;
    }

    #pragma endregion

    #pragma region Utility methods

    RTFACT_VEC2F_INLINE int maxDim() const
    {
        return static_cast<int>(y > x);
    }

    RTFACT_VEC2F_INLINE float length() const
    {
        return Math::sqrt(x * x + y * y);
    }

    RTFACT_VEC2F_INLINE float lengthSqr() const
    {
        return x * x + y * y;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> normalize() const
    {
        const float lenRecip = 1.f / length();

        return Vec2f<1>(x * lenRecip, y * lenRecip);
    }

    RTFACT_VEC2F_INLINE void setNormalized()
    {
        const float lenRecip = 1.f / length();

        x *= lenRecip;
        y *= lenRecip;
    }

    RTFACT_VEC2F_INLINE float average() const
    {
        return (x + y) * 0.33333334f;
    }

    RTFACT_VEC2F_INLINE float addVertical() const
    {
        return x + y;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1>& addHoriz() const
    {
        return *this;
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_VEC2F_INLINE const Vec2f<1> replicate(
        const float aX,
        const float aY)
    {
        return Vec2f<1>(aX, aY);
    }

    static RTFACT_VEC2F_INLINE const Vec2f<1> replicate(
        const Vec2f<1>& aVec)
    {
        return aVec;
    }

    #pragma endregion

    #pragma region Operators

    #pragma region Arithmetic operators

    RTFACT_VEC2F_INLINE const Vec2f<1> operator + (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<1>(x + aVec.x, y + aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator += (
        const Vec2f<1>& aVec)
    {
        x += aVec.x;
        y += aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> operator - (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<1>(x - aVec.x, y - aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator -= (
        const Vec2f<1>& aVec)
    {
        x -= aVec.x;
        y -= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> operator * (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<1>(x * aVec.x, y * aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator *= (
        const Vec2f<1>& aVec)
    {
        x *= aVec.x;
        y *= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> operator / (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<1>(x / aVec.x, y / aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator /= (
        const Vec2f<1>& aVec)
    {
        x /= aVec.x;
        y /= aVec.y;

        return *this;
    }

    #pragma endregion // Arithmetic operators

    #pragma region Unary operators

    RTFACT_VEC2F_INLINE const Vec2f<1> operator - () const
    {
        return Vec2f<1>(-x, -y);
    }

    #pragma endregion // Unary operators

    #pragma region Comparison operators

    RTFACT_VEC2F_INLINE bool operator > (
        const Vec2f<1>& aVec) const
    {
        return (x > aVec.x) & (y > aVec.y);
    }

    RTFACT_VEC2F_INLINE bool operator >= (
        const Vec2f<1>& aVec) const
    {
        return (x >= aVec.x) & (y > aVec.y);
    }

    RTFACT_VEC2F_INLINE bool operator < (
        const Vec2f<1>& aVec) const
    {
        return (x < aVec.x) & (y < aVec.y);
    }

    RTFACT_VEC2F_INLINE bool operator <= (
        const Vec2f<1>& aVec) const
    {
        return (x <= aVec.x) & (y <= aVec.y);
    }

    RTFACT_VEC2F_INLINE bool operator == (
        const Vec2f<1>& aVec) const
    {
        return (x == aVec.x) & (y == aVec.y);
    }

    RTFACT_VEC2F_INLINE bool operator != (
        const Vec2f<1>& aVec) const
    {
        return (x != aVec.x) | (y != aVec.y);
    }

    #pragma endregion // Comparison operators

    #pragma region Scalar arithmetic operators

    RTFACT_VEC2F_INLINE const Vec2f<1> operator * (
        const float aValue) const
    {
        return Vec2f<1>(x * aValue, y * aValue);
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator *= (
        const float aValue)
    {
        x *= aValue;
        y *= aValue;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> operator / (
        const float aValue) const
    {
        return Vec2f<1>(x / aValue, y / aValue);
    }

    RTFACT_VEC2F_INLINE Vec2f<1>& operator /= (
        const float aValue)
    {
        x /= aValue;
        y /= aValue;

        return *this;
    }

    #pragma endregion // Scalar arithmetic operators

    #pragma region FPacket4 arithmetic operator declarations

    const Vec2f<4> operator * (
        const Packet<4, float>& aPacket) const;

    const Vec2f<4> operator / (
        const Packet<4, float>& aPacket) const;

    #pragma endregion // FPacket4 arithmetic operator declarations

    #pragma region Vec2f4 arithmetic operator declarations

    const Vec2f<4> operator + (
        const Vec2f<4>& aVec) const;

    const Vec2f<4> operator - (
        const Vec2f<4>& aVec) const;

    const Vec2f<4> operator * (
        const Vec2f<4>& aVec) const;

    const Vec2f<4> operator / (
        const Vec2f<4>& aVec) const;

    #pragma endregion // Vec2f4 arithmetic operator declarations

    #pragma region Vec2f4 comparison operator declarations

    const Mask<4> operator > (
        const Vec2f<4>& aVec) const;

    const Mask<4> operator >= (
        const Vec2f<4>& aVec) const;

    const Mask<4> operator < (
        const Vec2f<4>& aVec) const;

    const Mask<4> operator <= (
        const Vec2f<4>& aVec) const;

    const Mask<4> operator == (
        const Vec2f<4>& aVec) const;

    const Mask<4> operator != (
        const Vec2f<4>& aVec) const;

    #pragma endregion // Vec2f4 comparison operator declarations

    #pragma endregion
};

#pragma region Scalar arithmetic operators

RTFACT_VEC2F_INLINE const Vec2f<1> operator * (
    const float aValue,
    const Vec2f<1>& aVec)
{
    return Vec2f<1>(aValue * aVec.x, aValue * aVec.y);
}

RTFACT_VEC2F_INLINE const Vec2f<1> operator / (
    const float aValue,
    const Vec2f<1>& aVec)
{
    return Vec2f<1>(aValue / aVec.x, aValue / aVec.y);
}

#pragma endregion

#pragma region Mask1::blend

RTFACT_PACKET_INLINE const Vec2f<1>& Mask<1>::iif(
    const Vec2f<1>& aVec1,
    const Vec2f<1>& aVec2) const
{
    return (data ? aVec1 : aVec2);
}

#pragma endregion // Mask1::blend

RTFACT_NAMESPACE_END

#endif // RTFACT__VEC2F1_HPP
