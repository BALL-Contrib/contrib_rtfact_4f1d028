/* 
 *  TriangleMeshOBJLoader.hpp
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
 *  Created on: 2009-02-02 02:21:22 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__TRIANGLEMESHOBJLOADER_HPP
#define RTFACT__TRIANGLEMESHOBJLOADER_HPP

#include <RTfact/Config/Common.hpp>

#include <locale>
#include <stdlib.h>
#include <string.h>

#if defined(RTFACT_OS_POSIX) || defined(RTFACT_OS_MACOS)
#   include <libgen.h>
#   define _strnicmp strncasecmp
#endif

#include <sstream>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include <RTfact/Utils/IO/Loader.hpp>

RTFACT_NAMESPACE_BEGIN

namespace IO {

template<
    class tMesh,
    class tMeshPopulator>
class TriangleMeshOBJLoader :
    public EntityFileLoader<tMesh>
{
public:

    typedef typename EntityFileLoader<tMesh>::Entity Entity;

private:

    static void skipWhiteSpace(const char * &aStr)
    {
        while(isspace(*aStr))
        {
            ++aStr;
        }
    }

    static std::string trimRight(const char* aStr)
    {
        size_t len = strlen(aStr);
        const char *firstChar = aStr;
        const char *lastChar = firstChar + len - 1;

        while(lastChar >= firstChar && isspace(*lastChar))
        {
            --lastChar;
        }

        return std::string(firstChar, lastChar + 1);
    }

    static void loadMTL(
        tMeshPopulator& aMeshPopulator,
        const std::string& aDirectory,
        const std::string& aFileName,
        const Configuration& aConfig = Configuration())
    {
        std::string path = appendPath(aDirectory, aFileName);
        std::ifstream fsMtl(path.c_str());

        if(!fsMtl)
        {
            RTFACT_USER_WARNING_F("Could not open material file: %1%", path);

            return;
        }

        std::string textureDir = aDirectory;

        if(aConfig.exists("textureDir"))
        {
            textureDir = appendPath(textureDir, aConfig["textureDir"]);
        }

        Configuration::EntryGroup appearanceParams;

        uint lineNum = 0;
        bool appearanceCreated = false;

        while(!fsMtl.eof())
        {
            lineNum ++;

            std::string curLine, lineType, lineValue;
            std::getline(fsMtl, curLine);
            std::istringstream issLine(curLine);

            issLine >> std::ws >> lineType >> std::ws;
            std::getline(issLine, lineValue);

            lineValue = boost::algorithm::trim_right_copy(lineValue);

            if(lineType == "newmtl")
            {
                if(appearanceCreated)
                {
                    aMeshPopulator.addAppearance(appearanceParams);
                }
                else
                {
                    appearanceCreated = true;
                }

                appearanceParams = Configuration::EntryGroup(lineValue);
                appearanceParams.add("textureDir", textureDir);
            }
            else if(lineType[0] == '#' || lineType.size() == 0)
            {}
            else
            {
                appearanceParams.add(lineType, lineValue);
            }
        }

        if(appearanceCreated)
        {
            aMeshPopulator.addAppearance(appearanceParams);
        }
    }

public:

    Entity* load(
        const std::string &aPath,
        const Configuration& aConfig = Configuration())
    {
        Entity* mesh = new Entity;

        tMeshPopulator meshPopulator(mesh);

        size_t lineNum = 0;

        std::ifstream inputStream(aPath.c_str());

        if(!inputStream)
        {
            RTFACT_USER_ERROR_F("Could not open file: %1%", aPath);

            return 0;
        }

        const bool warningsEnabled =
            aConfig.exists("warnings") && aConfig["warnings"].as<bool>();

        const float scaleFactor = aConfig.exists("geometryScaleFactor") ?
            aConfig["geometryScaleFactor"].as<float>() :
            1.0f;

        uint tmpIdx[12288];

        while(!inputStream.eof())
        {
            ++lineNum;

            std::string buf;
            std::getline(inputStream, buf);
            const char *cmdString = buf.c_str();

            skipWhiteSpace(cmdString);

            switch(std::tolower(*cmdString, std::locale::classic()))
            {
                case 0:
                {
                    break;
                }
                case 'v':
                {
                    uint vertexType;

                    ++cmdString;

                    switch(std::tolower(*cmdString, std::locale::classic()))
                    {
                        case 'n':
                        {
                            vertexType = 1;
                            ++cmdString;

                            break;
                        }
                        case 't':
                        {
                            vertexType = 2;
                            cmdString++;

                            break;
                        }
                        default: 
                        {
                            if(isspace(*cmdString))
                            {
                                vertexType = 0;
                            }
                            else
                            {
                                goto parsingError;
                            }
                        }
                    }

                    float tmpVert[4];
                    uint tmpVertPointer = 0;

                    for(;;)
                    {
                        skipWhiteSpace(cmdString);

                        if(*cmdString == 0)
                        {
                            break;
                        }

                        char *newCmdString;

                        float flt = (float) strtod(cmdString, &newCmdString);

                        if(newCmdString == cmdString)
                        {
                            goto parsingError;
                        }

                        cmdString = newCmdString;

                        if(tmpVertPointer >= sizeof(tmpVert) / sizeof(float))
                        {
                            goto parsingError;
                        }

                        tmpVert[tmpVertPointer++] = flt;
                    }

                    if((vertexType != 2 && tmpVertPointer != 3) ||
                       (vertexType == 2 && tmpVertPointer < 2))
                    {
                        goto parsingError;
                    }

                    if(vertexType == 0)
                    {
                        meshPopulator.addVertex(
                            scaleFactor * tmpVert[0],
                            scaleFactor * tmpVert[1],
                            scaleFactor * tmpVert[2]);
                    }
                    else if(vertexType == 1)
                    {
                        meshPopulator.addNormal(tmpVert[0], tmpVert[1], tmpVert[2]);
                    }
                    else
                    {
                        meshPopulator.addTextureCoords(tmpVert[0], tmpVert[1]);
                    }

                    break;
                }
                case 'f':
                {
                    ++cmdString;

                    if(std::tolower(*cmdString, std::locale::classic()) == 'o')
                    {
                        ++cmdString;
                    }

                    skipWhiteSpace(cmdString);

                    uint tmpIdxPtr = 0;

                    for(;;)
                    {
                        if(tmpIdxPtr + 3 >= sizeof(tmpIdx) / sizeof(int))
                        {
                            goto parsingError;
                        }

                        char *newCmdString;
                        const int idx = strtol(cmdString, &newCmdString, 10);

                        if(cmdString == newCmdString)
                        {
                            goto parsingError;
                        }

                        cmdString = newCmdString;

                        tmpIdx[tmpIdxPtr++] = idx - 1;

                        skipWhiteSpace(cmdString);

                        if(*cmdString == '/')
                        {
                            ++cmdString;

                            skipWhiteSpace(cmdString);

                            if(*cmdString != '/')
                            {
                                const int idx = strtol(cmdString, &newCmdString, 10);

                                if(cmdString == newCmdString)
                                {
                                    goto parsingError;
                                }

                                cmdString = newCmdString;

                                tmpIdx[tmpIdxPtr++] = idx - 1;
                            }
                            else
                            {
                                tmpIdx[tmpIdxPtr++] = -1;
                            }

                            skipWhiteSpace(cmdString);

                            if(*cmdString == '/')
                            {
                                ++cmdString;

                                skipWhiteSpace(cmdString);

                                const int idx = strtol(cmdString, &newCmdString, 10);

                                //Do ahead lookup of one number
                                skipWhiteSpace((const char * &)newCmdString);

                                if(std::isdigit(*newCmdString) ||
                                   ((*newCmdString == 0 || *newCmdString == '#') &&
                                    cmdString != newCmdString))
                                {
                                    if(cmdString == newCmdString)
                                    {
                                        goto parsingError;
                                    }

                                    cmdString = newCmdString;

                                    tmpIdx[tmpIdxPtr++] = idx - 1;
                                }
                                else
                                {
                                    tmpIdx[tmpIdxPtr++] = -1;
                                }
                            }
                            else
                            {
                                tmpIdx[tmpIdxPtr++] = -1;
                            }
                        }
                        else
                        {
                            tmpIdx[tmpIdxPtr++] = -1;
                            tmpIdx[tmpIdxPtr++] = -1;
                        }

                        skipWhiteSpace(cmdString);

                        if(*cmdString == 0)
                        {
                            break;
                        }
                    }

                    if(tmpIdxPtr < 9)
                    {
                        goto parsingError;
                    }

                    for(uint idx = 3; idx < tmpIdxPtr - 3; idx += 3)
                    {
                        const uint idx3 = idx + 3;

                        meshPopulator.addTriangle(
                            tmpIdx[0], tmpIdx[idx],     tmpIdx[idx3],
                            tmpIdx[2], tmpIdx[idx + 2], tmpIdx[idx3 + 2],
                            tmpIdx[1], tmpIdx[idx + 1], tmpIdx[idx3 + 1]);
                    }

                    break;
                }
                case 'o':
                case 'g':
                case 's':
                case '#':
                {
                    break;
                }
                default:
                {
                    if(_strnicmp(cmdString, "usemtl", 6) == 0)
                    {
                        cmdString += 6;

                        skipWhiteSpace(cmdString);

                        const std::string name = trimRight(cmdString);

                        if(name.empty())
                        {
                            goto parsingError;
                        }

                        meshPopulator.setActiveAppearance(name);
                    }
                    else if(_strnicmp(cmdString, "mtllib", 6) == 0)
                    {
                        cmdString += 6;
                        skipWhiteSpace(cmdString);

                        std::string matPath = trimRight(cmdString);

                        if(matPath.empty())
                        {
                            goto parsingError;
                        }

                        loadMTL(
                            meshPopulator, getParentDirectory(aPath),
                            matPath, aConfig);
                    }
                    else if(warningsEnabled)
                    {
                        RTFACT_USER_WARNING_F(
                            "%1%: unrecognized format at line: %2%",
                            aPath % lineNum);
                    }
                }
            }

            continue;

            parsingError:
            {
                if(warningsEnabled)
                {
                    RTFACT_USER_WARNING_F(
                        "%1%: parsing error at line: %2%",
                        aPath % lineNum);
                }
            }
        }

        meshPopulator.finalize();

        return mesh;
    }
};

} // namespace IO

RTFACT_NAMESPACE_END

#endif // RTFACT__TRIANGLEMESHOBJLOADER_HPP
