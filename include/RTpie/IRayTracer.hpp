/* 
 *  IRayTracer.hpp
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

#ifndef IRAYTRACER_H_
#define IRAYTRACER_H_

#include <RTpie/Config/Common.hpp>

#include <RTpie/Scene/IScene.hpp>
#include <RTpie/Tasks/IRenderTask.hpp>
#include <RTpie/Tasks/IPickTask.hpp>
#include <RTpie/Tasks/IIntersectTask.hpp>
#include <RTpie/Tasks/IRayGenerationTask.hpp>
#include <RTpie/Tasks/IRayPickTask.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IRAYTRACER_ID \
		{ 0xf2346d70, 0x5353, 0x401d, { 0x8d, 0xd5, 0x51, 0x5c, 0x16, 0x11, 0x97, 0x8f } }


/**
 *
 *  The IRayTracer Interface is the connecting piece between the Scene and any Rendering Task
 *
 *  The IRayTracer manages the structure required to perform RayTracing on the Scene
 *  (e.g. AccelerationStructures)
 *
 *  After creation, the IRayTracer is connected with a Scene via AttachScene
 *
 *  Afterwards, several Tasks can be created from the IRayTracer, including
 *  IRenderingTask and IPickingTask
 *
 *  Before creating any Task, you can call SyncStructures() to update internal structures.
 *  This call is optional, as any Task creation calls this function if necessary
 *  However, if Tasks are created and run in parallel, it is highly recommended to call
 *  SyncStructures() prior to any Task creation to prevent read and write conflicts
 *
 */
struct IRayTracer : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IRAYTRACER_ID)

    /*
     * Attach the scene to the RayTracer
     * Note: An IScene can be attached to an arbitrary number of IRayTracer Instances.
     * @param aScene the IScene Interface pointer of the scene
     * @return HRESULT
     */
    virtual HRESULT AttachScene(IScene *aScene) = 0;

    /**
     *  Synchronice Rendering Structures with content of the scene
     *  Note: This function is optional, as any Task Creation from IRayTracer calls this
     *  function automatically. However, calling this function first guarantuees that
     *  all Tasks created afterwards can be executed in parallel.
     *  @return HRESULT
     */
    virtual HRESULT SyncStructures() = 0;

    /**
     *  Create a RenderTaks
     *  @param _retval an IRenderTask interface pointer of the new RenderTaks
     *  @return HRESULT
     */
    virtual HRESULT CreateRenderTask(IRenderTask **_retval) = 0;

    /**
     *  Create a PickTaks
     *  @param _retval an IPickTask interface pointer of the new PickTask
     *  @return HRESULT
     */
    virtual HRESULT CreatePickTask(IPickTask **_retval) = 0;

    /**
     *  Create a RayGenerationTask
     *  @param _retval an IRayGenerationTask interface pointer of the new RayGenerationTask
     *  @return HRESULT
     */
    virtual HRESULT CreateRayGenerationTask(IRayGenerationTask **_retval) = 0;

    /**
     *  Create a RayPickTask
     *  @param _retval an IRayPickTask interface pointer of the new RayPickTask
     *  @return HRESULT
     */
    virtual HRESULT CreateRayPickTask(IRayPickTask **_retval) = 0;

    /**
     *  Create a IntersectTask
     *  @param _retval an IIntersectTaks interface pointer of the new IntersectTask
     *  @return HRESULT
     */
    virtual HRESULT CreateIntersectTask(IIntersectTask **_retval) = 0;

};

/**
 *  Create a new RayTracer
 *  @param _retval the IRayTracer interface pointer of the newly created RayTracer
 *  @return HRESULT
 */
RTFACT_RTPIE_API HRESULT CreateRayTracer(IRayTracer** _retval);

RTFACT_RTPIE_NAMESPACE_END

#endif /* RENDERER_H_ */
