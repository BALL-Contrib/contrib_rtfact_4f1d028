/* 
 *  RTGlassShader.hpp
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
 
 #ifndef RTFACT_RTPIE__GLASSSHADER_HPP
#define RTFACT_RTPIE__GLASSSHADER_HPP

#include <RTpie/Config/Common.hpp>

#include "Scene/Appearance.hpp"
#include "Data/Image2D.hpp"

#include <RTfact/Model/SurfaceShader/GlassShader.hpp>

#define RTFACT_RTPIE__PHONG_PARAM_NORMAL_RELFECTANCE "normalReflectance"
#define RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH          "maxDepth"

RTFACT_RTPIE_NAMESPACE_BEGIN

template<
    class tContext>
class RTGlassShader :
    public Appearance
{
    friend class Scene;
    friend class Renderer;

protected:

    typedef GlassShader<tContext> t_Shader;

    typename Traits<t_Shader>::Ptr mShader;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    RTGlassShader( Scene *aScene
    ) :
        Appearance(aScene),
        mShader(new t_Shader)
    {
        mShader->setMaxDepth(6);
    }

    RTGlassShader(
        const RTGlassShader&);

public:

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        if(!mScene)
            return false;
        mScene->notifyChange();

        if(name == RTFACT_RTPIE__PHONG_PARAM_NORMAL_RELFECTANCE)
        {
            mShader->setNormalReflectance(boost::get<float>(value));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH)
        {
            mShader->setMaxDepth(boost::get<uint>(value));
        }
        else
        {
            return false;
        }

        return true;
    }

    virtual bool getParam(
        const std::string &name,
        ParamValue &value)
    {
        if(name == RTFACT_RTPIE__PHONG_PARAM_NORMAL_RELFECTANCE)
        {
            value = mShader->getNormalReflectance();
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH)
        {
            value = (int) mShader->getMaxDepth();
        }
        else
        {
            return false;
        }

        return true;
    }

    virtual bool hasParam(
        const std::string &name)
    {
        return
            name == RTFACT_RTPIE__PHONG_PARAM_NORMAL_RELFECTANCE ||
            name == RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH;
    }

    virtual HRESULT GetType(const char** _retval){
        *_retval = "GlassShader";
        return S_OK;
    }

    /*
    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_NORMAL_RELFECTANCE,
            INT_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_ALPHA,
            FLOAT_VALUE));
    }
    */
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__GLASSSHADER_HPP
