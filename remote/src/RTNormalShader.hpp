/* 
 *  RTNormalShader.hpp
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
 *  Created on: 2009-12-23 17:40:49 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
#ifndef RTFACT_REMOTE__NORMALSHADER_HPP
#define RTFACT_REMOTE__NORMALSHADER_HPP

#include <RTremote/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "RTPrivateAppearance.hpp"
#include "RTImageImpl.hpp"

#include <RTfact/Model/SurfaceShader/NormalShader.hpp>


RTFACT_REMOTE_NAMESPACE_BEGIN

template<
    class tContext>
class RTNormalShader :
    public RTPrivateAppearance
{
    friend class Renderer;

protected:

    typedef NormalShader<tContext> t_Shader;

    typename Traits<t_Shader>::Ptr mShader;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    RTNormalShader(
        const uint aID
    ) :
        RTPrivateAppearance(aID),
        mShader(new t_Shader)
    {}

    RTNormalShader(
        const RTNormalShader&);

public:

    virtual bool hasNewAppearance(){
        return false;
    }

    virtual void setAppearanceUpdated()
    {}

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        return false;
    }

    virtual bool getParam(
        const std::string &name,
        ParamValue &value)
    {
        return false;
    }

    virtual bool hasParam(
        const std::string &name)
    {
        return false;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__PHONGSHADERSL_HPP
