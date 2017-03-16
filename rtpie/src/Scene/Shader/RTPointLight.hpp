/* 
 *  RTPointLight.hpp
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
 
 #ifndef RTFACT_RTPIE__RTPOINTLIGHT_HPP
#define RTFACT_RTPIE__RTPOINTLIGHT_HPP

#include <RTpie/Config/Common.hpp>

#include "Scene/Light.hpp"

#include <RTfact/Model/Light/PointLight.hpp>
#include <RTfact/Model/LightAttenuation/OpenGLLightAttenuation.hpp>

#define RTFACT_RTPIE__POINT_LIGHT_POSITION     "position"
#define RTFACT_RTPIE__POINT_LIGHT_INTENSITY    "intensity"
#define RTFACT_RTPIE__POINT_LIGHT_ATTENUATION  "attenuation"
#define RTFACT_RTPIE__POINT_LIGHT_CAST_SHADOW  "castShadow"

RTFACT_RTPIE_NAMESPACE_BEGIN

class RTPointLight :
    public Light
{
    friend class Scene;
    friend class Renderer;

    typedef PointLight<OpenGLLightAttenuation> t_Light;

protected:

    Traits<t_Light>::Ptr mLight;
	Vec3f1 position;

    RTPointLight(Scene *aScene) :
        Light(aScene),
        mLight(new t_Light)
    {}

    virtual Traits<RTfact::Light>::Ptr getRTfactLight()
    {
        return mLight;
    }

    virtual void updateTransformed(const RTfact::Transform &t)
    {
		    mLight->setPosition(t.applyToPointAffine(position));
    }

public:

	RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        if(!mScene)
            return false;
        mScene->notifyChange();

        if(name == RTFACT_RTPIE__POINT_LIGHT_POSITION)
        {
            const float3& f = boost::get<float3>(value);
			      position = Vec3f1(f.x, f.y, f.z);
            mLight->setPosition(position);
        }
        else if(name == RTFACT_RTPIE__POINT_LIGHT_INTENSITY)
        {
            const float3& f = boost::get<float3>(value);

            mLight->setIntensity(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__POINT_LIGHT_ATTENUATION)
        {
            const float3& f = boost::get<float3>(value);

            mLight->setAttenuation(OpenGLLightAttenuation(f.x, f.y, f.z));
        }
        else if (name == RTFACT_RTPIE__POINT_LIGHT_CAST_SHADOW)
        {
            const bool *b = boost::get<bool>(&value);
            if (b)
                mLight->setCastsShadows(*b);
            else
            {
                const int *i = boost::get<int>(&value);
                if (i)
                    mLight->setCastsShadows(*i!=0);
                else
                    return false;
            }
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
        if(name == RTFACT_RTPIE__POINT_LIGHT_POSITION)
        {
            const Vec3f1& v = mLight->getPosition();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__POINT_LIGHT_INTENSITY)
        {
            const Vec3f1& v = mLight->getIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__POINT_LIGHT_ATTENUATION)
        {
            const OpenGLLightAttenuation& v = mLight->getAttenuation();

            value = float3(v.getConstantFactor(), v.getLinearFactor(), v.getQuadraticFactor());
        }
        else if (name == RTFACT_RTPIE__POINT_LIGHT_CAST_SHADOW)
        {
            value = mLight->getCastsShadows();
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
            name == RTFACT_RTPIE__POINT_LIGHT_POSITION ||
            name == RTFACT_RTPIE__POINT_LIGHT_INTENSITY ||
            name == RTFACT_RTPIE__POINT_LIGHT_ATTENUATION ||
            name == RTFACT_RTPIE__POINT_LIGHT_CAST_SHADOW;
    }

    virtual HRESULT GetType(const char** _retval){
        *_retval = "PointLight";
        return S_OK;
    }

    /*
    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_RTPIE__POINT_LIGHT_POSITION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__POINT_LIGHT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__POINT_LIGHT_ATTENUATION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__POINT_LIGHT_CAST_SHADOW,
            BOOL_VALUE));
    }
    */
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__RTPOINTLIGHT_HPP
