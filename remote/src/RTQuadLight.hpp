#ifndef RTFACT_REMOTE__RTQuadLight_HPP
#define RTFACT_REMOTE__RTQuadLight_HPP

#include <RTremote/Config/Common.hpp>

#include "RTPrivateLight.hpp"

#include <RTfact/Model/Light/QuadLight.hpp>
#include <RTfact/Model/LightAttenuation/OpenGLLightAttenuation.hpp>

#define RTFACT_REMOTE__QUAD_LIGHT_VERTEX0        "vertex0"
#define RTFACT_REMOTE__QUAD_LIGHT_VERTEX1        "vertex1"
#define RTFACT_REMOTE__QUAD_LIGHT_VERTEX2        "vertex2"
#define RTFACT_REMOTE__QUAD_LIGHT_INVERSE        "inverse"
#define RTFACT_REMOTE__QUAD_LIGHT_INTENSITY      "intensity"
#define RTFACT_REMOTE__QUAD_LIGHT_ATTENUATION    "attenuation"
#define RTFACT_REMOTE__QUAD_LIGHT_PARENT_GROUP   "parentGroup"
#define RTFACT_REMOTE__QUAD_LIGHT_CAST_SHADOW    "castShadow"

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTQuadLight :
    public RTPrivateLight
{
    friend class Renderer;

    typedef QuadLight<OpenGLLightAttenuation> t_Light;

protected:

    Traits<t_Light>::Ptr mLight;
    GroupHandle mGroup;
    Vec3f1 vertex0;
    Vec3f1 vertex1;
    Vec3f1 vertex2;
    bool invert;
    Vec3f1 intensity;
    Vec3f1 attenuation;

    RTQuadLight() :
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
        mLight->setVertex0(t.applyToPointAffine(vertex0));
        mLight->setVertex1(t.applyToPointAffine(vertex1));
        mLight->setVertex2(t.applyToPointAffine(vertex2));
    }

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        if(name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX0)
        {
            const float3& f = boost::get<float3>(value);
            vertex0 = Vec3f1(f.x, f.y, f.z);
            mLight->setVertex0(vertex0);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX1)
        {
            const float3& f = boost::get<float3>(value);
            vertex1 = Vec3f1(f.x, f.y, f.z);
            mLight->setVertex1(vertex1);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX2)
        {
            const float3& f = boost::get<float3>(value);
            vertex2 = Vec3f1(f.x, f.y, f.z);
            mLight->setVertex2(vertex2);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_INVERSE)
        {
            const bool& f = boost::get<bool>(value);
            mLight->setInverse(f);
        }
        else if (name == RTFACT_REMOTE__QUAD_LIGHT_INTENSITY)
        {
            const float3& f = boost::get<float3>(value);
            intensity = Vec3f1(f.x, f.y, f.z);
            mLight->setIntensity(intensity);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_ATTENUATION)
        {
            const float3& f = boost::get<float3>(value);

            mLight->setAttenuation(OpenGLLightAttenuation(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_PARENT_GROUP)
        {
            mGroup = boost::get<GroupHandle>(value);
        }
        else if (name == RTFACT_REMOTE__QUAD_LIGHT_CAST_SHADOW)
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
        if (name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX0)
        {
            const Vec3f1& v = mLight->getVertex0();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX1)
        {
            const Vec3f1& v = mLight->getVertex1();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX2)
        {
            const Vec3f1& v = mLight->getVertex2();

            value = float3(v.x, v.y, v.z);
        }
        else if (name == RTFACT_REMOTE__QUAD_LIGHT_INVERSE)
        {
            value = mLight->getInverse();
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_INTENSITY)
        {
            const Vec3f1& v = mLight->getIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_ATTENUATION)
        {
            const OpenGLLightAttenuation& v = mLight->getAttenuation();

            value = float3(v.getConstantFactor(), v.getLinearFactor(), v.getQuadraticFactor());
        }
        else if(name == RTFACT_REMOTE__QUAD_LIGHT_PARENT_GROUP)
        {
            value = mGroup;
        }
        else if (name == RTFACT_REMOTE__QUAD_LIGHT_CAST_SHADOW)
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
            name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX0 ||
            name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX1 ||
            name == RTFACT_REMOTE__QUAD_LIGHT_VERTEX2 ||
            name == RTFACT_REMOTE__QUAD_LIGHT_INVERSE ||
            name == RTFACT_REMOTE__QUAD_LIGHT_ATTENUATION ||
            name == RTFACT_REMOTE__QUAD_LIGHT_INTENSITY ||
            name == RTFACT_REMOTE__QUAD_LIGHT_PARENT_GROUP ||
            name == RTFACT_REMOTE__QUAD_LIGHT_CAST_SHADOW;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_VERTEX0,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_VERTEX1,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_VERTEX2,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_INVERSE,
            BOOL_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_ATTENUATION,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_INTENSITY,
            FLOAT_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_PARENT_GROUP,
            GROUPREF_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__QUAD_LIGHT_CAST_SHADOW,
            BOOL_VALUE));
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__RTQuadLight_HPP
