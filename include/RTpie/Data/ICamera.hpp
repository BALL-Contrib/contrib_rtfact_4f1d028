/*
 *  ICamera.hpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */


#ifndef ICAMERA_H_
#define ICAMERA_H_

#include <RTpie/Config/Common.hpp>

#include <RTpie/Base/IUnknown.hpp>
#include <RTpie/Data/BasicData.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_ICAMERA_ID \
	{ 0xc3f0be8d, 0x75e, 0x451b, { 0x98, 0xcf, 0x10, 0xdd, 0xfc, 0xe0, 0x86, 0xb0 } }

/**
 *  The ICamera Interface is used to configure a Camera
 *
 */
struct ICamera : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_ICAMERA_ID)

    /**
     *  get camera position
     *  @param pos the position of the camera
     *  @return HRESULT
     */
     virtual HRESULT GetPosition(float *aPosX, float *aPosY, float *aPosZ) = 0;

    /**
     *  set camera position
     *  @param pos the new position of the camera
     *  @return HRESULT
     */
    virtual HRESULT SetPosition(const float &aPosX, const float &aPosY, const float &aPosZ ) = 0;


    /**
     *  get camera Direction
     *  @param pos the Direction of the camera
     *  @return HRESULT
     */
    virtual HRESULT GetDirection(float *aDirX, float *aDirY, float *aDirZ) = 0;

    /**
     *  set camera Direction
     *  @param pos the new Direction of the camera
     *  @return HRESULT
     */
    virtual HRESULT SetDirection(const float &aDirX, const float &aDirY, const float &aDirZ) = 0;


    /**
     *  get camera UpVector
     *  @param pos the UpVector of the camera
     *  @return HRESULT
     */
    virtual HRESULT GetUpVector(float *aUpX, float *aUpY, float *aUpZ) = 0;

    /**
     *  set camera UpVector
     *  @param pos the new UpVector of the camera
     *  @return HRESULT
     */
    virtual HRESULT SetUpVector(const float &aUpX, const float &aUpY, const float &aUpZ) = 0;


    /**
     *  return the opening angle of the camera in degrees.
     *  @param _retval the opening angle of the camera in degrees
     *  @return HRESULT
     */
    virtual HRESULT GetOpeningAngle(float *_retval) = 0;


    /**
     *  Set the opening angle of the camera.
     *  @param angle the opening angle
     *  @return HRESULT
     */
    virtual HRESULT SetOpeningAngle(const float &angle) = 0;

    /** TODO: comment
     */
    virtual HRESULT GetFrustum( float * left, float * right,
                                float * top, float * bottom,
                                float * nearPlane, float * farPlane) = 0;

    /** TODO: comment
     */
    virtual HRESULT SetFrustum( const float &left, const float &right,
                                const float &top, const float &bottom,
                                const float &nearPlane, const float &farPlane) = 0;

};

/**
 *  Create a new camera
 *  @param _retval the ICamera interface pointer of the newly created camera
 *  @return HRESULT
 */
RTFACT_RTPIE_API HRESULT CreateCamera(ICamera** _retval);

RTFACT_RTPIE_NAMESPACE_END

#endif /* RENDERER_H_ */
