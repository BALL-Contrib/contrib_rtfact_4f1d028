/*
 *  XML3DPhongShaderGI.hpp
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
 *  Created on: 2011-06-17 10:28:18 +0200
 *  Author(s): Felix Klein, Kristian Sons, Vaclav Krajicek
 */

 #ifndef RTFACT_RTPIE__XML3DPHONGSHADERGI_HPP
#define RTFACT_RTPIE__XML3DPHONGSHADERGI_HPP

#include <RTpie/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "Scene/Appearance.hpp"
#include "Data/Image2D.hpp"

#include <RTfact/Model/SurfaceShader/XML3DPhongShaderGI.hpp>
#include <RTfact/Model/Texture/Image2DTexture.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>

#define RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_COLOR         "diffuseColor"
#define RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_TEXTURE       "diffuseTexture"
#define RTFACT_RTPIE__PHONGGI_PARAM_SPECULAR_COLOR        "specularColor"
#define RTFACT_RTPIE__PHONGGI_PARAM_EMISSIVE_COLOR        "emissiveColor"
#define RTFACT_RTPIE__PHONGGI_PARAM_REFLECTIVE_COLOR      "reflective"
#define RTFACT_RTPIE__PHONGGI_PARAM_AMBIENT_INTENSITY     "ambientIntensity"
#define RTFACT_RTPIE__PHONGGI_PARAM_SHININESS             "shininess"
#define RTFACT_RTPIE__PHONGGI_PARAM_TRANSPARENCY          "transparency"
#define RTFACT_RTPIE__PHONGGI_PARAM_USE_VERTEX_COLOR      "useVertexColor"
#define RTFACT_RTPIE__PHONGGI_PARAM_RECEIVE_ILLUMINATION  "receiveIllumination"

RTFACT_RTPIE_NAMESPACE_BEGIN

template<
    class tContext>
class XML3DPhongShaderGI :
    public Appearance
{
    friend class Scene;
    friend class Renderer;

protected:

    typedef BasicImage2D<unsigned char>                              t_Image;
    typedef Image2DTexture<t_Image, BilinearFilterImageSampler>      t_Texture;
    typedef RTfact::XML3DPhongShaderGI<tContext, t_Texture>          t_Shader;

    typename Traits<t_Shader>::Ptr mShader;
    bool mHasNewAppearance;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    XML3DPhongShaderGI(
        Scene* aScene
    ) :
        Appearance(aScene),
        mShader(new t_Shader ),
        mHasNewAppearance(true)
    {}

    XML3DPhongShaderGI(
        const XML3DPhongShaderGI&);

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

        if(name == RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_TEXTURE)
        {
            IImage2D::ComponentType componentType;
            boost::get<IImage2D*>(value)->GetComponentType(&componentType);
            if(componentType != IImage2D::COMPONENT_BYTE)
            {
                return false;
            }

            Traits<t_Image>::Ptr image = boost::static_pointer_cast<t_Image>(
                    static_cast<Image2D*>(
                    boost::get<IImage2D*>(value))->getRTfactImage()
                    );

            mShader->getDiffuseTexture().setImage(image);
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setDiffuseColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_SPECULAR_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setSpecularColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_EMISSIVE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setEmissiveColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_REFLECTIVE_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setReflectanceColor(Vec3f1(f.x, f.y, f.z));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_AMBIENT_INTENSITY)
        {
            const float *f = boost::get<float>(&value);
            if (f)
                mShader->setAmbientIntensity(Vec3f1(*f, *f, *f));
            else
            {
                const float3 *f3 = boost::get<float3>(&value);

                if (f3)
                    mShader->setAmbientIntensity(Vec3f1(f3->x, f3->y, f3->z));
                else
                    return false;
            }
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_SHININESS)
        {
            mShader->setShininess(boost::get<float>(value));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_TRANSPARENCY)
        {
            mShader->setTransparency(boost::get<float>(value));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_USE_VERTEX_COLOR)
        {
            mShader->setUseVertexColor(boost::get<bool>(value));
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_RECEIVE_ILLUMINATION)
        {
            mShader->setReceiveIllumination(boost::get<bool>(value));
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
        if(name == RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_COLOR)
        {
            const Vec3f1& v = mShader->getDiffuseColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_TEXTURE)
        {
            value = new Image2D(mShader->getDiffuseTexture().getImage());
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_SPECULAR_COLOR)
        {
            const Vec3f1& v = mShader->getSpecularColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_EMISSIVE_COLOR)
        {
            const Vec3f1& v = mShader->getEmissiveColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_REFLECTIVE_COLOR)
        {
            const Vec3f1& v = mShader->getReflectanceColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_AMBIENT_INTENSITY)
        {
            const Vec3f1& v = mShader->getAmbientIntensity();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_SHININESS)
        {
            value = mShader->getShininess();
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_TRANSPARENCY)
        {
            value = mShader->getTransparency();
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_USE_VERTEX_COLOR)
        {
            value = static_cast<bool>(mShader->getUseVertexColor());
        }
        else if(name == RTFACT_RTPIE__PHONGGI_PARAM_RECEIVE_ILLUMINATION)
        {
            value = static_cast<bool>(mShader->getReceiveIllumination());
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
            name == RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_COLOR ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_DIFFUSE_TEXTURE ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_SPECULAR_COLOR ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_EMISSIVE_COLOR ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_REFLECTIVE_COLOR ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_AMBIENT_INTENSITY ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_SHININESS ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_TRANSPARENCY ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_USE_VERTEX_COLOR ||
            name == RTFACT_RTPIE__PHONGGI_PARAM_RECEIVE_ILLUMINATION;
    }

    virtual HRESULT GetType(const char** _retval){
        *_retval = "XML3DPhongShaderGI";
        return S_OK;
    }

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__XML3DPHONGSHADERGI_HPP
