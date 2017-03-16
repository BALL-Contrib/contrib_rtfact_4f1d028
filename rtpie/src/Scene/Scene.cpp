/*
 *  Scene.cpp
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

 #define RTFACT_RTPIE_LIB

 //-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------

 #include <RTfact/Config/Init.hpp>

#include <Scene/Scene.hpp>

#include <Scene/Geometry.hpp>

#include <RTfact/Utils/FPSMeter.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/cast.hpp>
//#include <RTpie/RTLambMaterial.hpp>

//-----------------------------------------------------------------------------
// RTpie Display Includes
//-----------------------------------------------------------------------------

#include "Scene/Shader/RTPhongShader.hpp"
#include "Scene/Shader/RTPlasticShader.hpp"
#include "Scene/Shader/RTGlassShader.hpp"
#include "Scene/Shader/RTPointLight.hpp"
#include "Scene/Shader/RTSpotLight.hpp"
#include "Scene/Shader/RTDirectionalLight.hpp"
#include "Scene/Shader/RTQuadLight.hpp"
#include "Scene/Shader/RTHemiCosineLight.hpp"

#include "Scene/Shader/XML3DPhongShader.hpp"
#include "Scene/Shader/XML3DPhongShaderGI.hpp"
#include "Scene/Shader/XML3DMatteShader.hpp"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RTFACT_RTPIE_NAMESPACE_BEGIN

RTFACT_RTPIE_QI_START(Scene)
RTFACT_RTPIE_QI_ADD(IScene)
RTFACT_RTPIE_QI_END

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

Scene::Scene()
{
    mVersion = 1;
    mModified = true;
    // Create Default Appearance
    IAppearance * imat;
    CreateAppearance("PhongShader", &imat);

    imat->SetParam3f("diffuseColor", 1.0f, 1.0f, 1.0f);
    imat->SetParam3f("ambientIntensity", .7f, .7f, .0f );

    // default Environment Texture is transparent
    scene.environmentTexture.setColor( Packet<4, float>(0, 0, 0, 0) );

    mDefaultAppearance = static_cast<Appearance*>(imat);
    imat->Release();
}

Scene::~Scene()
{
    for(AppMap::iterator it = mAppearances.begin(); it != mAppearances.end(); ++it)
    {
        Appearance *app = *it;
        app->mScene = 0;
    }
    for(AppMap::iterator it = mAppearances.begin(); it != mAppearances.end(); ++it)
    {
        Appearance *app = *it;
        app->mScene = 0;
    }
    for(IntMap<SceneObject*>::iterator it = mSceneObjects.begin(); it != mSceneObjects.end(); ++it)
    {
        (*it)->mScene = 0;
    }
}

//-----------------------------------------------------------------------------
// IScene Implementation
//-----------------------------------------------------------------------------

HRESULT Scene::CreateGeometry(IGeometry** _retval)
{
    Geometry *object;
    HRESULT rv = getHandle(scene.createGeometry(), &object);
    if(SUCCEEDED(rv))
        *_retval = object;
    return rv;
}

HRESULT Scene::GetGeometryCount(unsigned int *_retval)
{
    *_retval = static_cast<unsigned int>(scene.mGeometries.size());
    return S_OK;
}

HRESULT Scene::GetGeometry(unsigned int aIdx, IGeometry** _retval){
    if(aIdx >= scene.getGeometryCount() )
        return E_INVALIDARG;
    Geometry *object;
    HRESULT rv = getHandle(scene.getGeometry(aIdx), &object);
    if(SUCCEEDED(rv))
        *_retval = object;
    return rv;
}

HRESULT Scene::CreateLight(const char* aType, ILight** _retval)
{
    Light *light = 0;

    std::string type(aType);

    if(type == "PointLight")
    {
        light = new RTPointLight(this);

        light->setParam("attenuation", float3(1, 0, 0));
    }
    if(type == "SpotLight")
    {
        light = new RTSpotLight(this);

        light->setParam("attenuation", float3(1, 0, 0));
    }
    else if(type == "HemiCosineLight")
    {
        light = new RTHemiCosineLight(this);
    }
    else if(type == "DirectionalLight")
    {
        light = new RTDirectionalLight(this);
    }
    else if(type == "QuadLight")
    {
        light = new RTQuadLight(this);
    }

    if(light)
    {
        mLightsources.push_back(light);
        switchLightOn(light);
    }
    else
    {
        RTFACT_USER_WARNING_F("Unknown light source type: %1%", type);
        return E_FAIL;
    }
    light->AddRef();
    *_retval = light;
    return S_OK;
}

HRESULT Scene::GetLightCount(unsigned int *_retval)
{
    *_retval = static_cast<unsigned int>(mLightsources.size());
    return S_OK;
}

HRESULT Scene::GetLight(unsigned int aIdx, ILight** _retval){
    if(aIdx >= mLightsources.size() )
        return E_INVALIDARG;
    *_retval = *(mLightsources.begin() + aIdx);
    (*_retval)->AddRef();
    return S_OK;
}

HRESULT Scene::CreateAppearance(const char* aType, IAppearance** _retval)
{
    Appearance *app;
    std::string type(aType);

    if (type == "PhongShader" || type == "Phong")
    {
        app = new RTPhongShader<t_Context>(this);
    }
    else if (type == "PlasticShader")
    {
        app = new RTPlasticShader<t_Context>(this);
    }
    else if (type == "PhongGlass" || type == "XML3DGlassShader")
    {
        app = new RTGlassShader<t_Context>(this);

        /*app->setParam("diffuseColor", float3(0.3f, 0.3f, 0.3f));
        app->setParam("ambientIntensity", float3(0.5f, 0.5f, 0.5f));
        app->setParam("reflective", float3(0.9f, 0.9f, 0.9f));*/
    }
    else if (type == "EyeLightShader" || type == "EyeLight")
    {
        app = new RTPhongShader<t_Context>(this);
        app->setParam("receiveIllumination", false);
    }
    else if (type == "XML3DPhongShader")
    {
        app = new XML3DPhongShader<t_Context>(this);
    }
    else if (type == "XML3DPhongShaderGI")
    {
        app = new XML3DPhongShaderGI<t_Context>(this);
    }
    else if (type == "XML3DMatteShader")
    {
        app = new XML3DMatteShader<t_Context>(this);
    }
    else
    {
        app = 0;
    }

    if (app)
    {
        size_t appID = mAppearances.insert(app);
        app->setID(appID);

        Traits<t_Scene::AppearanceBase>::Ptr rtApp =
            boost::dynamic_pointer_cast<t_Scene::AppearanceBase>(
                app->getRTfactAppearance());

        if (!rtApp)
        {
            RTFACT_PROGRAM_ERROR("Incorrect appearance specified");
            return E_FAIL;
        }
        if(scene.appearances.size() == appID){
            scene.appearances.push_back(rtApp);
        }
        else{
            if(scene.appearances.size() < appID)
                scene.appearances.resize(appID + 1);

            // though this probably won't happen
            scene.appearances[appID] = rtApp;
        }

        app->setAppearanceUpdated();
    }
    else
    {
        RTFACT_LOG_WARNING(std::string("Unknown appearance with name: ")+type);
        return E_FAIL;
    }
    app->AddRef();
    *_retval = app;
    return S_OK;
}

HRESULT Scene::GetAppearanceCount(unsigned int *_retval)
{
    *_retval = static_cast<unsigned int>(mAppearances.size());
    return S_OK;
}

HRESULT Scene::GetAppearance(unsigned int aIdx, IAppearance** _retval){
    if(aIdx >= mAppearances.size() )
        return E_INVALIDARG;
    *_retval = *(mAppearances.begin() + aIdx);
    (*_retval)->AddRef();
    return S_OK;
}

HRESULT Scene::SetEnvironmentTexture(IImage2D* aImage)
{
    int compontentCount;
    aImage->GetComponentCount(&compontentCount);
    RTFACT_USER_ERROR_IF( compontentCount != 3, "Environment texture should be 3-float format");

    scene.environmentTexture.setImage(
        boost::static_pointer_cast<t_EnvImage>(
            static_cast<Image2D*>(aImage)->getRTfactImage()));
    return S_OK;
}

HRESULT Scene::SetEnvironmentColor(const float aRed, const float aGreen, const float aBlue, const float aAlpha)
{
    scene.environmentTexture.setColor( Packet<4, float>(aRed, aGreen, aBlue, aAlpha) );
    return S_OK;
}


//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------


void Scene::switchLightOn(
        Light *aLight)
{
    Traits<t_Scene::LightBase>::Ptr rtLight =
        boost::dynamic_pointer_cast<t_Scene::LightBase>(
            aLight->getRTfactLight()
            );
    if(!rtLight)
    {
        RTFACT_PROGRAM_ERROR("Incorrect light source handle specified");
    }

    //If the light is already present in the RTfact list, do nothing
    //This can happen, if switchLightOn is called multiple times without switching it off first
    for(t_Scene::LightVector::iterator it = scene.lights.begin();
        it != scene.lights.end(); ++it)
    {
        if(*it == rtLight)
        {
            return;
        }
    }

    scene.lights.push_back(rtLight);
}

void Scene::switchLightOff(
        Light *aLight)
{
    Traits<t_Scene::LightBase>::Ptr rtLight =
        boost::dynamic_pointer_cast<t_Scene::LightBase>(
            aLight->getRTfactLight()
            );

    for(t_Scene::LightVector::iterator it = scene.lights.begin();
        it != scene.lights.end(); ++it)
    {
        if(*it == rtLight)
        {
            scene.lights.erase(it);

            break;
        }
    }
}

void Scene::removeLight(
        Light *aLight)
{
    if(aLight)
    {
        switchLightOff(aLight);

        for(std::vector<Light*>::iterator it = mLightsources.begin();
            it != mLightsources.end(); ++it)
        {
            if(*it == aLight)
            {
                mLightsources.erase(it);

                break;
            }
        }
    }
}

void Scene::removeAppearance(
        Appearance *aAppearance)
{
    size_t appID = aAppearance->getID();
    scene.appearances[appID] = 0;
    mAppearances.erase(appID);
}

void Scene::notifyChange()
{
    ++mVersion;
    mModified = true;
}

unsigned int Scene::getVersion()
{
    return mVersion;
}


void Scene::update()
{
    if(mModified){
        mModified = false;
        for(AppMap::iterator it = mAppearances.begin(); it != mAppearances.end(); ++it)
        {
            Appearance *app = *it;
            if(app->hasNewAppearance())
            {
                Traits<t_Scene::AppearanceBase>::Ptr rtApp =
                            boost::dynamic_pointer_cast<t_Scene::AppearanceBase>(
                                    app->getRTfactAppearance());
                if (rtApp)
                {
                    scene.appearances[app->getID()] = rtApp;
                    app->setAppearanceUpdated();
                }
            }
        }
    }

}

//-----------------------------------------------------------------------------

HRESULT CreateScene(IScene** _retval)
{
    IScene* handle = new Scene();
    handle->AddRef();
    *_retval = handle;
    return S_OK;
}

//-----------------------------------------------------------------------------

RTFACT_RTPIE_NAMESPACE_END
