/* 
 *  IPickTask.hpp
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

#ifndef IPICKTASK_H_
#define IPICKTASK_H_

#include <RTpie/Config/Common.hpp>

#include <RTpie/Data/ICamera.hpp>
#include <RTpie/Scene/IInstance.hpp>
#include <RTpie/Scene/IMesh.hpp>
#include <RTpie/Tasks/IPickResult.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IPICKTASK_ID \
		{ 0x2454bcf5, 0x5ccb, 0x4188, { 0xbf, 0x89, 0x6d, 0xb6, 0x93, 0x2c, 0x93, 0x7e } }

/**
 *
 *  The IPick Interface is used to configure and execute
 *  an object picking operation.
 *
 *  Picking need the following to be configure:
 *  - a Camera Configuration (via ICamera)
 *  - a screen resolution and a point inside this screen
 *
 *  After configuration call Run() to perform the picking.
 *
 *  Afterwards the functions defined in the IPickResult interface can be
 *  used to obtain the result.
 *
 */
struct IPickTask : public IPickResult
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IPICKTASK_ID)

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
    virtual HRESULT Pick(const int aX, const int aY,
		const int aResX, const int aResY) = 0;

};

RTFACT_RTPIE_NAMESPACE_END

#endif /* RENDERER_H_ */
