/* 
 *  RayTracingCamera.hpp
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
 *  Created on: 2009-02-17 16:54:17 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__RTCAMERA_HPP
#define RTFACT__RTCAMERA_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Camera.hpp>
#include <RTfact/Concept/PixelSampler.hpp>

#include <RTfact/Utils/RayPacket.hpp>

RTFACT_NAMESPACE_BEGIN

class RayTracingCamera :
    public Camera
{
protected:

    float mResXf;
    float mResYf;

public:

    void setParams(
        Params& aParams)
    {
        Camera::setParams(aParams);

        mResXf = static_cast<float>(mResX);
        mResYf = static_cast<float>(mResY);
    }

    template<
        uint taSize>
    RTFACT_CAMERA_INLINE RayPacket<taSize> generateRay(
        PixelSampler::Sample<taSize>& aSample) const;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__RTCAMERA_HPP
