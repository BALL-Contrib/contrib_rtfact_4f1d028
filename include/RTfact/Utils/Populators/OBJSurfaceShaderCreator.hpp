/* 
 *  OBJSurfaceShaderCreator.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2009-02-10 00:37:36 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__OBJSURFACESHADERCREATOR_HPP
#define RTFACT__OBJSURFACESHADERCREATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/HashMap.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>
#include <RTfact/Utils/IO/Loader.hpp>

#include <RTfact/Concept/Image/Image2D.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>

#include <RTfact/Model/Texture/Image2DTexture.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>

#include <RTfact/Model/SurfaceShader/PhongShader.hpp>
#include <RTfact/Model/SurfaceShader/GlassShader.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tSurfaceShaderBase>
class OBJSurfaceShaderCreator
{
    typedef typename tSurfaceShaderBase::Context t_Context;

    typedef BasicImage2D<float>              t_Image;

    typedef Image2DTexture<
        t_Image, BilinearFilterImageSampler> t_Texture;

    typedef HashMap<std::string, t_Image*>   t_ImageMap;

    t_ImageMap mImageMap;

    template<
        class tImage,
        class tTexture,
        class tShader>
    tShader* createPhongShader(
        const IO::Configuration::EntryGroup& aParams)
    {
        tShader* shader = new tShader();

        if(aParams.exists("map_Kd"))
        {
            const std::string texturePath =
                IO::appendPath(aParams["textureDir"], aParams["map_Kd"]);

            tImage* image;

            typename t_ImageMap::const_iterator imageIt =
                mImageMap.find(aParams["map_Kd"]);

            if(imageIt != mImageMap.end())
            {
                image = imageIt->second;
            }
            else
            {
                image = IO::Loader<tImage>::load(texturePath);

                mImageMap.insert(
                    typename t_ImageMap::value_type(aParams["map_Kd"], image));
            }

            shader->setDiffuseTexture(tTexture(image));
        }

        if(aParams.exists("Ka"))
        {
            shader->setAmbientIntensity(aParams["Ka"].as<Vec3f1>());
        }

        if(aParams.exists("Kd"))
        {
            shader->setDiffuseColor(aParams["Kd"].as<Vec3f1>());
        }

        if(aParams.exists("Ks"))
        {
            shader->setSpecularColor(aParams["Ks"].as<Vec3f1>());
        }

        if(aParams.exists("Ns"))
        {
            shader->setShininess(aParams["Ns"].as<float>());
        }

        if(aParams.exists("Refl"))
        {
            shader->setReflectanceColor(aParams["Refl"].as<Vec3f1>());
        }

        if(aParams.exists("Kr"))
        {
            shader->setReflectanceColor(aParams["Kr"].as<Vec3f1>());
        }

        if(aParams.exists("Tr"))
        {
            shader->setTransparency(1-aParams["Tr"].as<float>());
        }

        return shader;
    }

public:

    tSurfaceShaderBase* create(
        const IO::Configuration::EntryGroup& aParams)
    {
        if(aParams.exists("type") && aParams["type"] == "glass")
        {
            return new GlassShader<t_Context>;
        }
        else
        {
            return createPhongShader<
                t_Image, t_Texture, PhongShader<t_Context, t_Texture> >(
                    aParams);
        }
    }

};

RTFACT_NAMESPACE_END

#endif // RTFACT__OBJSURFACESHADERCREATOR_HPP
