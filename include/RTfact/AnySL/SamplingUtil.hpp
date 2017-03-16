/*
 * SamplingUtil.hpp
 *
 *  Created on: Mar 11, 2011
 *      Author: Vaclav Krajicek
 */

#ifndef SAMPLINGUTIL_HPP_
#define SAMPLINGUTIL_HPP_

#include "ShaderMath.hpp"

inline Vector CosineHemisphere(
    const Vector2D& aSamples,
    float* oPDF = NULL)
{
    const float term1 = 2* M_PI * aSamples.x;
    const float term2 = sqrtf(1.f - aSamples.y);

    const Vector ret(
        cos(term1) * term2,
        sin(term1) * term2,
        sqrt(aSamples.y));

    if(oPDF)
    {
        *oPDF = ret.z * INV_PI;
    }

    return ret;
}

inline Vector PowerCosineHemisphere(
    const Vector2D& aSamples,
    const float aPower,
    float* oPDF = NULL)
{
    const float term1 = 2 * M_PI * aSamples.x;
    const float term2 = pow(aSamples.y, 1/(aPower + 1.f));
    const float term3 = sqrt(1.f - term2 * term2);

    if(oPDF)
    {
        *oPDF =
            ((aPower + 1.f) * 0.5f * INV_PI) * pow(term2, aPower);
    }

    return Vector(
        cos(term1) * term3,
        sin(term1) * term3,
        term2);
}

#endif /* SAMPLINGUTIL_HPP_ */
