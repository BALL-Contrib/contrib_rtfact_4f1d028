/* 
 *  RTDirectionalLight.hpp
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
 *  Created on: 2009-09-30 14:15:34 +0200
 *  Author(s): Iliyan Georgiev, Dmitri Rubinstein, Felix Klein, Petr Kmoch
 */
 
 #ifndef RTFACT_REMOTE__RTDIRECTIONALLIGHT_HPP
#define RTFACT_REMOTE__RTDIRECTIONALLIGHT_HPP

#include <RTremote/Config/Common.hpp>

#include "RTPrivateLight.hpp"
#include <RTfact/Model/Light/DirectionalLight.hpp>
#include <RTfact/Model/LightAttenuation/OpenGLLightAttenuation.hpp>

#define RTFACT_REMOTE__DIRECTIONAL_LIGHT_DIRECTION    "direction"
#define RTFACT_REMOTE__DIRECTIONAL_LIGHT_INTENSITY    "intensity"
#define RTFACT_REMOTE__DIRECTIONAL_LIGHT_PARENT_GROUP "parentGroup"
#define RTFACT_REMOTE__DIRECTIONAL_LIGHT_CAST_SHADOW  "castShadow"

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTDirectionalLight :
    public RTPrivateLight
{
    friend class Renderer;

    typedef DirectionalLight t_Light;

protected:

    Traits<t_Light>::Ptr mLight;
    GroupHandle mGroup;
    Vec3f1 direction;

    RTDirectionalLight() :
        mLight(new t_Light), mGroup(0)
    {}

    virtual Traits<Light>::Ptr getRTfactLight()
    {
        return mLight;
    }

    virtual GroupHandle getParentGroup()
    {
        return mGroup;
    }

    virtual void updateTransformed(const RTfact::Transform& t)
    {
        mLight->setDirection(t.applyToVectorAffine(direction));
    }

public:

	RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    virtual bool setParam(
        const std::string &name, 
        const ParamValue &value)
    {
        if(name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_DIRECTION)
        {
            const float3& f = boost::get<float3>(value);
			direction = Vec3f1(f.x, f.y, f.z);
            mLight->setDirection( direction );
        }
        else if(name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_INTENSITY)
        {
            const float3& f = boost::get<float3>(value);

            mLight->setIntensity(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_PARENT_GROUP)
        {
            mGroup = boost::get<GroupHandle>(value);
        }
        else if (name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_CAST_SHADOW)
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
        if(name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_DIRECTION)
        {
            const Vec3f1& v = mLight->getDirection();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_INTENSITY)
        {
            const Vec3f1& v = mLight->getIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_PARENT_GROUP)
        {
            value = mGroup;
        }
        else if (name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_CAST_SHADOW)
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
            name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_DIRECTION ||
            name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_INTENSITY ||
            name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_PARENT_GROUP ||
	    name == RTFACT_REMOTE__DIRECTIONAL_LIGHT_CAST_SHADOW;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_REMOTE__DIRECTIONAL_LIGHT_DIRECTION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__DIRECTIONAL_LIGHT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__DIRECTIONAL_LIGHT_PARENT_GROUP,
            GROUPREF_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__DIRECTIONAL_LIGHT_CAST_SHADOW,
            BOOL_VALUE));
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__RTDIRECTIONALLIGHT_HPP
