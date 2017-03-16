/* 
 *  Parameters.hpp
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
 *  Created on: 2010-06-11 12:01:36 +0200
 *  Author(s): Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__SHADER_PARAMETERS_HPP
#define RTFACT__SHADER_PARAMETERS_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Shaders/Includes.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

enum Semantic {
    #define SHADER_PARAMETER_MACRO(maConstant,maString,ma_3,ma_4) PARAM_##maConstant,
    #include <RTfact/Utils/Shaders/ParameterList.hpp>
    PARAM_COUNT
};

inline const std::string semantic2str(
    const Semantic aSemantic)
{
    switch (
        aSemantic)
    {
        #define SHADER_PARAMETER_MACRO(maConstant,maString,ma_3,ma_4) case PARAM_##maConstant: return maString;
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
        default:
            return "";
    }
}

inline const Semantic str2semantic(
    const std::string &aString)
{
    #define SHADER_PARAMETER_MACRO(maConstant,maString,ma_3,ma_4) if (aString==maString) return PARAM_##maConstant;
    #include <RTfact/Utils/Shaders/ParameterList.hpp>
    return PARAM_IGNORE;
}

inline const Semantic str2semantic(
    const std::string &aSemantic,
    const std::string &aName)
{
    Semantic res=str2semantic(aSemantic);
    if (
        res==PARAM_IGNORE)
    {
        return str2semantic(aName);
    }
    else
    {
        return res;
    }
}

struct Parameters {
    GLuint mParamTextures[PARAM_COUNT],mParamWriteTextures[PARAM_COUNT],mFramebuffers[PARAM_COUNT];
    CGparameter mParam[PARAM_COUNT];
    GLfloat mFocusDistance;
    const GLfloat *mEyeTransform;
};

const std::string EFFECT_PARAMETER_NAME="RT_effectParam";
const std::string EFFECT_PARAMETER_TEX0_NAME="RT_effectParamTex0";
const std::string SHADER_MAIN_NAME="RT_shaderMain";
const std::string EFFECT_PASS_NAME="RT_pass";

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__SHADER_PARAMETERS_HPP
