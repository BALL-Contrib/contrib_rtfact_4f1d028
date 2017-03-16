/*
 *  Camera.hpp
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
 *  Created on: 2010-09-26 19:11:26 +0200
 *  Author(s): Felix Klein
 */

 #ifndef RTFACT_RTPIE__IMAGEIMPL_HPP
#define RTFACT_RTPIE__IMAGEIMPL_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Data/ICamera.hpp>

RTFACT_RTPIE_FORWARD(Camera)


#include "Base/Base.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

class Camera :
    public Base, public ICamera
{
public:

    // IUnknown

    RTFACT_RTPIE_FORWARD_BASE

    RTFACT_RTPIE_DECLARE_QI

    // ICamera

    virtual HRESULT GetPosition(float *aPosX, float *aPosY, float *aPosZ)
    {
        *aPosX = mParams.position.x;
        *aPosY = mParams.position.y;
        *aPosZ = mParams.position.z;
        return S_OK;
    }

    virtual HRESULT SetPosition(const float &aPosX, const float &aPosY, const float &aPosZ)
    {
        Vec3f1 newPos = Vec3f1(aPosX, aPosY, aPosZ);
        if(newPos != mParams.position)
        {
            mParams.position = newPos;
            ++mVersion;
        }

        return S_OK;
    }

    virtual HRESULT GetDirection(float *aDirX, float *aDirY, float *aDirZ)
    {
        *aDirX = mParams.direction.x;
        *aDirY = mParams.direction.y;
        *aDirZ = mParams.direction.z;
        return S_OK;
    }

    virtual HRESULT SetDirection(const float &aDirX, const float &aDirY, const float &aDirZ)
    {
        Vec3f1 newDir = Vec3f1(aDirX, aDirY, aDirZ);
        if(newDir != mParams.direction){
            mParams.direction = newDir;
            ++mVersion;
        }
        return S_OK;
    }

    virtual HRESULT GetUpVector(float *aUpX, float *aUpY, float *aUpZ)
    {
        *aUpX = mParams.up.x;
        *aUpY = mParams.up.y;
        *aUpZ = mParams.up.z;
        return S_OK;
    }

    virtual HRESULT SetUpVector(const float &aUpX, const float &aUpY, const float &aUpZ)
    {
        Vec3f1 newUp = Vec3f1(aUpX, aUpY, aUpZ);
        if(newUp != mParams.up){
            mParams.up = newUp;
            ++mVersion;
        }

        return S_OK;
    }

    virtual HRESULT GetOpeningAngle(float *_retval){
        *_retval = mParams.openingAngle;
        return S_OK;
    }

    virtual HRESULT SetOpeningAngle(const float &angle){
        if(angle != mParams.openingAngle){
            mParams.openingAngle = angle;
            ++mVersion;
        }
        return S_OK;
    }

    virtual HRESULT GetFrustum( float * left, float * right,
                                float * top, float * bottom,
                                float * nearPlane, float * farPlane){

        *left = mParams.frustumLeft;
        *right = mParams.frustumRight;
        *top = mParams.frustumTop;
        *bottom = mParams.frustumBottom;
        *nearPlane = mParams.frustumNearPlane;
        *farPlane = mParams.frustumFarPlane;

        return S_OK;
    }

    virtual HRESULT SetFrustum( const float &left, const float &right,
                                const float &top, const float &bottom,
                                const float &nearPlane, const float &farPlane){

        mParams.frustumLeft = left;
        mParams.frustumRight = right;
        mParams.frustumTop = top;
        mParams.frustumBottom = bottom;
        mParams.frustumNearPlane = nearPlane;
        mParams.frustumFarPlane = farPlane;

        //TODO: set?

        return S_OK;
    }

    // Internal

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS


    t_Camera::Params mParams;

    Camera()
    {
        mParams.position     = Vec3f<1>(-2.0, 2.0, 0);
        mParams.direction    = Vec3f<1>(1.0, -0.5, 0).normalize();
        mParams.up           = Vec3f<1>(0, 1, 0).normalize();
        mParams.openingAngle = 60;
        mParams.resX         = 64;
        mParams.resY         = 64;
    };

    t_Camera getRTfactCamera(int resX, int resY) const
    {
        t_Camera::Params params = mParams;
        params.resX = resX;
        params.resY = resY;

        t_Camera    camera;
        camera.setParams(params);

        return camera;
    }

    virtual ~Camera(){}

    unsigned int getVersion(){
        return mVersion;
    }

private:
    unsigned int mVersion;

};


RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGEIMPL_HPP
