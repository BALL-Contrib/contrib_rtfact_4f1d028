/* 
 *  Scene.hpp
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
 
 #ifndef RTFACT_RTPIE__SCENE_HPP
#define RTFACT_RTPIE__SCENE_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Scene/IScene.hpp>

RTFACT_RTPIE_FORWARD(Scene)

#include <vector>
#include <memory>

#include "Base/Base.hpp"
#include "Base/SceneObject.hpp"
#include "Scene/Appearance.hpp"
#include "Scene/Light.hpp"

#define RTFACT_RTPIE__DEFAULT_RESX 512
#define RTFACT_RTPIE__DEFAULT_RESY 512

RTFACT_RTPIE_NAMESPACE_BEGIN


class Scene : public Base, public IScene
{
    friend class Renderer;

public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IScene

    virtual HRESULT CreateGeometry(IGeometry** _retval);

    virtual HRESULT GetGeometryCount(unsigned int *_retval);

    virtual HRESULT GetGeometry(unsigned int aIdx, IGeometry** _retval);

    virtual HRESULT CreateLight(const char* aType, ILight** _retval);

    virtual HRESULT GetLightCount(unsigned int *_retval);

    virtual HRESULT GetLight(unsigned int aIdx, ILight** _retval);

    virtual HRESULT CreateAppearance(const char* aType, IAppearance** _retval);

    virtual HRESULT GetAppearanceCount(unsigned int *_retval);

    virtual HRESULT GetAppearance(unsigned int aIdx, IAppearance** _retval);

    virtual HRESULT SetEnvironmentTexture(IImage2D* aImage);

    virtual HRESULT SetEnvironmentColor(const float aRed, const float aGreen, const float aBlue, const float aAlpha);



    // Internal

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    /**
     * Constructor.
     */
    Scene();


    /**
     * Destructor
     */
    virtual ~Scene();


    //-----------------------------------------------------------------------------
    // API helpers
    //-----------------------------------------------------------------------------

    template<class T1, class T2>
    HRESULT getHandle(T2 aInternalGeometry, T1 **_retval)
    {
        size_t index = aInternalGeometry->mExID;
        T1* handle;
        if(index != static_cast<size_t>(-1))
            handle = static_cast<T1*>(mSceneObjects[index]);
        else
        {
            handle = new T1(this, aInternalGeometry);
            aInternalGeometry->mExID = mSceneObjects.insert(handle);
        }
        handle->AddRef();
        *_retval = handle;
        return S_OK;
    }

    template<class T1>
    void clearHandle(T1 aInternalGeometry)
    {
        size_t index = aInternalGeometry->mExID;
        aInternalGeometry->mExID = static_cast<size_t>(-1);
        mSceneObjects.erase(index);
        notifyChange();
    }

    void switchLightOn(Light *aLight);
    void switchLightOff(Light *aLight);
    void removeLight(Light *aLight);
    void removeAppearance(Appearance *aAppearance);

    void notifyChange();
    unsigned int getVersion();

    void update();

private:


    //---------------------------------------------------------------------
    // API Data
    //---------------------------------------------------------------------

    typedef IntMap<Appearance*>     AppMap;

    AppMap                          mAppearances;
    std::vector<Light*>             mLightsources;
    // This map is used to prevent returning duplicated API Geometries
    IntMap<SceneObject*>            mSceneObjects;
    AppearancePtr                   mDefaultAppearance;

    unsigned int                    mVersion;
    bool                            mModified;


    //---------------------------------------------------------------------
    // RTfact Data
    //---------------------------------------------------------------------

public:

    // Scene
    t_Scene scene;

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__RENDERER_HPP
