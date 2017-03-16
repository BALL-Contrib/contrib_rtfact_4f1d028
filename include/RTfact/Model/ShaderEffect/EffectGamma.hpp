/* 
 *  EffectGamma.hpp
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
 *  Created on: 2010-06-11 12:35:28 +0200
 *  Author(s): Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__EFFECTGAMMA_HPP
#define RTFACT__EFFECTGAMMA_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/ShaderEffect/DefaultEffectRuntime.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

class EffectGamma :
    public DefaultEffectRuntime
{
    GLfloat mGamma;
    
    void setGamma(
        const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_GAMMA],mGamma);
    }


public:
    EffectGamma() :
        mGamma(1.0f)
    {
        settableParameters(
            PARAM_GAMMA);
    }
    
    DECLARE_GET_AND_SET_PARAMETER()
    
    const bool updateParameter(
        const Parameters &aParameters,
        const Semantic aParameter) const
    {
        switch (
            aParameter)
        {
            case PARAM_GAMMA:
                setGamma(aParameters.mParam);
                return true;
            default:
                return false;
        }
    }
        
    void updateParameters(
        const Parameters &aParameters)
    {
        setGamma(aParameters.mParam);
    }
    
    static const std::string getName()
    {
        return "GammaCorrection";
    }

    static const std::string getFileName()
    {
        return "gamma.cgfx";
    }
};

template <>
RTFACT_FORCE_INLINE const bool EffectGamma::getParameter<GLfloat>(
    const Semantic aParameter,
    GLfloat &aValue) const
{
    if (
        aParameter==PARAM_GAMMA)
    {
        aValue=mGamma;
        return true;
    }
    else
    {
        return false;
    }
}

template <>
RTFACT_FORCE_INLINE const bool EffectGamma::setParameter<GLfloat>(
    const Semantic aParameter,
    const GLfloat &aValue)
{
    if (
        aParameter==PARAM_GAMMA &&
        aValue!=0.0f)
    {
        mGamma=aValue;
        return true;
    }
    return false;
}

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__EFFECTGAMMA_HPP
