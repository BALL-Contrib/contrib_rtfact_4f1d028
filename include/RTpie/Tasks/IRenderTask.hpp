/* 
 *  IRenderTask.hpp
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
 
 /*
 * Renderer.h
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef IRENDERER_H_
#define IRENDERER_H_

#include <RTpie/Config/Common.hpp>

#include <RTpie/Scene/IScene.hpp>
#include <RTpie/Data/IFrameBuffer.hpp>
#include <RTpie/Data/ICamera.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IRENDERTASK_ID \
		{ 0x1a9686dd, 0x42f7, 0x4154, { 0xb9, 0xdd, 0x77, 0xca, 0x71, 0x15, 0x5, 0xd7 } }


/**
 *
 *  The IRenderTask Interface is used to configure and execute
 *  a rendering process
 *
 *  Make sure following is done prior to any render operation:
 *  - Attach a renderBuffer     (use SetRenderBuffer)
 *  - Configure camera          (use SetCameraPosition and SetCameraOpeningAngle)
 *
 *  After configuration, call Run() to perfom the Rendering.
 *  Run() can be called after reconfiguration the RenderTask to execute another
 *  rendering operation.
 *
 *  If you want to execute IRenderTask in a certain thread, make sure the creation
 *  of the RenderTask and the execution of Run() happen in the same thread
 */
struct IRenderTask : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IRENDERTASK_ID)

    /**
     * set the RenderBuffer to the RenderTask
     * @param aBuffer an IRenderBuffer interface pointer of the RenderBuffer
     * @return HRESULT
     */
    virtual HRESULT SetFrameBuffer(IFrameBuffer *aBuffer) = 0;

    /**
     * get the RenderBuffer to the RenderTask
     * @param aBuffer the IRenderBuffer interface pointer of the RenderBuffer
     * @return HRESULT
     */
    virtual HRESULT GetFrameBuffer(IFrameBuffer **_retval) = 0;


    /**
     *  set the camera of the Rendering Task
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
     *  sets the oversampling rate of the pixel sampler
     *  @param rate the new oversampling rate (must be a power of 2)
     *  @return HRESULT
     */
    virtual HRESULT SetOverSamplingRate(unsigned int rate) = 0;


    /**
     *  get the current oversampling rate of the pixel sampler
     *  @param rate the new oversampling rate (must be a power of 2)
     *  @return HRESULT
     */
    virtual HRESULT GetOverSamplingRate(unsigned int *_retval) = 0;

    /**
     *  (de)activates pixel accumulation
     *  @param active true iff pixel accumulation should be active
     *  @return HRESULT
     */
    virtual HRESULT SetAccumulatePixels(bool active) = 0;


    /**
     *  get pixel accumulation
     *  @param _retval true iff pixel accumulation is active
     *  @return HRESULT
     */
    virtual HRESULT GetAccumulatePixels(bool *_retval) = 0;

    /**
     *  (de)activates global illumination calculation
     *  @param active true if GI should be active
     *  @return HRESULT
     */
    virtual HRESULT SetGlobalIllumination(bool active) = 0;

    /**
     *  get global illumination calculation
     *  @param _retval true if GI is active
     *  @return HRESULT
     */
    virtual HRESULT GetGlobalIllumination(bool *_retval) = 0;

    /**
     *  set global value of maximu depth of ray tracing
     *  @param maxDepth maximum depth of ray tracing, 0 means to use default
     *  @return HRESULT
     */
    virtual HRESULT SetMaxDepth(unsigned int maxDepth) = 0;

    /**
     *  get maximum depth of ray tracing
     *  @param _retval max depth of ray tracing
     *  @return HRESULT
     */
    virtual HRESULT GetMaxDepth(unsigned int *_retval) = 0;

    /**
     *  Perform the RenderingTask
     *  @return HRESULT
     */
    virtual HRESULT Run() = 0;

};


RTFACT_RTPIE_NAMESPACE_END

#endif /* RENDERER_H_ */
