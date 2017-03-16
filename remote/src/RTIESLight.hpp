#ifndef RTFACT_REMOTE__RTIESLIGHT_HPP
#define RTFACT_REMOTE__RTIESLIGHT_HPP

#include <RTremote/Config/Common.hpp>

#include "RTPrivateLight.hpp"

#include <RTfact/Model/Light/IESLight.hpp>
#include <RTfact/Model/LightAttenuation/OpenGLLightAttenuation.hpp>

#define RTFACT_REMOTE__IES_LIGHT_POSITION        "position"
#define RTFACT_REMOTE__IES_LIGHT_DIRECTION       "direction"
#define RTFACT_REMOTE__IES_LIGHT_BASE_VECTOR     "baseVector"
#define RTFACT_REMOTE__IES_LIGHT_OFFSET_ANGLE    "offsetAngle"
#define RTFACT_REMOTE__IES_LIGHT_INTENSITY       "intensity"
#define RTFACT_REMOTE__IES_LIGHT_COLOR           "color"
#define RTFACT_REMOTE__IES_LIGHT_ATTENUATION     "attenuation"
#define RTFACT_REMOTE__IES_LIGHT_PARENT_GROUP    "parentGroup"
#define RTFACT_REMOTE__IES_LIGHT_CAST_SHADOW     "castShadow"
#define RTFACT_REMOTE__IES_LIGHT_FILE            "file"

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTIESLight :
    public RTPrivateLight
{
    friend class Renderer;

    typedef IESLight<OpenGLLightAttenuation> t_Light;

protected:

    Traits<t_Light>::Ptr mLight;
    GroupHandle mGroup;
    Vec3f1 position;
    Vec3f1 direction;
    Vec3f1 baseVector;
    Vec3f1 color;

    RTIESLight() :
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

    virtual void updateTransformed(const RTfact::Transform &t)
    {
        mLight->setPosition(t.applyToPointAffine(position));
        mLight->setDirection(t.applyToVectorAffine(direction));
        mLight->setBaseVector(t.applyToVectorAffine(baseVector));
    }

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    virtual bool isInitialized () { return mLight->isInitialized(); }

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        if(name == RTFACT_REMOTE__IES_LIGHT_POSITION)
        {
            const float3& f = boost::get<float3>(value);
            position = Vec3f1(f.x, f.y, f.z);
            mLight->setPosition(position);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_DIRECTION)
        {
            const float3& f = boost::get<float3>(value);
            direction = Vec3f1(f.x, f.y, f.z);
            mLight->setDirection(direction);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_BASE_VECTOR)
        {
            const float3& f = boost::get<float3>(value);
            baseVector = Vec3f1(f.x, f.y, f.z);
            mLight->setBaseVector(baseVector);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_OFFSET_ANGLE)
        {
            const float& f = boost::get<float>(value);
            mLight->setOffsetAngle(f);
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_INTENSITY)
        {
            const float& f = boost::get<float>(value);
            mLight->setIntensity(f);
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_COLOR)
        {
            const float3& f = boost::get<float3>(value);
            color = Vec3f1(f.x, f.y, f.z);
            mLight->setColor(color);
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_ATTENUATION)
        {
            const float3& f = boost::get<float3>(value);

            mLight->setAttenuation(OpenGLLightAttenuation(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_PARENT_GROUP)
        {
            mGroup = boost::get<GroupHandle>(value);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_CAST_SHADOW)
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
        else if (name == RTFACT_REMOTE__IES_LIGHT_FILE)
        {
            const std::string f = boost::get<std::string>(value);
            if(!mLight->setFile(f))
              return false;
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
        if(name == RTFACT_REMOTE__IES_LIGHT_POSITION)
        {
            const Vec3f1& v = mLight->getPosition();

            value = float3(v.x, v.y, v.z);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_DIRECTION)
        {
            const Vec3f1& v = mLight->getDirection();

            value = float3(v.x, v.y, v.z);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_BASE_VECTOR)
        {
            const Vec3f1& v = mLight->getBaseVector();

            value = float3(v.x, v.y, v.z);
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_OFFSET_ANGLE)
        {
            value = mLight->getOffsetAngle();
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_INTENSITY)
        {
            value = mLight->getIntensity();
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_COLOR)
        {
            const Vec3f1& v = mLight->getColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_ATTENUATION)
        {
            const OpenGLLightAttenuation& v = mLight->getAttenuation();

            value = float3(v.getConstantFactor(), v.getLinearFactor(), v.getQuadraticFactor());
        }
        else if(name == RTFACT_REMOTE__IES_LIGHT_PARENT_GROUP)
        {
            value = mGroup;
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_CAST_SHADOW)
        {
            value = mLight->getCastsShadows();
        }
        else if (name == RTFACT_REMOTE__IES_LIGHT_FILE)
        {
            value = mLight->getFile();
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
            name == RTFACT_REMOTE__IES_LIGHT_POSITION ||
            name == RTFACT_REMOTE__IES_LIGHT_DIRECTION ||
            name == RTFACT_REMOTE__IES_LIGHT_BASE_VECTOR ||
            name == RTFACT_REMOTE__IES_LIGHT_OFFSET_ANGLE ||
            name == RTFACT_REMOTE__IES_LIGHT_INTENSITY ||
            name == RTFACT_REMOTE__IES_LIGHT_COLOR ||
            name == RTFACT_REMOTE__IES_LIGHT_ATTENUATION ||
            name == RTFACT_REMOTE__IES_LIGHT_PARENT_GROUP ||
            name == RTFACT_REMOTE__IES_LIGHT_CAST_SHADOW ||
            name == RTFACT_REMOTE__IES_LIGHT_FILE;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_POSITION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_DIRECTION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_BASE_VECTOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_OFFSET_ANGLE,
            FLOAT_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_ATTENUATION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_PARENT_GROUP,
            GROUPREF_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_CAST_SHADOW,
            BOOL_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__IES_LIGHT_FILE,
            STRING_VALUE));
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__RTIESLIGHT_HPP
