#ifndef RTFACT__VEC2FMATHUTILS_HPP
#define RTFACT__VEC2FMATHUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Math {

#pragma region min/max

RTFACT_VEC2F_INLINE const Vec2f<1> min(
    const Vec2f<1>& aVec1,
    const Vec2f<1>& aVec2)
{
    return Vec2f<1>(min(aVec1.x, aVec1.y), min(aVec1.x, aVec1.y));
}

RTFACT_VEC2F_INLINE const Vec2f<1> max(
    const Vec2f<1>& aVec1,
    const Vec2f<1>& aVec2)
{
    return Vec2f<1>(max(aVec1.x, aVec1.y), max(aVec1.x, aVec1.y));
}

RTFACT_VEC2F_INLINE float min(
    const Vec2f<1>& aVec)
{
    return min(aVec.x, aVec.y);
}

RTFACT_VEC2F_INLINE float max(
    const Vec2f<1>& aVec)
{
    return max(aVec.x, aVec.y);
}

RTFACT_VEC2F_INLINE const Vec2f<4> min(
    const Vec2f<4>& aVec1,
    const Vec2f<4>& aVec2)
{
    return Vec2f<4>(min(aVec1.x, aVec2.x), min(aVec1.y, aVec2.y));
}

RTFACT_VEC2F_INLINE const Vec2f<4> max(
    const Vec2f<4>& aVec1,
    const Vec2f<4>& aVec2)
{
    return Vec2f<4>(max(aVec1.x, aVec2.x), max(aVec1.y, aVec2.y));
}

RTFACT_VEC2F_INLINE const Packet<4, float> min(
    const Vec2f<4>& aVec)
{
    return min(aVec.x, aVec.y);
}

RTFACT_VEC2F_INLINE const Packet<4, float> max(
    const Vec2f<4>& aVec)
{
    return max(aVec.x, aVec.y);
}

#pragma endregion // min/max

#pragma region dot

RTFACT_MATH_INLINE float dot(
    const Vec2f<1>& aVec1,
    const Vec2f<1>& aVec2)
{
    return aVec1.x * aVec2.x + aVec1.y * aVec2.y;
}

RTFACT_MATH_INLINE const Packet<4, float> dot(
    const Vec2f<4>& aVec1,
    const Vec2f<4>& aVec2)
{
    return aVec1.x * aVec2.x + aVec1.y * aVec2.y;
}

RTFACT_MATH_INLINE const Packet<4, float> dot(
    const Vec2f<1>& aVec1,
    const Vec2f<4>& aVec2)
{
    return dot(Vec2f<4>::replicate(aVec1), aVec2);
}

RTFACT_MATH_INLINE const Packet<4, float> dot(
    const Vec2f<4>& aVec1,
    const Vec2f<1>& aVec2)
{
    return dot(aVec1, Vec2f<4>::replicate(aVec2));
}

#pragma endregion // dot

#pragma region reciprocal

RTFACT_VEC2F_INLINE const Vec2f<1> reciprocal(
    const Vec2f<1>& aVec)
{
    return Vec2f<1>(reciprocal(aVec.x), reciprocal(aVec.y));
}

RTFACT_VEC2F_INLINE const Vec2f<4> reciprocal(
    const Vec2f<4>& aVec)
{
    return Vec2f<4>(
        Math::reciprocal(aVec.x),
        Math::reciprocal(aVec.y));
}

#pragma endregion // reciprocal

} // namespace Math
RTFACT_NAMESPACE_END

#endif // RTFACT__VEC2FMATHUTILS_HPP
