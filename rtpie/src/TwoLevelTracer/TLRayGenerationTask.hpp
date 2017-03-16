#ifndef RTFACT_RTPIE__TLRAYGENERATIONTASK_HPP
#define RTFACT_RTPIE__TLRAYGENERATIONTASK_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Tasks/IRayGenerationTask.hpp>

#include "TwoLevelTracer/TLTask.hpp"
#include "Data/Camera.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

/**
 * This task generates a ray given screen coordinates.
 */
class TLRayGenerationTask : public TLTask, public IRayGenerationTask
{
public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IGenerateRayTask

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

    virtual HRESULT GenerateRay(const int aX, const int aY,
               const int aResX, const int aResY)
    {
        if(!preRun())
            return E_FAIL;

        if(!mCamera)
            return E_FAIL;

        typedef t_PixelSampler::Sample<1> tRayPacketSample;
        typedef RTfact::RayPacket<1> tRay;

        // TODO: Check if Intersection Test is possible

        tRayPacketSample rayPacketSample;
        rayPacketSample.imageX = (float)aX;
        rayPacketSample.imageY = (float)aY;


        t_Camera camera = mCamera->getRTfactCamera(aResX, aResY);

        tRay ray = camera.generateRay(rayPacketSample);

        mRayOrigin = float3(ray.org.xAt(0), ray.org.yAt(0), ray.org.zAt(0));
        mRayDirection = float3(ray.dir.xAt(0), ray.dir.yAt(0), ray.dir.zAt(0));

        mGeneratedRay = true;

        return S_OK;
    }

    virtual HRESULT HasResult(bool *_retval)
    {
        *_retval = mGeneratedRay;
        return S_OK;
    }

    virtual HRESULT GetRayOrigin(float *aV1, float *aV2, float *aV3)
    {
        if(!mGeneratedRay)
            return E_FAIL;

        *aV1 = mRayOrigin.x;
        *aV2 = mRayOrigin.y;
        *aV3 = mRayOrigin.z;

        return S_OK;
    }

    virtual HRESULT GetRayDirection(float *aV1, float *aV2, float *aV3)
    {
        if(!mGeneratedRay)
            return E_FAIL;

        *aV1 = mRayDirection.x;
        *aV2 = mRayDirection.y;
        *aV3 = mRayDirection.z;

        return S_OK;
    }
    // TLTask:

protected:

    virtual void sceneChanged() {}

    // Internal

public:

    TLRayGenerationTask(TwoLevelTracer *aTracer)
        : TLTask(aTracer), mGeneratedRay(false){}

    virtual ~TLRayGenerationTask()
    {
    }

    // Picking Configuration
    CameraPtr mCamera;

    // Result Data
    bool mGeneratedRay;
    float3 mRayOrigin;
    float3 mRayDirection;
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__TLRAYGENERATIONTASK_HPP
