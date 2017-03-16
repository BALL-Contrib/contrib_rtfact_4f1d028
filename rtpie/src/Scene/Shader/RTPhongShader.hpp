/* 
 *  RTPhongShader.hpp
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
 
 #ifndef RTFACT_RTPIE__PHONGSHADER_HPP
#define RTFACT_RTPIE__PHONGSHADER_HPP

#include <RTpie/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "Scene/Appearance.hpp"
#include "Data/Image2D.hpp"

#include <RTfact/Model/SurfaceShader/PhongShader.hpp>
#include <RTfact/Model/Texture/Image2DTexture.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>

#define RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR     "diffuseColor"
#define RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_TEXTURE   "diffuseTexture"
#define RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR    "specularColor"
#define RTFACT_RTPIE__PHONG_PARAM_EMISSIVE_COLOR    "emissiveColor"
#define RTFACT_RTPIE__PHONG_PARAM_REFLECTIVE_COLOR  "reflective"
#define RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY "ambientIntensity"
#define RTFACT_RTPIE__PHONG_PARAM_SHININESS         "shininess"
#define RTFACT_RTPIE__PHONG_PARAM_TRANSPARENCY      "transparency"
#define RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH         "maxDepth"
#define RTFACT_RTPIE__PHONG_PARAM_USE_VERTEX_COLOR  "useVertexColor"
#define RTFACT_RTPIE__PHONG_PARAM_RECEIVE_ILLUMINATION "receiveIllumination"

RTFACT_RTPIE_NAMESPACE_BEGIN

template<
    class tContext>
class RTPhongShader :
    public Appearance
{
    friend class Scene;
    friend class Renderer;

protected:

    typedef BasicImage2D<float>                                      t_ImageF;
    typedef Image2DTexture<t_ImageF, BilinearFilterImageSampler>     t_TextureF;
    typedef PhongShader<tContext, t_TextureF>                        t_ShaderF;

    typedef BasicImage2D<unsigned char>                              t_ImageB;
    typedef Image2DTexture<t_ImageB, BilinearFilterImageSampler>     t_TextureB;
    typedef PhongShader<tContext, t_TextureB>                        t_ShaderB;

    typename Traits<t_ShaderF>::Ptr mShaderF;
    typename Traits<t_ShaderB>::Ptr mShaderB;
    bool mHasNewAppearance;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        if(mShaderB)
            return mShaderB;
        else
            return mShaderF;
    }

    RTPhongShader(
        Scene* aScene
    ) :
        Appearance(aScene),
        mShaderF(new t_ShaderF),
        mShaderB(0),
        mHasNewAppearance(true)
    {}

    RTPhongShader(
        const RTPhongShader&);

public:

    virtual bool hasNewAppearance(){
        return mHasNewAppearance;
    }

    virtual void setAppearanceUpdated()
    {
        mHasNewAppearance = false;
    }

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        if(!mScene)
            return false;
        mScene->notifyChange();

        if(name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_TEXTURE)
        {
            IImage2D::ComponentType componentType;
            boost::get<IImage2D*>(value)->GetComponentType(&componentType);
            if(componentType != IImage2D::COMPONENT_FLOAT &&
               componentType != IImage2D::COMPONENT_BYTE)
            {
                return false;
            }

            if(componentType == IImage2D::COMPONENT_BYTE)
            {
                if(mShaderF)
                {
                    mShaderB = new t_ShaderB;
                    copyShaderParams(mShaderF, mShaderB);
                    mShaderF = 0;
                    mHasNewAppearance = true;
                }

                Traits<t_ImageB>::Ptr image = boost::static_pointer_cast<t_ImageB>(
                        static_cast<Image2D*>(
                        boost::get<IImage2D*>(value))->getRTfactImage()
                        );

                mShaderB->getDiffuseTexture().setImage(image);

            } else if(componentType == IImage2D::COMPONENT_FLOAT)
            {
                if(mShaderB){
                    mShaderF = new t_ShaderF;
                    copyShaderParams(mShaderB, mShaderF);
                    mShaderB = 0;
                    mHasNewAppearance = true;
                }

                Traits<t_ImageF>::Ptr image = boost::static_pointer_cast<t_ImageF>(
                    static_cast<Image2D*>(
                        boost::get<IImage2D*>(value))->getRTfactImage()
                        );
                mShaderF->getDiffuseTexture().setImage(image);
            }
        }
        else if(mShaderF)
            return setParamIntern(name, value, mShaderF);
        else
            return setParamIntern(name, value, mShaderB);

        return false;
    }

    template <class tShaderPtr>
    bool setParamIntern(const std::string &name,
        const ParamValue &value,
        tShaderPtr aShader)
    {
        if(name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            aShader->setDiffuseColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            aShader->setSpecularColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_EMISSIVE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            aShader->setEmissiveColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_REFLECTIVE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            aShader->setReflectanceColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY)
        {
            const float *f = boost::get<float>(&value);
            if (f)
                aShader->setAmbientIntensity(Vec3f1(*f, *f, *f));
            else
            {
                const float3 *f3 = boost::get<float3>(&value);

                if (f3)
                    aShader->setAmbientIntensity(Vec3f1(f3->x, f3->y, f3->z));
                else
                    return false;
            }
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SHININESS)
        {
            aShader->setShininess(boost::get<float>(value));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_TRANSPARENCY)
        {
            aShader->setTransparency(boost::get<float>(value));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH)
        {
            aShader->setMaxDepth(boost::get<int>(value));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_USE_VERTEX_COLOR)
        {
            aShader->setUseVertexColor(boost::get<bool>(value));
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_RECEIVE_ILLUMINATION)
        {
            aShader->setReceiveIllumination(boost::get<bool>(value));
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
        if(mShaderF)
            return getParamIntern(name, value, mShaderF);
        else
            return getParamIntern(name, value, mShaderB);
    }

    template <class tShaderPtr>
    bool getParamIntern(const std::string &name,
        ParamValue &value,
        tShaderPtr aShader)
    {
        if(name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const Vec3f1& v = aShader->getDiffuseColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_TEXTURE)
        {
            value = new Image2D(aShader->getDiffuseTexture().getImage());
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const Vec3f1& v = aShader->getSpecularColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_EMISSIVE_COLOR)
        {
            const Vec3f1& v = aShader->getEmissiveColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_REFLECTIVE_COLOR)
        {
            const Vec3f1& v = aShader->getReflectanceColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY)
        {
            const Vec3f1& v = aShader->getAmbientIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_SHININESS)
        {
            value = aShader->getShininess();
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_TRANSPARENCY)
        {
            value = aShader->getTransparency();
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH)
        {
            value = static_cast<int>(aShader->getMaxDepth());
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_USE_VERTEX_COLOR)
        {
            value = static_cast<bool>(aShader->getUseVertexColor());
        }
        else if(name == RTFACT_RTPIE__PHONG_PARAM_RECEIVE_ILLUMINATION)
        {
            value = static_cast<bool>(aShader->getReceiveIllumination());
        }
        else
        {
            return false;
        }

        return true;
    }
    template <class tShaderPtrA, class tShaderPtrB>
    void copyShaderParams(
            tShaderPtrA aSrc,
            tShaderPtrB aDest)
    {
        aDest->setDiffuseColor(aSrc->getDiffuseColor());
        aDest->setSpecularColor(aSrc->getSpecularColor());
        aDest->setEmissiveColor(aSrc->getEmissiveColor());
        aDest->setReflectanceColor(aSrc->getReflectanceColor());
        aDest->setAmbientIntensity(aSrc->getAmbientIntensity());
        aDest->setShininess(aSrc->getShininess());
        aDest->setTransparency(aSrc->getTransparency());
        aDest->setMaxDepth(aSrc->getMaxDepth());
        aDest->setUseVertexColor(aSrc->getUseVertexColor());
        aDest->setReceiveIllumination(aSrc->getReceiveIllumination());
    }

    virtual bool hasParam(
        const std::string &name)
    {
        return
            name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_TEXTURE ||
            name == RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_EMISSIVE_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_REFLECTIVE_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY ||
            name == RTFACT_RTPIE__PHONG_PARAM_SHININESS ||
            name == RTFACT_RTPIE__PHONG_PARAM_TRANSPARENCY ||
            name == RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH ||
            name == RTFACT_RTPIE__PHONG_PARAM_USE_VERTEX_COLOR ||
            name == RTFACT_RTPIE__PHONG_PARAM_RECEIVE_ILLUMINATION;
    }


    virtual HRESULT GetType(const char** _retval){
        *_retval = "PhongShader";
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
            RTFACT_RTPIE__PHONG_PARAM_DIFFUSE_TEXTURE,
            IMAGEREF_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_SPECULAR_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_EMISSIVE_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_REFLECTIVE_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_AMBIENT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_SHININESS,
            FLOAT_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_TRANSPARENCY,
            FLOAT_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_MAX_DEPTH,
            INT_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_USE_VERTEX_COLOR,
            BOOL_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_RTPIE__PHONG_PARAM_RECEIVE_ILLUMINATION,
            BOOL_VALUE));
    }
    */
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__PHONGSHADER_HPP
