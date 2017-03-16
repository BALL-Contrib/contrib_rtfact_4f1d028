/* 
 *  OBJXLightCreator.hpp
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
 *  Author(s): Mike Phillips, Iliyan Georgiev
 */
 
 #ifndef RTFACT__OBJXLIGHTCREATOR_HPP
#define RTFACT__OBJXLIGHTCREATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/Light/PointLight.hpp>
#include <RTfact/Model/Light/QuadLight.hpp>
#include <RTfact/Model/Light/SpotLight.hpp>
#include <RTfact/Model/Light/DirectionalLight.hpp>
#include <RTfact/Model/Light/EnvironmentLight.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>

#include <RTfact/Model/LightAttenuation/NoLightAttenuation.hpp>
#include <RTfact/Model/LightAttenuation/ConstantLightAttenuation.hpp>
#include <RTfact/Model/LightAttenuation/LinearLightAttenuation.hpp>
#include <RTfact/Model/LightAttenuation/QuadraticLightAttenuation.hpp>
#include <RTfact/Model/LightAttenuation/OpenGLLightAttenuation.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tLightBase>
class OBJXLightCreator
{
public:

    static tLightBase* create(
        const IO::Configuration::EntryGroup& aParams)
    {
        const bool warningsEnabled = aParams["warnings"].as<bool>();

        if(!aParams.exists("type"))
        {
            RTFACT_USER_WARNING_IF_F(warningsEnabled,
                "%1% (%2%): No light source type specified",
                aParams["__FILE__"] % aParams["__LINE__"]);

            return RTFACT_NULL;
        }

        if(!aParams.exists("intensity"))
        {
            RTFACT_USER_WARNING_IF_F(warningsEnabled,
                "%1% (%2%): No point light source intensity specified",
                aParams["__FILE__"] % aParams["__LINE__"]);

            return RTFACT_NULL;
        }

        const Vec3f1 intensity = aParams["intensity"].as<Vec3f1>();

        const Vec3f1 attenuation = aParams.exists("attenuation") ?
            aParams["attenuation"].as<Vec3f1>() :
            Vec3f1(0.f, 0.f, 1.f);

        const bool castsShadows =
            aParams.exists("shadows") ?
                !(aParams["shadows"] == "off") :
                true;

        tLightBase* light = RTFACT_NULL;

        if(aParams["type"] == "point")
        {
            if(!aParams.exists("position"))
            {
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No point light source position specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }

            const Vec3f1 position = aParams["position"].as<Vec3f1>();
            
            if(attenuation == Vec3f1::C_0_0_0() ||
               attenuation == Vec3f1::C_1_0_0)
            {
                light = new PointLight<NoLightAttenuation>(position, intensity);
            }
            else if(attenuation.y == 0.f &&
                    attenuation.z == 0.f)
            {
                light = new PointLight<ConstantLightAttenuation>(position, intensity);
            }
            else if(attenuation.x == 0.f &&
                    attenuation.z == 0.f)
            {
                light = new PointLight<LinearLightAttenuation>(position, intensity);
            }
            else if(attenuation.x == 0.f &&
                    attenuation.y == 0.f)
            {
                light = new PointLight<QuadraticLightAttenuation>(position, intensity);
            }
            else
            {
                light = new PointLight<OpenGLLightAttenuation>(
                    position, intensity,
                    OpenGLLightAttenuation(attenuation.x, attenuation.y, attenuation.z));
            }
        }


		else if(aParams["type"] == "spot")
        {
            if(!aParams.exists("position"))
            {
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No spot light source position specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }

			if(!aParams.exists("direction"))
			{
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No spot light source direction specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }

			if(!aParams.exists("angle"))
			{
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No spot light source angle specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }

            const Vec3f1 position = aParams["position"].as<Vec3f1>();
			const Vec3f1 direction = aParams["direction"].as<Vec3f1>();
			const float angle = aParams["angle"].as<float>();
			float cutoff = aParams["cutoff"].as<float>();


			if(!aParams.exists("cutoff"))
			{
                cutoff = angle;
            }
            
            if(attenuation == Vec3f1::C_0_0_0() ||
               attenuation == Vec3f1::C_1_0_0)
            {
                light = new SpotLight<NoLightAttenuation>(position, intensity, direction, angle, cutoff);
            }
            else if(attenuation.y == 0.f &&
                    attenuation.z == 0.f)
            {
                light = new SpotLight<ConstantLightAttenuation>(position, intensity, direction, angle, cutoff);
            }
            else if(attenuation.x == 0.f &&
                    attenuation.z == 0.f)
            {
                light = new SpotLight<LinearLightAttenuation>(position, intensity, direction, angle, cutoff);
            }
            else if(attenuation.x == 0.f &&
                    attenuation.y == 0.f)
            {
                light = new SpotLight<QuadraticLightAttenuation>(position, intensity, direction, angle, cutoff);
            }
            else
            {
                light = new SpotLight<OpenGLLightAttenuation>(
                    position, intensity, direction, angle, cutoff,
                    OpenGLLightAttenuation(attenuation.x, attenuation.y, attenuation.z));
            }
        }


		/*defines a quad type area light source*/
		else if(aParams["type"] == "quad")
        {
			/*Only 3 vertices need to be specified.  The fourth is assumed implicitly.
			   Winding rules for polygons apply (CCW winding is forward facing)*/
            if(!aParams.exists("vertex0"))
            {
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No point light source position specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }
			if(!aParams.exists("vertex1"))
            {
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No point light source position specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }
			if(!aParams.exists("vertex2"))
            {
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No point light source position specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }

			
            const Vec3f1 vertex0 = aParams["vertex0"].as<Vec3f1>();
			const Vec3f1 vertex1 = aParams["vertex1"].as<Vec3f1>();
			const Vec3f1 vertex2 = aParams["vertex2"].as<Vec3f1>();
            
            if(attenuation == Vec3f1::C_0_0_0() ||
               attenuation == Vec3f1::C_1_0_0)
            {
                light = new QuadLight<NoLightAttenuation>(vertex0, vertex1, vertex2, intensity);
            }
            else if(attenuation.y == 0.f &&
                    attenuation.z == 0.f)
            {
                light = new QuadLight<ConstantLightAttenuation>(vertex0, vertex1, vertex2, intensity);
            }
            else if(attenuation.x == 0.f &&
                    attenuation.z == 0.f)
            {
                light = new QuadLight<LinearLightAttenuation>(vertex0, vertex1, vertex2, intensity);
            }
            else if(attenuation.x == 0.f &&
                    attenuation.y == 0.f)
            {
                light = new QuadLight<QuadraticLightAttenuation>(vertex0, vertex1, vertex2, intensity);
            }
            else
            {
                light = new QuadLight<OpenGLLightAttenuation>(
                    vertex0, vertex1, vertex2, intensity,
                    OpenGLLightAttenuation(attenuation.x, attenuation.y, attenuation.z));
            }
        }
        else if(aParams["type"] == "directional")
        {
            if(!aParams.exists("direction"))
            {
                RTFACT_USER_WARNING_IF_F(warningsEnabled,
                    "%1% (%2%): No light source direction specified",
                    aParams["__FILE__"] % aParams["__LINE__"]);

                return RTFACT_NULL;
            }

            light = new DirectionalLight(aParams["direction"].as<Vec3f1>(), intensity);
        }
        else if (aParams["type"] == "environment")
        {
            if (aParams.exists("file") && 
                aParams.exists("CDFScaleX") &&
                aParams.exists("CDFScaleY"))
            {
                light = new EnvironmentLight<BasicImage2D<float> >(intensity,
                    IO::Loader<BasicImage2D<float> >::load(
                    IO::appendPath(aParams["baseDir"], aParams["file"])),
                    aParams["CDFScaleX"].as<float>(),
                    aParams["CDFScaleY"].as<float>());
            }
            else
            {
                light = new EnvironmentLight<BasicImage2D<float> >(intensity);
            }
        }
        else
        {
            RTFACT_USER_WARNING_IF_F(warningsEnabled,
                "%1% (%2%): Unrecognized light source type: %3%",
                aParams["__FILE__"] % aParams["__LINE__"] % aParams["type"]);

            return RTFACT_NULL;
        }

        light->setCastsShadows(castsShadows);

        return light;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__OBJXLIGHTCREATOR_HPP
