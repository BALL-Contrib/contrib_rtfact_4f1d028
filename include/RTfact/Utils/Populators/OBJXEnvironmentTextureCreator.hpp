/* 
 *  OBJXEnvironmentTextureCreator.hpp
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
 
 #ifndef RTFACT__OBJXENVIRONMENTTEXTURECREATOR_HPP
#define RTFACT__OBJXENVIRONMENTTEXTURECREATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/EnvironmentTexture/Image2DEnvironmentTexture.hpp>
#include <RTfact/Model/EnvironmentTexture/Float3ConstantEnvironmentTexture.hpp>

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tEnvironmentTexture>
class OBJXEnvironmentTextureCreator
{
public:

    static tEnvironmentTexture* create(
        const IO::Configuration::EntryGroup& aParams)
    {
        const bool warningsEnabled = aParams["warnings"].as<bool>();

        if(!aParams.exists("type") || aParams["type"] != "image")
        {
            RTFACT_USER_WARNING_IF_F(warningsEnabled,
                "%1% (%2%): Incompatible environment texture specified. Should be Image.",
                aParams["__FILE__"] % aParams["__LINE__"]);

            return RTFACT_NULL;
        }

        if(!aParams.exists("file"))
        {
            RTFACT_USER_WARNING_IF_F(warningsEnabled,
                "%1% (%2%): No texture file specified",
                aParams["__FILE__"] % aParams["__LINE__"]);

            return RTFACT_NULL;
        }

        if(!aParams.exists("upDimension"))
        {
            RTFACT_USER_WARNING_IF_F(warningsEnabled,
                "%1% (%2%): No texture up dimension specified",
                aParams["__FILE__"] % aParams["__LINE__"]);

            return RTFACT_NULL;
        }

        return new tEnvironmentTexture(
            IO::Loader<typename tEnvironmentTexture::Image>::load(
                IO::appendPath(aParams["baseDir"], aParams["file"])),
            aParams["upDimension"].as<uint>());
    }
};

template<>
class OBJXEnvironmentTextureCreator<Float3ConstantEnvironmentTexture>
{
public:

    static Float3ConstantEnvironmentTexture* create(
        const IO::Configuration::EntryGroup& aParams);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__OBJXENVIRONMENTTEXTURECREATOR_HPP
