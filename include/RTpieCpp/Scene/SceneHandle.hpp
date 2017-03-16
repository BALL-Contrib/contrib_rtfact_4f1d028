/*
 *  SceneHandle.hpp
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

 #ifndef RTFACT_HANDLE_SCENE_HPP
#define RTFACT_HANDLE_SCENE_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Scene/IScene.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IScene
WRAPPER_FWD(SceneHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/SceneContentHandles.hpp>
#include <RTpieCpp/Scene/LightHandle.hpp>
#include <RTpieCpp/Scene/AppearanceHandle.hpp>
#include <RTpieCpp/Data/Image2DHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IScene
WRAPPER_START(SceneHandle, BaseHandle)

    GeometryHandle createGeometry()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IGeometry*, CreateGeometry)

    unsigned int getGeometryCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetGeometryCount)

    GeometryHandle getGeometry(unsigned int aIdx)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::IGeometry*, GetGeometry, aIdx)

    LightHandle createLight(const char* aType)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::ILight*, CreateLight, aType)

    unsigned int getLightCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetLightCount)

    LightHandle getLight(unsigned int aIdx)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::ILight*, GetLight, aIdx)

    AppearanceHandle createAppearance(const char* aType)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::IAppearance*, CreateAppearance, aType)

    unsigned int getAppearanceCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetAppearanceCount)

    AppearanceHandle getAppearance(unsigned int aIdx)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::IAppearance*, GetAppearance, aIdx)

    void setEnvironmentTexture(Image2DHandle aImage)
    IMPLEMENT_WRAPPER_NO_RETURN( SetEnvironmentTexture(aImage.get()) )

    void setEnvironmentColor(const float aRed, const float aGreen, const float aBlue, const float aAlpha)
    IMPLEMENT_WRAPPER_NO_RETURN( SetEnvironmentColor(aRed, aGreen, aBlue, aAlpha) )

WRAPPER_END
#undef IMPL_INTERFACE


RTFACT_HANDLE_NAMESPACE_BEGIN

inline SceneHandle CreateSceneHandle()
{
    RTfact::RTpie::IScene *result;
    RTfact::RTpie::CreateScene(&result);
    return result;
}

RTFACT_HANDLE_NAMESPACE_END


#endif // RTFACT_HANDLE_SCENE_HPP
