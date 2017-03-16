/* 
 *  BasicTriangleSceneOBJXLoader.hpp
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
 
#ifndef RTFACT__BASICTRIANGLESCENEOBJXLOADER_HPP
#define RTFACT__BASICTRIANGLESCENEOBJXLOADER_HPP

#include <RTfact/Config/Common.hpp>

#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>

RTFACT_NAMESPACE_BEGIN

namespace IO {

template<
    class tLightCreator,
    class tEnvironmentTextureCreator>
class BasicTriangleSceneOBJXLoader
{
public:

    template<
        class tShadingInfoScene>
    static void load(
        tShadingInfoScene& aScene,
        const std::string &aPath,
        const Configuration& aConfig = Configuration())
    {
        std::ifstream inputStream(aPath.c_str());

        if(!inputStream)
        {
            RTFACT_USER_ERROR_F("Could not open file: %1%", aPath);

            return;
        }

        const bool warningsEnabled =
            aConfig.exists("warnings") && aConfig["warnings"].as<bool>();

        const std::string textureDir = getParentDirectory(aPath);

        size_t lineNum = 0;

        while(!inputStream.eof())
        {
            ++lineNum;

            std::string line, lineType;
            std::getline(inputStream, line);
            std::istringstream lineStream(line);

            lineStream >> std::ws >> lineType >> std::ws;

            if(lineType == "light" ||
               lineType == "environmentTexture")
            {
                Configuration::EntryGroup params;

                params.add("__LINE__", lineNum);
                params.add("__FILE__", aPath);
                params.add("baseDir", textureDir);
                params.add("warnings", warningsEnabled);

                while(!inputStream.eof())
                {
                    ++lineNum;

                    std::string paramsLine, paramsLineType, paramsLineValue;
                    std::getline(inputStream, paramsLine);
                    std::istringstream paramsLineStream(paramsLine);

                    paramsLineStream >> std::ws >> paramsLineType >> std::ws;

                    if(paramsLineType.size() == 0)
                    {
                        break;
                    }

                    std::getline(paramsLineStream, paramsLineValue);

                    boost::algorithm::trim(paramsLineValue);

                    if(paramsLineValue.size() == 0)
                    {
                        RTFACT_USER_WARNING_IF_F(warningsEnabled,
                            "%1%: unrecognized format at line: %2%",
                            aPath % lineNum);

                        continue;
                    }

                    params.add(paramsLineType, paramsLineValue);
                }

                if(lineType == "light")
                {
                    typename tShadingInfoScene::LightBase* light =
                        tLightCreator::create(params);

                    if(light)
                    {
                        aScene.lights.push_back(light);
                    }
                }
                else if(lineType == "environmentTexture")
                {
                    typename tShadingInfoScene::EnvironmentTexture* envTex =
                        tEnvironmentTextureCreator::create(params);

                    if(envTex)
                    {
                        aScene.environmentTexture = *envTex;
                    }
                }
            }
            else if(line[0] == '#' || line.size() == 0)
            {}
            else if(warningsEnabled)
            {
                RTFACT_USER_WARNING_F(
                    "%1%: unrecognized format at line: %2%",
                    aPath % lineNum);
            }
        }
    }
};

} // namespace IO

RTFACT_NAMESPACE_END

#endif // RTFACT__BASICTRIANGLESCENEOBJXLOADER_HPP
