/* 
 *  Camera.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__CAMERA_HPP
#define RTFACT__CAMERA_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>

RTFACT_NAMESPACE_BEGIN

class RTFACT_ALIGN_SIMD Camera
{
protected:

    Vec3f<1> mPos;
    Vec3f<1> mXAxis;
    Vec3f<1> mYAxis;
    Vec3f<1> mZAxis;
    Vec3f<1> mUp;

    float mOpeningAngle;
    uint mResX;
    uint mResY;

public:
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    struct RTFACT_ALIGN_SIMD Params
    {
        RTFACT_SIMD_ALIGNED_NEW_OPERATORS

        Vec3f<1> position;
        Vec3f<1> direction;
        Vec3f<1> up;

        float openingAngle;
        uint resX;
        uint resY;
    };

    void getParams(
        Params& oParams) const
    {
        oParams.position = mPos;
        oParams.direction = mZAxis;
        oParams.up = mUp;
        oParams.openingAngle = mOpeningAngle;
        oParams.resX = mResX;
        oParams.resY = mResY;
    }

    void setParams(
        const Params& aParams)
    {
        mPos = aParams.position;
        mZAxis = aParams.direction;
        mUp = aParams.up;

        mXAxis = Math::cross(aParams.direction, aParams.up);
        mYAxis = Math::cross(mXAxis, mZAxis);

        mOpeningAngle = aParams.openingAngle;

        mResX = aParams.resX;
        mResY = aParams.resY;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__CAMERA_HPP
