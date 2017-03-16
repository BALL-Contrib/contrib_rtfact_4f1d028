/* 
 *  EffectSSAOBlur.hpp
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
 
 #ifndef RTFACT__EFFECTSSAOBLUR_HPP
#define RTFACT__EFFECTSSAOBLUR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/ShaderEffect/DefaultEffectRuntime.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

class EffectSSAOBlur :
    public DefaultEffectRuntime
{
protected:
    float mKernelRadius;
    float mSharpness;
    
    void setRadius(
        const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_BLURKERNELRADIUS],mKernelRadius);
    }
    
    void setFalloff(
        const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_BLURFALLOFF],2.0f/((mKernelRadius+1.0f)*(mKernelRadius+1.0f)));
    }

    void setSharpness(
        const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_BLURSHARPNESS],mKernelRadius);
    }

    
public:
    EffectSSAOBlur() :
        mKernelRadius(9.0f),
        mSharpness(16.0f)
    {
        settableParameters(
            PARAM_BLURKERNELRADIUS,
            PARAM_BLURSHARPNESS);
    }
    
    DECLARE_GET_AND_SET_PARAMETER()

    const bool updateParameter(
        const Parameters &aParameters,
        const Semantic aParameter) const
    {
        switch (
            aParameter)
        {
            case PARAM_BLURKERNELRADIUS:
                setRadius(aParameters.mParam);
                setFalloff(aParameters.mParam);
                return true;
            case PARAM_BLURSHARPNESS:
                setSharpness(aParameters.mParam);
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
        setFalloff(par);
        setSharpness(par);
    }

    static const std::string getName()
    {
        return "SSAO:Blur";
    }

    static const std::string getFileName()
    {
        return "ssao_blur.cgfx";
    }
};

template <>
RTFACT_FORCE_INLINE const bool EffectSSAOBlur::getParameter<uint>(
    const Semantic aParameter,
    uint &aValue) const
{
    if (
        aParameter==PARAM_BLURKERNELRADIUS)
    {
        aValue=uint(mKernelRadius);
        return true;
    }
    return false;
}

template <>
RTFACT_FORCE_INLINE const bool EffectSSAOBlur::getParameter<GLfloat>(
    const Semantic aParameter,
    GLfloat &aValue) const
{
    if (
        aParameter==PARAM_BLURSHARPNESS)
    {
        aValue=mSharpness;
        return true;
    }
    return false;
}

template <>
RTFACT_FORCE_INLINE const bool EffectSSAOBlur::setParameter<uint>(
    const Semantic aParameter,
    const uint &aValue)
{
    if (
        aParameter==PARAM_BLURKERNELRADIUS &&
        aValue>0)
    {
        mKernelRadius=GLfloat(aValue);
        return true;
    }
    return false;
}

template <>
RTFACT_FORCE_INLINE const bool EffectSSAOBlur::setParameter<GLfloat>(
    const Semantic aParameter,
    const GLfloat &aValue)
{
    if (
        aParameter==PARAM_BLURSHARPNESS)
    {
        mSharpness=aValue;
        return true;
    }
    return false;
}

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__EFFECTSSAOBLUR_HPP
