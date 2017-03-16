/* 
 *  RenderTaskHandle.hpp
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
 
 #ifndef RTFACT_HANDLE_RENDERTASK_HPP
#define RTFACT_HANDLE_RENDERTASK_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Tasks/IRenderTask.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRenderTask
WRAPPER_FWD(RenderTaskHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/SceneHandle.hpp>
#include <RTpieCpp/Data/FrameBufferHandle.hpp>
#include <RTpieCpp/Data/CameraHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRenderTask
WRAPPER_START(RenderTaskHandle, BaseHandle)

    // Frame Buffer Management

    void setFrameBuffer(FrameBufferHandle aBuffer)
    IMPLEMENT_WRAPPER_NO_RETURN( SetFrameBuffer(aBuffer.get()) )

    FrameBufferHandle getFrameBuffer()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IFrameBuffer*, GetFrameBuffer)

    void setCamera(CameraHandle aCamera)
    IMPLEMENT_WRAPPER_NO_RETURN( SetCamera(aCamera.get()) )

    CameraHandle getCamera()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::ICamera*, GetCamera)

    void setOverSamplingRate(unsigned int rate)
    IMPLEMENT_WRAPPER_NO_RETURN( SetOverSamplingRate(rate) )

    unsigned int getOverSamplingRate()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetOverSamplingRate)

    void setAccumulatePixels(bool active)
    IMPLEMENT_WRAPPER_NO_RETURN( SetAccumulatePixels(active) )

    bool getAccumulatePixels()
    IMPLEMENT_WRAPPER_ARG0(bool, GetAccumulatePixels)

    void setGlobalIllumination(bool active)
    IMPLEMENT_WRAPPER_NO_RETURN( SetGlobalIllumination(active) )

    bool getGlobalIllumination()
    IMPLEMENT_WRAPPER_ARG0(bool, GetGlobalIllumination)

    void setMaxDepth(unsigned int maxDepth)
    IMPLEMENT_WRAPPER_NO_RETURN( SetMaxDepth(maxDepth) )

    unsigned int getMaxDepth()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetMaxDepth)

    void run()
    IMPLEMENT_WRAPPER_NO_RETURN( Run() )

WRAPPER_END
#undef IMPL_INTERFACE



#endif // RTFACT_HANDLE_RENDERER_HPP
