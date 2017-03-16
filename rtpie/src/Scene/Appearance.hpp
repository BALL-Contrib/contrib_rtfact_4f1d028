/* 
 *  Appearance.hpp
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
 
 #ifndef RTFACT_RTPIE__APPEARANCE_HPP
#define RTFACT_RTPIE__APPEARANCE_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Scene/IAppearance.hpp>

RTFACT_RTPIE_FORWARD(Appearance)

#include <Base/Parameterizable.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

class Appearance :
    public ParameterizableSceneObject, public IAppearance
{
public:
    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IParameterizable

    RTFACT_RTPIE_FORWARD_PARAMETERIZABLE

    virtual HRESULT BeginParams(){ return S_OK;}

    virtual HRESULT EndParams(){ return S_OK;}

    // IAppearance

    virtual HRESULT GetType(const char **_retval) = 0;

    // Internal

    Appearance(Scene *aScene
    ) :
        ParameterizableSceneObject(aScene)
    {}

    virtual ~Appearance() {}

protected:

    virtual void preDeletion();

public:

    size_t mID;

    size_t getID() const
    {
        return mID;
    }

    void setID(size_t aID)
    {
        mID = aID;
    }

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance() = 0;

    virtual bool hasNewAppearance(){
        return false;
    }

    virtual void setAppearanceUpdated()
    {}

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__APPEARANCE_HPP
