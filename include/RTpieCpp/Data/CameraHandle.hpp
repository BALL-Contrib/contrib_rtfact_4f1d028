/*
 *  CameraHandle.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
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
 *  Created on: 2010-09-26 18:55:36 +0200
 *  Author(s): Felix Klein
 */

 #ifndef RTFACT_HANDLE_CAMERA_HPP
#define RTFACT_HANDLE_CAMERA_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Data/ICamera.hpp>

#define IMPL_INTERFACE RTfact::RTpie::ICamera
WRAPPER_FWD(CameraHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Data/Float3.hpp>

#define IMPL_INTERFACE RTfact::RTpie::ICamera
WRAPPER_START(CameraHandle, BaseHandle)

    float3 getPosition()
    IMPLEMENT_WRAPPER_FLOAT3(  GetPosition )

    void setPosition(const float3 &pos)
    IMPLEMENT_WRAPPER_NO_RETURN( SetPosition(pos.x, pos.y, pos.z) )

    float3 getDirection()
    IMPLEMENT_WRAPPER_FLOAT3( GetDirection )

    void setDirection(const float3 &dir)
    IMPLEMENT_WRAPPER_NO_RETURN( SetDirection(dir.x, dir.y, dir.z) )

    float3 getUpVector()
    IMPLEMENT_WRAPPER_FLOAT3( GetUpVector )

    void setUpVector(const float3 &up)
    IMPLEMENT_WRAPPER_NO_RETURN( SetUpVector(up.x, up.y, up.z) )

    float getOpeningAngle()
    IMPLEMENT_WRAPPER_ARG0( float, GetOpeningAngle )

    void setOpeningAngle(const float &angle)
    IMPLEMENT_WRAPPER_NO_RETURN( SetOpeningAngle(angle) )

    void getFrustum(float * left, float * right,
                    float * top, float * bottom,
                    float * nearPlane, float * farPlane)
    IMPLEMENT_WRAPPER_NO_RETURN( GetFrustum(left, right, top, bottom, nearPlane, farPlane) )

    void setFrustum(const float &left, const float &right,
                    const float &top, const float &bottom,
                    const float &nearPlane, const float &farPlane)
    IMPLEMENT_WRAPPER_NO_RETURN( SetFrustum(left, right, top, bottom, nearPlane, farPlane) )


WRAPPER_END
#undef IMPL_INTERFACE


RTFACT_HANDLE_NAMESPACE_BEGIN

inline CameraHandle CreateCameraHandle()
{
    RTfact::RTpie::ICamera *result;
    RTfact::RTpie::CreateCamera(&result);
    return result;
}

RTFACT_HANDLE_NAMESPACE_END



#endif // RTFACT_HANDLE_RENDERER_HPP
