#ifndef RTFACT__TEXTUREUTILS_HPP
#define RTFACT__TEXTUREUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

float lanczos(
    const float aX,
    const float aTau = 2.f)
{
    float absX = Math::abs(aX);

    if(absX < 1e-5)
    {
        return 1.f;
    }

    if(absX > 1.f)
    {
        return 0;
    }

    absX *= RTFACT_PI;

    const float absXTau = absX * aTau;

    const float sinc1 = Math::sin(absXTau) / absXTau;
    const float sinc2 = Math::sin(absX) / absX;

    return sinc1 * sinc2;
}

RTFACT_NAMESPACE_END

#endif // RTFACT__TEXTUREUTILS_HPP
