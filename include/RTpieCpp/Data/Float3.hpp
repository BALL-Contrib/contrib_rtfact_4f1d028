/*
 * Float3.hpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Lachsen
 */

#ifndef FLOAT3_HPP_
#define FLOAT3_HPP_

#include <RTpieCpp/Config/Common.hpp>

RTFACT_HANDLE_NAMESPACE_BEGIN

struct float3
{

    union {
        struct { float x, y, z; };
        struct { float data[3]; };
    };

    float3() {}

    float3(
        const float aX,
        const float aY,
        const float aZ
    ) :
        x(aX),
        y(aY),
        z(aZ)
    {}

    explicit float3(
        const float* aData
    ) :
        x(aData[0]),
        y(aData[1]),
        z(aData[2])
    {}

    float& operator[](
        const unsigned int aIndex)
    {
        return data[aIndex];
    }

    const float& operator[](
        const unsigned int aIndex) const
    {
        return data[aIndex];
    }

    const bool operator==(const float3 &aOther)const
    {
        return (x == aOther.x && y == aOther.y && z == aOther.z);
    }
};

RTFACT_HANDLE_NAMESPACE_END


#endif /* FLOAT3_HPP_ */
