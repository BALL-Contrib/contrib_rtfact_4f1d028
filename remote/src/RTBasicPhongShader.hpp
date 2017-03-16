/* 
 *  RTBasicPhongShader.hpp
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
 *  Author(s): Iliyan Georgiev, Felix Klein, Dmitri Rubinstein, Lukas Marsalek
 */
 
 #ifndef RTFACT_REMOTE__BASICPHONGSHADER_HPP
#define RTFACT_REMOTE__BASICPHONGSHADER_HPP

#include <RTremote/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "RTPrivateAppearance.hpp"
#include "RTImageImpl.hpp"

#include <RTfact/Model/SurfaceShader/BasicPhongShader.hpp>
#include <RTfact/Model/Texture/Image2DTexture.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>

#define RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR     "diffuseColor"
#define RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_TEXTURE   "diffuseTexture"
#define RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR    "specularColor"
#define RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY "ambientIntensity"
#define RTFACT_REMOTE__PHONG_PARAM_SHININESS         "shininess"
#define RTFACT_REMOTE__PHONG_PARAM_USE_VERTEX_COLOR  "useVertexColor"
#define RTFACT_REMOTE__PHONG_PARAM_RECEIVE_ILLUMINATION  "receiveIllumination"
#define RTFACT_REMOTE__PHONG_PARAM_SHADOW_SAMPLES    "numShadowSamples"

RTFACT_REMOTE_NAMESPACE_BEGIN

template<
    class tContext>
class RTBasicPhongShader :
    public RTPrivateAppearance
{
    friend class Renderer;

protected:

    typedef BasicImage2D<float>                                      t_ImageF;
    typedef Image2DTexture<t_ImageF, BilinearFilterImageSampler>     t_TextureF;
    typedef BasicPhongShader<tContext, t_TextureF>                        t_ShaderF;

    typedef BasicImage2D<unsigned char>                              t_ImageB;
    typedef Image2DTexture<t_ImageB, BilinearFilterImageSampler>     t_TextureB;
    typedef BasicPhongShader<tContext, t_TextureB>                        t_ShaderB;

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

    RTBasicPhongShader(
        const uint aID
    ) :
        RTPrivateAppearance(aID),
        mShaderF(new t_ShaderF),
        mShaderB(0),
        mHasNewAppearance(true)
    {}

    RTBasicPhongShader(
        const RTBasicPhongShader&);

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
        if(name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_TEXTURE)
        {
            if(boost::get<RTImageHandle>(value)->getComponentType() !=
                RTImage::COMPONENT_FLOAT &&
                boost::get<RTImageHandle>(value)->getComponentType() !=
                                RTImage::COMPONENT_BYTE)
            {
                return false;
            }

            if(boost::get<RTImageHandle>(value)->getComponentType() ==
                    RTImage::COMPONENT_BYTE)
            {
                if(mShaderF)
                {
                    mShaderB = new t_ShaderB;
                    copyShaderParams(mShaderF, mShaderB);
                    mShaderF = 0;
                    mHasNewAppearance = true;
                }

                Traits<t_ImageB>::Ptr image = boost::static_pointer_cast<t_ImageB>(
                    boost::polymorphic_cast<RTImageImpl*>(
                        boost::get<RTImageHandle>(value).get())->getRTfactImage()
                        );

                mShaderB->getDiffuseTexture().setImage(image);

            } else if(boost::get<RTImageHandle>(value)->getComponentType()
                    == RTImage::COMPONENT_FLOAT)
            {
                if(mShaderB){
                    mShaderF = new t_ShaderF;
                    copyShaderParams(mShaderB, mShaderF);
                    mShaderB = 0;
                    mHasNewAppearance = true;
                }

                Traits<t_ImageF>::Ptr image = boost::static_pointer_cast<t_ImageF>(
                    boost::polymorphic_cast<RTImageImpl*>(
                        boost::get<RTImageHandle>(value).get())->getRTfactImage()
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
        if(name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            aShader->setDiffuseColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            aShader->setSpecularColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY)
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
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SHININESS)
        {
            aShader->setShininess(boost::get<float>(value));
        }
		else if(name == RTFACT_REMOTE__PHONG_PARAM_USE_VERTEX_COLOR)
		{
			aShader->setUseVertexColor(boost::get<bool>(value));
		}
		else if(name == RTFACT_REMOTE__PHONG_PARAM_RECEIVE_ILLUMINATION)
		{
			aShader->setReceiveIllumination(boost::get<bool>(value));
		}
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SHADOW_SAMPLES)
        {
            aShader->setNumShadowSamples(boost::get<int>(value));
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
        if(name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR)
        {
            const Vec3f1& v = aShader->getDiffuseColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_TEXTURE)
        {
            value = RTImageHandle(new RTImageImpl(aShader->getDiffuseTexture().getImage()));
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR)
        {
            const Vec3f1& v = aShader->getSpecularColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY)
        {
            const Vec3f1& v = aShader->getAmbientIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_REMOTE__PHONG_PARAM_SHININESS)
        {
            value = aShader->getShininess();
        }
		else if(name == RTFACT_REMOTE__PHONG_PARAM_USE_VERTEX_COLOR)
		{
			value = static_cast<bool>(aShader->getUseVertexColor());
		}
		else if(name == RTFACT_REMOTE__PHONG_PARAM_RECEIVE_ILLUMINATION)
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
        aDest->setAmbientIntensity(aSrc->getAmbientIntensity());
        aDest->setShininess(aSrc->getShininess());
		aDest->setUseVertexColor(aSrc->getUseVertexColor());
		aDest->setReceiveIllumination(aSrc->getReceiveIllumination());
    }

    virtual bool hasParam(
        const std::string &name)
    {
        return
            name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR ||
            name == RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_TEXTURE ||
            name == RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR ||
            name == RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY ||
            name == RTFACT_REMOTE__PHONG_PARAM_SHININESS ||
			name == RTFACT_REMOTE__PHONG_PARAM_USE_VERTEX_COLOR ||
			name == RTFACT_REMOTE__PHONG_PARAM_RECEIVE_ILLUMINATION;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_DIFFUSE_TEXTURE,
            IMAGEREF_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_SPECULAR_COLOR,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_AMBIENT_INTENSITY,
            FLOAT3_VALUE));

        infos.push_back(ParamInfo(
            RTFACT_REMOTE__PHONG_PARAM_SHININESS,
            FLOAT_VALUE));

		infos.push_back(ParamInfo(
			RTFACT_REMOTE__PHONG_PARAM_USE_VERTEX_COLOR,
			BOOL_VALUE));

		infos.push_back(ParamInfo(
			RTFACT_REMOTE__PHONG_PARAM_RECEIVE_ILLUMINATION,
			BOOL_VALUE));
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__BASICPHONGSHADER_HPP
