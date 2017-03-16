/*
 *  IIntersectTask.hpp
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

#ifndef IINTERSECTTASK_H_
#define IINTERSECTTASK_H_

#include <RTpie/Config/Common.hpp>

#include <RTpie/Scene/IScene.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IINTERSECTTASK_ID \
		{ 0x31ed9a6d, 0xa86f, 0x4029, { 0x89, 0x77, 0xc2, 0x9c, 0x3e, 0x70, 0xda, 0xcd } }


/**
 *
 *  The IRenderer Interface is used to configure and execute
 *  the rendering process and related operations
 *
 *  Make sure following is done prior to any render operation:
 *  - Attach a scene            (use AttachScene)
 *  - Attach a framebuffer      (use AttachFloatFrameBuffer or AttachByteFrameBuffer)
 *  - Configure camera          (use SetCameraPosition and SetCameraOpeningAngle)
 *
 *  After configuration, the following operations can be executed:
 *  - RendererToBuffer          Render the Scene to the Buffer
 *  - IntersetRays              Perform an intersection test with the speficied rays
 *                              returning depth values
 *  - Pick                      Pick an Geometry behind a certain pixel of the final rendering
 *
 *  Before rendering, you can call PrepareRendering() to update internal structures.
 *  This call is optional, as any operation calls this function if necessary
 *  However, if the operations are run in parallel, it is highly recommended to call
 *  PrepareRendering() prior to the operations to prevent read and write conflicts
 *
 */
struct IIntersectTask : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IINTERSECTTASK_ID)

    /**
     *  travers a number of rays with the given origin and direction through the scene and
     *  store for each ray the distance from it's origin to the first intersection point.
     *  @param aOrigins the origins of the rays. 3 consecutive floats specify one origin.
     *  @param aDirections the directions of the rays. 3 consecutive floats specify one direction.
     *  @param aRayCount the number of rays
     *  @param oDistances the distance result of the traversal is stored at this memory adress. One float for each ray.
     *  @return HRESULT
     */
    virtual HRESULT IntersectRays(
        const float* aOrigins,
        const float* aDirections,
        const unsigned int aRayCount,
        float* oDistances) = 0;

};

RTFACT_RTPIE_NAMESPACE_END

#endif /* IINTERSECTTASK_H_ */
