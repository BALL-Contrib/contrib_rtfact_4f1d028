/* 
 *  Light.hpp
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
 
 #ifndef RTFACT_RTPIE__LIGHTSOURCE_HPP
#define RTFACT_RTPIE__LIGHTSOURCE_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Scene/ILight.hpp>

RTFACT_RTPIE_FORWARD(Scene)

#include <Base/Parameterizable.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

class Light :
    public ParameterizableSceneObject, public ILight
{
    friend class Scene;
    friend class Renderer;

public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IParameterizable

    RTFACT_RTPIE_FORWARD_PARAMETERIZABLE

    virtual HRESULT BeginParams(){ return S_OK;}

    virtual HRESULT EndParams(){ return S_OK;}

    // ILight

    virtual HRESULT SwitchOn();

    virtual HRESULT SwitchOff();

    virtual HRESULT GetType(const char **_retval) = 0;

    // Internal

protected:

    virtual Traits<RTfact::Light>::Ptr getRTfactLight() = 0;

    virtual void updateTransformed(const RTfact::Transform&) = 0;

public:

    Light(Scene *aScene) : ParameterizableSceneObject(aScene){};

    virtual ~Light() {}

protected:

    virtual void preDeletion();



};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__LIGHTSOURCE_HPP
