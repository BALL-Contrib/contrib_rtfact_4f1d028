/* 
 *  IScene.hpp
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
 
 /*
 * Renderer.h
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef ISCENE_H_
#define ISCENE_H_

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>

#include <RTpie/Scene/IGeometry.hpp>
#include <RTpie/Scene/IMesh.hpp>
#include <RTpie/Scene/IInstance.hpp>
#include <RTpie/Scene/ILight.hpp>
#include <RTpie/Scene/IAppearance.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_ISCENE_ID \
		{ 0xce163a93, 0xff01, 0x4c37, { 0x8c, 0x3, 0x1c, 0x4b, 0xf4, 0xef, 0xad, 0xc8 } }


/**
 * The IScene Interface is used to construct the scene for the rendering
 *
 * To create a new SceneInstance, call the function RTfact::RTpie::CreateSceneInstance
 *
 * Note that all content created from the scene ( including IGeometry, IMesh, IInterface,
 * IAppearance and ILight) cannot be exchanged with or connected to content created
 * from another scene. The only exception for this right now is IImage2D
 *
 */
struct IScene : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_ISCENE_ID)

    /**
     *  Create an empty Geometry.
     *  @param _retval the IGeometry interface pointer of the newly created object
     *  @return HRESULT
     */
    virtual HRESULT CreateGeometry(IGeometry** _retval) = 0;

    /**
     *  Get the number of objects of this scene.
     *  @param _retval the number of objects
     *  @return HRESULT
     */
    virtual HRESULT GetGeometryCount(unsigned int *_retval) = 0;

    /**
     *  Get an object by index.
     *  @param aIdx the index of the objects. This index has to be above 0
     *  and smaller than the number returned by GetGeometryCount()
     *  @param _retval the IGeometry interface pointer of the object
     *  @return HRESULT
     */
    virtual HRESULT GetGeometry(unsigned int aIdx, IGeometry** _retval) = 0;

    /**
     *  Create a light source.
     *  @param aType the type of the light source.
     *  Supported values: "PointLight", "SpotLight", "DirectionalLight"
     *  @param _retval the ILight interface pointer of the newly created light source
     *  @return HRESULT
     */
    virtual HRESULT CreateLight(const char* aType, ILight** _retval) = 0;

    /**
     *  Get the number of light sources of this scene.
     *  @param _retval the number of light sources
     *  @return HRESULT
     */
    virtual HRESULT GetLightCount(unsigned int *_retval) = 0;

    /**
     *  Get a light source by index.
     *  @param aIdx the index of the light source. This index has to be above 0
     *  and smaller than the number returned by GetLightCount().
     *  @param _retval the ILight interface pointer of the light source
     *  @return HRESULT
     */
    virtual HRESULT GetLight(unsigned int aIdx, ILight** _retval) = 0;

    /**
     *  Create an appearance
     *  @param aType the type of the appearance.
     *  Supported values: "PhongShader", "PlasticShader", "GlassShader"
     *  @param _retval the IAppearance interface pointer of the appearance
     *  @return HRESULT
     */
    virtual HRESULT CreateAppearance(const char* aType, IAppearance** _retval) = 0;

    /**
     *  Get the number of appearances of this scene.
     *  @param _retval the number of appearances
     *  @return HRESULT
     */
    virtual HRESULT GetAppearanceCount(unsigned int *_retval) = 0;


    /**
     *  Get an appearance by index.
     *  @param aIdx the index of the appearance. This index has to be above 0
     *  and smaller than the number returned by GetLightCount().
     *  @param _retval the IAppearance interface pointer of the appearance
     *  @return HRESULT
     */
    virtual HRESULT GetAppearance(unsigned int aIdx, IAppearance** _retval) = 0;


    /**
     *  Set the environment texture of the scene.
     *  @param aImage an IImage2Dr interface pointer to the image
     *  to be used as environment texture
     *  @return HRESULT
     */
    virtual HRESULT SetEnvironmentTexture(IImage2D* aImage) = 0;


    /**
     *  Set the Environment Color of the scene.
     *  Sets the environment color (i.e. background) to a solid color defined by RGB
     *  @param aRed Red component, range 0-1
     *  @param aGreen Green component, range 0-1
     *  @param aBlue Blue component, range 0-1
     *  @param aAlpha Alpha component, range 0-1
     */
    virtual HRESULT SetEnvironmentColor(const float aRed,
            const float aGreen, const float aBlue, const float aAlpha) = 0;

};

/**
 *  Create a new scene
 *  @param _retval the IScene interface pointer of the newly created scene
 *  @return HRESULT
 */
RTFACT_RTPIE_API HRESULT CreateScene(IScene** _retval);

RTFACT_RTPIE_NAMESPACE_END

#endif /* RENDERER_H_ */
