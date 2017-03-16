/* 
 *  OBJMaterialCreator.hpp
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
 
 #ifndef RTFACT__OBJMATERIALCREATOR_HPP
#define RTFACT__OBJMATERIALCREATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/HashMap.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>
#include <RTfact/Utils/IO/Loader.hpp>

#include <RTfact/Concept/Image/Image2D.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>

#include <RTfact/Model/Texture/Image2DTexture.hpp>
#include <RTfact/Model/Texture/Float3ConstantTexture.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>

#include <RTfact/Model/Material/LambertianMaterial.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tMaterialBase>
class OBJMaterialCreator
{
    typedef HashMap<std::string, Image2D*> t_ImageMap;

    t_ImageMap mImageMap;

public:

    tMaterialBase* create(
        const IO::Configuration::EntryGroup& aParams)
    {
        tMaterialBase* material = RTFACT_NULL;

        Float3ConstantTexture ambTex(
            aParams.exists("Ka") ?
                aParams["Ka"].as<Vec3f1>() :
                Vec3f1::C_0_0_0());

        if(aParams.exists("map_Kd"))
        {
            typedef BasicImage2D<float> t_Image;
            typedef Image2DTexture<t_Image, BilinearFilterImageSampler> t_Texture;

            t_Image* image;

            typename t_ImageMap::iterator imageIt =
                mImageMap.find(aParams["map_Kd"]);

            if(imageIt == mImageMap.end())
            {
                std::string texturePath =
                    IO::appendPath(aParams["textureDir"], aParams["map_Kd"]);

                image = IO::Loader<t_Image>::load(texturePath);

                mImageMap.insert(t_ImageMap::value_type(aParams["map_Kd"], image));
            }
            else
            {
                image = static_cast<t_Image*>(imageIt->second);
            }

            material = new LambertianMaterial<t_Texture, Float3ConstantTexture>(
                t_Texture(image), ambTex);
        }
        else
        {
            Float3ConstantTexture diffTex(
                aParams.exists("Kd") ?
                    aParams["Kd"].as<Vec3f1>() :
                    Vec3f1::C_0_0_0());

            material = new LambertianMaterial<Float3ConstantTexture, Float3ConstantTexture>(
                diffTex, ambTex);
        }

        return material;
    }

};

RTFACT_NAMESPACE_END

#endif // RTFACT__OBJMATERIALCREATOR_HPP
