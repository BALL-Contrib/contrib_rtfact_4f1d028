#ifndef RTFACT__VEC2F4_HPP
#define RTFACT__VEC2F4_HPP

RTFACT_NAMESPACE_BEGIN

template<>
class Vec2f<4>
{
    typedef Vec2f<4> t_This;

public:

    enum
    {
        SIZE = 4,
        SIZE_SQRT = 2,
        CONTAINER_COUNT = 1
    };

    typedef Packet<4, float> FloatPacket;
    typedef Packet<4, int>   IntPacket;
    typedef float4           Data;
    typedef Vec2f<4>         Container;

    FloatPacket x;
    FloatPacket y;

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    #pragma region Constants

    //RTFACT_VEC2F_DECLARE_CONSTS

    static RTFACT_VEC2F_INLINE const Vec2f<4> C_0_0()
    {
        Vec2f<4> ret;

        ret.x = FloatPacket::C_0();
        ret.y = ret.x;

        return ret;
    }

    #pragma endregion

    #pragma region Constructors

    RTFACT_VEC2F_INLINE Vec2f()
    {}

    RTFACT_VEC2F_INLINE Vec2f(
        const FloatPacket& aX,
        const FloatPacket& aY
    ) :
        x(aX),
        y(aY)
    {}

private:

    RTFACT_VEC2F_INLINE Vec2f(
        const FloatPacket::Data& aX,
        const FloatPacket::Data& aY
    ) :
        x(aX),
        y(aY)
    {}

public:

    #pragma endregion

    #pragma region Accessors

    RTFACT_VEC2F_INLINE float xAt(
        uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return x[aIndex];
    }

    RTFACT_VEC2F_INLINE float& xAt(
        uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return x[aIndex];
    }

    RTFACT_VEC2F_INLINE float yAt(
        uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return y[aIndex];
    }

    RTFACT_VEC2F_INLINE float& yAt(
        uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return y[aIndex];
    }

    RTFACT_VEC2F_INLINE const FloatPacket& get(
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < 2);

        return *(&x + aIndex);
    }

    RTFACT_VEC2F_INLINE FloatPacket& get(
        const uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < 2);

        return *(&x + aIndex);
    }

    RTFACT_VEC2F_INLINE void set(
        const uint aIndex,
        const Vec2f<1>& aVec)
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        x[aIndex] = aVec.x;
        y[aIndex] = aVec.y;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> operator[](
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < 4);

        return Vec2f<1>(x[aIndex], y[aIndex]);
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

    RTFACT_VEC2F_INLINE const IntPacket maxDim() const
    {
        return IntPacket((x > y).data);
    }

    RTFACT_VEC2F_INLINE const FloatPacket length() const
    {
        return Math::sqrt(x * x + y * y);
    }

    RTFACT_VEC2F_INLINE const FloatPacket lengthSqr() const
    {
        return x * x + y * y;
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> normalize() const
    {
        FloatPacket norm = Math::recipSqrt(x * x + y * y);

        return Vec2f<4>(x * norm, y * norm);
    }

    RTFACT_VEC2F_INLINE void setNormalized()
    {
        FloatPacket norm = Math::recipSqrt(x * x + y * y);

        x *= norm;
        y *= norm;
    }

    RTFACT_VEC2F_INLINE const FloatPacket average() const
    {
        return (x + y) * FloatPacket::C_ONE_THIRD;
    }

    RTFACT_VEC2F_INLINE const FloatPacket addVertical() const
    {
        return x + y;
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> addHoriz() const
    {
        return Vec2f<1>(x.addHoriz(), y.addHoriz());
    }

    template<
        uint ta0,
        uint ta1,
        uint ta2,
        uint ta3>
    RTFACT_VEC2F_INLINE const Vec2f<4> shuffle() const
    {
        return Vec2f<4>(
            x.shuffle<ta0, ta1, ta2, ta3>(),
            y.shuffle<ta0, ta1, ta2, ta3>());
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_VEC2F_INLINE const Vec2f<4> replicate(
        const float aX,
        const float aY)
    {
        return Vec2f<4>(
            float4::replicate(aX),
            float4::replicate(aY));
    }

    static RTFACT_VEC2F_INLINE const Vec2f<4> replicate(
        const Vec2f<1>& aVec)
    {
        return Vec2f<4>(
            float4::replicate(aVec.x),
            float4::replicate(aVec.y));
    }

    #pragma endregion

    #pragma region Operators

    #pragma region Arithmetic operators

    RTFACT_VEC2F_INLINE const Vec2f<4> operator + (
        const Vec2f<4>& aVec) const
    {
        return Vec2f<4>(x + aVec.x, y + aVec.y);
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator + (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<4>(x + aVec.x, y + aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator += (
        const Vec2f<4>& aVec)
    {
        x += aVec.x;
        y += aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator += (
        const Vec2f<1>& aVec)
    {
        x += aVec.x;
        y += aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator - (
        const Vec2f<4>& aVec) const
    {
        return Vec2f<4>(x - aVec.x, y - aVec.y);
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator - (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<4>(x - aVec.x, y - aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator -= (
        const Vec2f<4>& aVec)
    {
        x -= aVec.x;
        y -= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator -= (
        const Vec2f<1>& aVec)
    {
        x -= aVec.x;
        y -= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator * (
        const Vec2f<4>& aVec) const
    {
        return Vec2f<4>(x * aVec.x, y * aVec.y);
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator * (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<4>(x * aVec.x, y * aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator *= (
        const Vec2f<4>& aVec)
    {
        x *= aVec.x;
        y *= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator *= (
        const Vec2f<1>& aVec)
    {
        x *= aVec.x;
        y *= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator / (
        const Vec2f<4>& aVec) const
    {
        return Vec2f<4>(x / aVec.x, y / aVec.y);
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator / (
        const Vec2f<1>& aVec) const
    {
        return Vec2f<4>(x / aVec.x, y / aVec.y);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator /= (
        const Vec2f<4>& aVec)
    {
        x /= aVec.x;
        y /= aVec.y;

        return *this;
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator /= (
        const Vec2f<1>& aVec)
    {
        x /= aVec.x;
        y /= aVec.y;

        return *this;
    }

    #pragma endregion // Arithmetic operators

    #pragma region Unary operators

    RTFACT_VEC2F_INLINE const Vec2f<4> operator - () const
    {
        return Vec2f<4>(-x, -y);
    }

    #pragma endregion // Unary operators

    #pragma region Comparison operators

    RTFACT_VEC2F_INLINE const Mask<4> operator > (
        const Vec2f<4>& aVec) const
    {
        return (x > aVec.x) & (y > aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator > (
        const Vec2f<1>& aVec) const
    {
        return (x > aVec.x) & (y > aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator >= (
        const Vec2f<4>& aVec) const
    {
        return (x >= aVec.x) & (y >= aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator >= (
        const Vec2f<1>& aVec) const
    {
        return (x >= aVec.x) & (y >= aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator < (
        const Vec2f<4>& aVec) const
    {
        return (x < aVec.x) & (y < aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator < (
        const Vec2f<1>& aVec) const
    {
        return (x < aVec.x) & (y < aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator <= (
        const Vec2f<4>& aVec) const
    {
        return (x <= aVec.x) & (y <= aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator <= (
        const Vec2f<1>& aVec) const
    {
        return (x <= aVec.x) & (y <= aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator == (
        const Vec2f<4>& aVec) const
    {
        return (x == aVec.x) & (y == aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator == (
        const Vec2f<1>& aVec) const
    {
        return (x == aVec.x) & (y == aVec.y);
    }

    RTFACT_VEC2F_INLINE const Mask<4> operator != (
        const Vec2f<4>& aVec) const
    {
        return (x != aVec.x) | (y != aVec.y);
    }

    #pragma endregion // Comparison operators

    #pragma region Scalar arithmetic operators

    RTFACT_VEC2F_INLINE const Vec2f<4> operator * (
        const float aValue) const
    {
        const FloatPacket tempPacket = FloatPacket::replicate(aValue);

        return Vec2f<4>(x * tempPacket, y * tempPacket);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator *= (
        const float aValue)
    {
        const FloatPacket tempPacket = FloatPacket::replicate(aValue);

        x *= tempPacket;
        y *= tempPacket;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator / (
        const float aValue) const
    {
        const FloatPacket tempPacket = FloatPacket::replicate(aValue);

        return Vec2f<4>(x / tempPacket, y / tempPacket);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator /= (
        const float aValue)
    {
        const FloatPacket tempPacket = FloatPacket::replicate(aValue);

        x /= tempPacket;
        y /= tempPacket;

        return *this;
    }

    #pragma endregion // Scalar arithmetic operators

    #pragma region Packet arithmetic operators

    RTFACT_VEC2F_INLINE const Vec2f<4> operator * (
        const FloatPacket& aPacket) const
    {
        return Vec2f<4>(x * aPacket, y * aPacket);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator *= (
        const FloatPacket& aPacket)
    {
        x *= aPacket;
        y *= aPacket;

        return *this;
    }

    RTFACT_VEC2F_INLINE const Vec2f<4> operator / (
        const FloatPacket& aPacket) const
    {
        return Vec2f<4>(x / aPacket, y / aPacket);
    }

    RTFACT_VEC2F_INLINE Vec2f<4>& operator /= (
        const FloatPacket& aPacket)
    {
        x /= aPacket;
        y /= aPacket;

        return *this;
    }

    #pragma endregion // Packet arithmetic operators

    #pragma endregion
};

#pragma region Operator definitions

#pragma region Scalar arithmetic operators

RTFACT_VEC2F_INLINE const Vec2f<4> operator * (
    const float aValue,
    const Vec2f<4>& aVec)
{
    const Vec2f<4>::FloatPacket tempPacket =
        Vec2f<4>::FloatPacket::replicate(aValue);
    return Vec2f<4>(
        tempPacket * aVec.x,
        tempPacket * aVec.y);
}

RTFACT_VEC2F_INLINE const Vec2f<4> operator / (
    const float aValue,
    const Vec2f<4>& aVec)
{
    const Vec2f<4>::FloatPacket tempPacket =
        Vec2f<4>::FloatPacket::replicate(aValue);
    return Vec2f<4>(
        tempPacket / aVec.x,
        tempPacket / aVec.y);
}

#pragma endregion // Scalar arithmetic operators

#pragma region Vec2f1 packet arithmetic operators

RTFACT_VEC2F_INLINE const Vec2f<4> Vec2f<1>::operator * (
    const Packet<4, float>& aPacket) const
{
    Vec2f<4> ret;

    ret.x = x * aPacket;
    ret.y = y * aPacket;

    return ret;
}

RTFACT_VEC2F_INLINE const Vec2f<4> Vec2f<1>::operator / (
    const Packet<4, float>& aPacket) const
{
    Vec2f<4> ret;

    ret.x = x / aPacket;
    ret.y = y / aPacket;

    return ret;
}

#pragma endregion // Vec2f1 packet arithmetic operators

#pragma region Vec2f1 arithmetic operators

RTFACT_VEC2F_INLINE const Vec2f<4> Vec2f<1>::operator + (
    const Vec2f<4>& aVec) const
{
    Vec2f<4> ret;

    ret.x = x + aVec.x;
    ret.y = y + aVec.y;

    return ret;
}

RTFACT_VEC2F_INLINE const Vec2f<4> Vec2f<1>::operator - (
    const Vec2f<4>& aVec) const
{
    Vec2f<4> ret;

    ret.x = x - aVec.x;
    ret.y = y - aVec.y;

    return ret;
}

RTFACT_VEC2F_INLINE const Vec2f<4> Vec2f<1>::operator * (
    const Vec2f<4>& aVec) const
{
    Vec2f<4> ret;

    ret.x = x * aVec.x;
    ret.y = y * aVec.y;

    return ret;
}

RTFACT_VEC2F_INLINE const Vec2f<4> Vec2f<1>::operator / (
    const Vec2f<4>& aVec) const
{
    Vec2f<4> ret;

    ret.x = x / aVec.x;
    ret.y = y / aVec.y;

    return ret;
}

#pragma endregion // Vec2f1 arithmetic operators

#pragma region Vec2f1 comparison operators

RTFACT_VEC2F_INLINE const Mask<4> Vec2f<1>::operator > (
    const Vec2f<4>& aVec) const
{
    return (x > aVec.x) & (y >  aVec.y);
}

RTFACT_VEC2F_INLINE const Mask<4> Vec2f<1>::operator >= (
    const Vec2f<4>& aVec) const
{
    return (x >= aVec.x) & (y >=  aVec.y);
}

RTFACT_VEC2F_INLINE const Mask<4> Vec2f<1>::operator < (
    const Vec2f<4>& aVec) const
{
    return (x < aVec.x) & (y <  aVec.y);
}

RTFACT_VEC2F_INLINE const Mask<4> Vec2f<1>::operator <= (
    const Vec2f<4>& aVec) const
{
    return (x <= aVec.x) & (y <=  aVec.y);
}

RTFACT_VEC2F_INLINE const Mask<4> Vec2f<1>::operator == (
    const Vec2f<4>& aVec) const
{
    return (x == aVec.x) & (y ==  aVec.y);
}

RTFACT_VEC2F_INLINE const Mask<4> Vec2f<1>::operator != (
    const Vec2f<4>& aVec) const
{
    return (x != aVec.x) | (y !=  aVec.y);
}

#pragma endregion // Vec2f1 comparison operators

#pragma region Packet4 arithmetic operators

template<>
RTFACT_PACKET_INLINE const Vec2f<4> Packet<4, float>::operator * (
    const Vec2f<4>& aVec) const
{
    Vec2f<4> ret;

    ret.x.data = data * aVec.x.data;
    ret.y.data = data * aVec.y.data;

    return ret;
}

template<>
RTFACT_PACKET_INLINE const Vec2f<4> Packet<4, float>::operator / (
    const Vec2f<4>& aVec) const
{
    Vec2f<4> ret;

    ret.x.data = data / aVec.x.data;
    ret.y.data = data / aVec.y.data;

    return ret;
}

template<>
RTFACT_PACKET_INLINE const Vec2f<4> Packet<4, float>::operator * (
    const Vec2f<1>& aVec) const
{
    Vec2f<4> ret;

    ret.x.data = data * Data::replicate(aVec.x);
    ret.y.data = data * Data::replicate(aVec.y);

    return ret;
}

template<>
RTFACT_PACKET_INLINE const Vec2f<4> Packet<4, float>::operator / (
    const Vec2f<1>& aVec) const
{
    Vec2f<4> ret;

    ret.x.data = data / Data::replicate(aVec.x);
    ret.y.data = data / Data::replicate(aVec.y);

    return ret;
}

#pragma endregion // Packet4 arithmetic operators

#pragma region Mask4::blend

RTFACT_PACKET_INLINE const Vec2f<4> Mask<4>::iif(
    const Vec2f<4>& aVec1,
    const Vec2f<4>& aVec2) const
{
    return Vec2f<4>(
        iif(aVec1.x, aVec2.x),
        iif(aVec1.y, aVec2.y));
}

#pragma endregion // Mask4::blend

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__VEC2F4_HPP
