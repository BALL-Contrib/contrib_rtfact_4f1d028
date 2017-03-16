/*
 *  TLPickTask.hpp
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
 *  Author(s): Felix Klein, Byelozyorov Sergiy
 */

 #ifndef RTFACT_RTPIE__TLPICKTASK_HPP
#define RTFACT_RTPIE__TLPICKTASK_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Tasks/IPickTask.hpp>

#include "TwoLevelTracer/TLTask.hpp"
#include "Data/Camera.hpp"
#include "Scene/Instance.hpp"
#include "Scene/Mesh.hpp"


//-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------
#include <RTfact/Utils/TwoLevelPicking.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

/**
 * The result of picking provides the Instance and
 * Mesh Geometry hit by the ray.
 */
class TLPickTask : public TLTask, public IPickTask
{
public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IPickTask

    virtual HRESULT SetCamera(ICamera *aCamera)
    {
        mCamera = static_cast<Camera*>(aCamera);
        return S_OK;
    }

    virtual HRESULT GetCamera(ICamera **_retval)
    {
        *_retval = mCamera.get();
        return S_OK;
    }

    virtual HRESULT Pick(const int aX, const int aY,
            const int aResX, const int aResY)
    {
        if(!preRun())
            return E_FAIL;

        if(!mCamera)
            return E_FAIL;

        typedef RTfact::TwoLevelPicking<SceneConfig> tPicking;
        typedef t_PixelSampler::Sample<1> tRayPacketSample;

        // TODO: Check if Intersection Test is possible

        tRayPacketSample rayPacketSample;
        rayPacketSample.imageX = static_cast<float>(aX);
        rayPacketSample.imageY = static_cast<float>(aY);

        t_Camera camera = mCamera->getRTfactCamera(aResX, aResY);

        //
        tPicking picking;
        tPicking::Result pickResult;

        picking.pick<t_Tree, tRayPacketSample,t_Camera>(
        rayPacketSample,
        camera,
        mIntersector,
        mTracer->tree,
        pickResult);

        if(!pickResult.isEmpty())
        {
            Instance *instance;
            Mesh *mesh;
            HRESULT rv = mScene->getHandle(pickResult.mInstance.get(), &instance);
            if(FAILED(rv))
              return E_FAIL;

            rv = mScene->getHandle(pickResult.mMesh.get(), &mesh);
            if(FAILED(rv)){
              instance->Release();
              return E_FAIL;
            }

            mHasContent = true;
            mInstance = instance;
            mMesh = mesh;
            mPosition = float3(pickResult.mPosition.xAt(0), pickResult.mPosition.yAt(0), pickResult.mPosition.zAt(0));
            mNormal = float3(pickResult.mNormal.xAt(0), pickResult.mNormal.yAt(0), pickResult.mNormal.zAt(0));

            instance->Release();
            mesh->Release();
        }
        else
            mHasContent = false;

        return S_OK;
    }

    virtual HRESULT HasResult(bool *_retval)
    {
        *_retval = mHasContent;
        return S_OK;
    }

    virtual HRESULT GetInstance(IInstance **_retval){
        if(!mHasContent)
            return E_FAIL;

        mInstance->AddRef();
        *_retval = mInstance.get();
        return S_OK;
    }

    virtual HRESULT GetMesh(IMesh **_retval){
        if(!mHasContent)
            return E_FAIL;

        mMesh->AddRef();
        *_retval = mMesh.get();
        return S_OK;
    }

    virtual HRESULT GetPosition(float *aV1, float *aV2, float *aV3)
    {
        if(!mHasContent)
            return E_FAIL;

        *aV1 = mPosition.x;
        *aV2 = mPosition.y;
        *aV3 = mPosition.z;

        return S_OK;
    }

    virtual HRESULT GetNormal(float *aV1, float *aV2, float *aV3)
    {
        if(!mHasContent)
            return E_FAIL;

        *aV1 = mNormal.x;
        *aV2 = mNormal.y;
        *aV3 = mNormal.z;

        return S_OK;
    }
    // TLTask:

protected:

    virtual void sceneChanged() {}

    // Internal

public:

    TLPickTask(TwoLevelTracer *aTracer)
        : TLTask(aTracer), mHasContent(false){}

    virtual ~TLPickTask()
    {
    }

    // Picking Configuration
    CameraPtr mCamera;

    // Result Data
    bool mHasContent;
    InstancePtr mInstance;
    MeshPtr mMesh;
    float3 mPosition;
    float3 mNormal;


    // RTfact Data
    t_Intersector mIntersector;

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__PICKING_HPP
