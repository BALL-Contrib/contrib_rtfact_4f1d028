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
 *  Created on: 2009-12-23 17:40:49 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
#ifndef RTFACT_REMOTE__PLASTICSHADER_HPP
#define RTFACT_REMOTE__PLASTICSHADER_HPP

#include <RTremote/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "RTPrivateAppearance.hpp"
#include "RTImageImpl.hpp"

#include <RTfact/Model/SurfaceShader/PlasticShader.hpp>

#define RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR     "diffuseColor"
#define RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR    "specularColor"
#define RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY "ambientIntensity"
#define RTFACT_REMOTE__PHONG_PARAM_SHININESS         "shininess"

RTFACT_REMOTE_NAMESPACE_BEGIN

template<
    class tContext>
class RTPlasticShader :
    public RTPrivateAppearance
{
    friend class Renderer;

protected:

    typedef PlasticShader<tContext> t_Shader;

    typename Traits<t_Shader>::Ptr mShader;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    RTPlasticShader(
        const uint aID
    ) :
        RTPrivateAppearance(aID),
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
        if(name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setDiffuseColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setSpecularColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY)
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
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SHININESS)
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
        if(name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const Vec3f1& v = mShader->getDiffuseColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const Vec3f1& v = mShader->getSpecularColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY)
        {
            const Vec3f1& v = mShader->getAmbientIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SHININESS)
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
            name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR ||
            name == RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR ||
            name == RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY ||
            name == RTFACT_REMOTE__PHONG_PARAM_SHININESS;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_SHININESS,
            FLOAT_VALUE));
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__PHONGSHADERSL_HPP
