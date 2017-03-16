/*
 * IRayGenerationTask.hpp
 *
 *  Created on: 04.10.2010
 *      Author: Boris Broenner
 */

#ifndef IRAYGENERATIONTASK_H_
#define IRAYGENERATIONTASK_H_

#include <RTpie/Config/Common.hpp>

#include <RTpie/Data/ICamera.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IRAYGENERATIONTASK_ID \
       { 0xe4e1041d, 0x4bdd, 0x422a, { 0x97, 0x95, 0x4f, 0xf9, 0x6a, 0x16, 0xda, 0x80 } }


/**
 *
 *  The IRayGenerationTask Interface is used to configure and execute a
 *  ray generation operation.
 *
 *  Ray generation needs the following to be configured:
 *  - a Camera Configuration (via ICamera)
 *  - a screen resolution and a point inside this screen
 *
 *  After configuration call Run() to perform the generation.
 *
 *  Afterwards the following function can be used to read the result:
 *  - HasResult            Is there any result at all?
 *  - GetRayOrigin     Get the generated ray's origin
 *  - GetRayDirection  Get the generated ray's direction
 *
 */
struct IRayGenerationTask : public IUnknown
{

   RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IRAYGENERATIONTASK_ID)

    /**
     *  set the camera of the Picking Task
     *  @param aCamera an ICamera interface pointer to the camera
     *  @return HRESULT
     */
    virtual HRESULT SetCamera(ICamera *aCamera) = 0;

    /**
     *  get the camera for the Rendering Task
     *  @param _retval an ICamera interface pointer to the camera
     *  @return HRESULT
     */
    virtual HRESULT GetCamera(ICamera **_retval) = 0;

    /**
     *  Shoot a ray through the given point in screen, and compute pick result
     *  @param aX screen space x coordinate 0 means left border
     *  @param aY screen space y coordinate 0 means upper border
     *  @param aResX the x resolution of the screen space
     *  @param aResY the y resolution of the screen space
     *  @return HRESULT
     */
    virtual HRESULT GenerateRay(const int aX, const int aY,
           const int aResX, const int aResY) = 0;

    // -----------------------------------------------------
    // Results of ray generation
    // -----------------------------------------------------

    /**
     * This always returns false, if the ray generation was not performed yet,
     * else it always returns true.
     * @param _retval contains true if ray generation has been performed.
     * @return HRESULT
     */
    virtual HRESULT HasResult(bool *_retval) = 0;

    /**
     * Access ray's origin of the generation result
     * @param _retval origin of the ray
     * @return HRESULT
     */
    virtual HRESULT GetRayOrigin(float *aX, float *aY, float *aZ) = 0;

    /**
     * Access ray's direction of the generation result
     * @param _retval direction of the ray
     * @return HRESULT
     */
    virtual HRESULT GetRayDirection(float *aX, float *aY, float *aZ) = 0;

};

RTFACT_RTPIE_NAMESPACE_END

#endif /* IRAYGENERATIONTASK_H_ */
