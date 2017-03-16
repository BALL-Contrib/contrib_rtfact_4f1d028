/*
 *  XML3DMatteShader.hpp
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
 *  Created on: 2010-10-13 16:41:18 +0200
 *  Author(s): Felix Klein, Kristian Sons
 */

 #ifndef RTFACT_RTPIE__XML3DMATTESHADER_HPP
#define RTFACT_RTPIE__XML3DMATTESHADER_HPP

#include <RTpie/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "Scene/Appearance.hpp"
#include "Data/Image2D.hpp"

#include <RTfact/Model/SurfaceShader/XML3DMatteShader.hpp>
#include <RTfact/Model/Texture/Image2DTexture.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>

#define RTFACT_RTPIE__MATTE_PARAM_COLOR     "color"
#define RTFACT_RTPIE__MATTE_PARAM_TEXTURE   "texture"

RTFACT_RTPIE_NAMESPACE_BEGIN

template<
    class tContext>
class XML3DMatteShader :
    public Appearance
{
    friend class Scene;
    friend class Renderer;

protected:

    typedef BasicImage2D<unsigned char>                              t_Image;
    typedef Image2DTexture<t_Image, BilinearFilterImageSampler>      t_Texture;
    typedef RTfact::XML3DMatteShader<tContext, t_Texture>            t_Shader;

    typename Traits<t_Shader>::Ptr mShader;
    bool mHasNewAppearance;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    XML3DMatteShader(
        Scene* aScene
    ) :
        Appearance(aScene),
        mShader(new t_Shader ),
        mHasNewAppearance(true)
    {}

    XML3DMatteShader(
        const XML3DMatteShader&);

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

        if(name == RTFACT_RTPIE__MATTE_PARAM_TEXTURE)
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

            mShader->getTexture().setImage(image);
        }
        else if(name == RTFACT_RTPIE__MATTE_PARAM_COLOR)
        {
            const float3& f = boost::get<float3>(value);

            mShader->setColor(Vec3f1(f.x, f.y, f.z));
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
        if(name == RTFACT_RTPIE__MATTE_PARAM_COLOR)
        {
            const Vec3f1& v = mShader->getColor();

            value = float3(v.x, v.y, v.z);
        }
        else if(name == RTFACT_RTPIE__MATTE_PARAM_TEXTURE)
        {
            value = new Image2D(mShader->getTexture().getImage());
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
            name == RTFACT_RTPIE__MATTE_PARAM_COLOR ||
            name == RTFACT_RTPIE__MATTE_PARAM_TEXTURE;
    }


    virtual HRESULT GetType(const char** _retval){
        *_retval = "XML3DMatteShader";
        return S_OK;
    }

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__XML3DMATTESHADER_HPP
