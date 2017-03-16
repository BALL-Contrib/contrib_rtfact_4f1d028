/* 
 *  EffectRTCBBlur.hpp
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
 *  Created on: 2010-08-04 17:05:57 +0200
 *  Author(s): Ken Dahm
 */
 
 #ifndef RTFACT__EFFECTRTCBBLUR_HPP
#define RTFACT__EFFECTRTCBBLUR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/ShaderEffect/DefaultEffectRuntime.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

class EffectRTCBBlur :
    public DefaultEffectRuntime
{
protected:
    float mLambda;
    float mPhi;

    void setLambda(const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_RTCBLAMBDA], mLambda);
    }

    void setPhi(const CGparameter * const aPar) const
    {
        cgSetParameter1f(aPar[PARAM_RTCBPHI], mPhi);
    }

public:
	EffectRTCBBlur() : mLambda(0.2f), mPhi(0.25f)
    {
        settableParameters(
            PARAM_RTCBLAMBDA,
            PARAM_RTCBPHI);
    }

	DECLARE_GET_AND_SET_PARAMETER()

    const bool updateParameter(
        const Parameters &aParameters,
        const Semantic aParameter) const
    {
        switch (aParameter)
        {
            case PARAM_RTCBLAMBDA:
                setLambda(aParameters.mParam);
                return true;
            case PARAM_RTCBPHI:
                setPhi(aParameters.mParam);
                return true;
            default:
                return false;
        }
    }

    void updateParameters(
        const Parameters &aParameters)
    {
        const CGparameter * const par=aParameters.mParam;
        setLambda(par);
		setPhi(par);
    }

    static const std::string getName()
    {
		return "RTCB:Blur";
    }

    static const std::string getFileName()
    {
        return "rtcb_blur.cgfx";
    }
};

template <>
RTFACT_FORCE_INLINE const bool EffectRTCBBlur::getParameter<GLfloat>(
    const Semantic aParameter,
    GLfloat &aValue) const
{
switch (aParameter)
    {
        case PARAM_RTCBLAMBDA:
            aValue=mLambda;
            return true;
        case PARAM_RTCBPHI:
            aValue=mPhi;
            return true;
        default:
            return false;
    }
}

template <>
RTFACT_FORCE_INLINE const bool EffectRTCBBlur::setParameter<GLfloat>(
    const Semantic aParameter,
    const GLfloat &aValue)
{
switch (aParameter)
    {
        case PARAM_RTCBLAMBDA:
            mLambda = aValue;
            return true;
        case PARAM_RTCBPHI:
            mPhi = aValue;
            return true;
        default:
            return false;
    }
}

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__EFFECTGAMMA_HPP
