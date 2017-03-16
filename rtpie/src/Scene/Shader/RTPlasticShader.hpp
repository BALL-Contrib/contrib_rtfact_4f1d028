/* 
 *  RTPlasticShader.hpp
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
 
 #ifndef RTFACT_RTPIE__PHONGSHADERSL_HPP
#define RTFACT_RTPIE__PHONGSHADERSL_HPP

#include <RTpie/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "Scene/Appearance.hpp"
#include "Data/Image2D.hpp"

#include <RTfact/Model/SurfaceShader/PlasticShader.hpp>

#define RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR     "diffuseColor"
#define RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR    "specularColor"
#define RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY "ambientIntensity"
#define RTFACT_RTPIE__PHONG_PARAM_SHININESS         "shininess"

RTFACT_RTPIE_NAMESPACE_BEGIN

template<
    class tContext>
class RTPlasticShader :
    public Appearance
{
    friend class Scene;
    friend class Renderer;

protected:

    typedef PlasticShader<tContext> t_Shader;

    typename Traits<t_Shader>::Ptr mShader;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    RTPlasticShader(
        Scene *aScene
    ) :
        Appearance(aScene),
        mShader(new t_Shader)
    {}

    RTPlasticShader(
        const RTPlasticShader&);

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
        if(!mScene)
            return false;
        mScene->notifyChange();

        if(name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setDiffuseColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setSpecularColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY)
        {
            const float *f = boost::get<float>(&value);

            if(f)
            {
               mShader->setAmbientIntensity(Vec3f1(*f, *f, *f));
            }
            else
            {
                const float3 *f3 = boost::get<float3>(&value);

                if(f3)
                {
                    mShader->setAmbientIntensity(Vec3f1(f3->x, f3->y, f3->z));
                }
                else
                {
                    return false;
                }
            }
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SHININESS)
        {
            mShader->setShininess(boost::get<float>(value));
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
        if(name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const Vec3f1& v = mShader->getDiffuseColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const Vec3f1& v = mShader->getSpecularColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY)
        {
            const Vec3f1& v = mShader->getAmbientIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SHININESS)
        {
            value = mShader->getShininess();
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
            name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY ||
            name == RTFACT_RTPIE__PHONG_PARAM_SHININESS;
    }

    virtual HRESULT GetType(const char** _retval){
        *_retval = "PlasticShader";
        return S_OK;
    }

    /*
    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_SHININESS,
            FLOAT_VALUE));
    }
    */
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__PHONGSHADERSL_HPP
