/* 
 *  RasterizationCamera.hpp
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
 *  Created on: 2008-11-13 03:17:48 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__RASTERIZATIONCAMERA_HPP
#define RTFACT__RASTERIZATIONCAMERA_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Camera.hpp>

RTFACT_NAMESPACE_BEGIN

class RasterizationCamera :
    public Camera
{
protected:

    float mNearPlane;
    float mFarPlane;

public:

    struct Params :
        public Camera::Params
    {
        float nearPlane;
        float farPlane;

        Params() :
            nearPlane(0.001f),
            farPlane(RTFACT_INFINITY)
        {}
    };

    void getParams(
        Params& oParams) const
    {
        Camera::getParams(oParams);

        oParams.nearPlane = mNearPlane;
        oParams.farPlane = mFarPlane;
    }

    void setParams(
        const Params& aParams)
    {
        Camera::setParams(aParams);

        mNearPlane = aParams.nearPlane;
        mFarPlane = aParams.farPlane;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__RASTERIZATIONCAMERA_HPP
