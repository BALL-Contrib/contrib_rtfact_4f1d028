/* 
 *  EffectSSAOCompute.hpp
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
 *  Author(s): Petr Kmoch, Lukas Marsalek
 */
 
 #ifndef RTFACT__EFFECTSSAOCOMPUTE_HPP
#define RTFACT__EFFECTSSAOCOMPUTE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/ShaderEffect/DefaultEffectRuntime.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

class EffectSSAOCompute :
    public DefaultEffectRuntime
{
    GLfloat mAORadius,mTangentBias;
    
    void setRadius(
        const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_AORADIUS],mAORadius);
    }
    
    void setTangentBias(
        const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_AOTANGENTBIAS],mTangentBias);
    }
    
public:
    EffectSSAOCompute() :
        mAORadius(0.5),
        mTangentBias(-RTFACT_PI_OVER_6)
    {
        settableParameters(
            PARAM_AORADIUS,
            PARAM_AOTANGENTBIAS);
    }
    
    DECLARE_GET_AND_SET_PARAMETER()
    
    const bool updateParameter(
        const Parameters &aParameters,
        const Semantic aParameter) const
    {
        switch (
            aParameter)
        {
            case PARAM_AORADIUS:
                setRadius(aParameters.mParam);
                return true;
            case PARAM_AOTANGENTBIAS:
                setTangentBias(aParameters.mParam);
                return true;
            default:
                return false;
        }
    }
    
    void updateParameters(
        const Parameters &aParameters)
    {
        const CGparameter * const par=aParameters.mParam;
        setRadius(par);
        setTangentBias(par);
    }

    static const std::string getName()
    {
        return "SSAO:Compute";
    }

    static const std::string getFileName()
    {
        return "ssao_compute.cgfx";
    }
};

template <>
RTFACT_FORCE_INLINE const bool EffectSSAOCompute::getParameter<GLfloat>(
    const Semantic aParameter,
    GLfloat& aValue) const
{
    if (
        aParameter==PARAM_AORADIUS)
    {
        aValue=mAORadius;
        return true;
    }
    else if (
        aParameter==PARAM_AOTANGENTBIAS)
    {
        aValue=mTangentBias;
        return true;
    }
    return false;
}

template <>
RTFACT_FORCE_INLINE const bool EffectSSAOCompute::setParameter<GLfloat>(
    const Semantic aParameter,
    const GLfloat &aValue)
{
    if (
        aParameter==PARAM_AORADIUS &&
        aValue>=0.0f)
    {
        mAORadius=aValue;
        return true;
    }
    else if (
        aParameter==PARAM_AOTANGENTBIAS)
    {
        mTangentBias=aValue;
        return true;
    }
    return false;
}

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__EFFECTSSAOCOMPUTE_HPP
