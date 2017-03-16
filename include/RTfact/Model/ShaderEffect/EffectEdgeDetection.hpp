/* 
 *  EffectEdgeDetection.hpp
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
 *  Created on: 2010-06-19 23:00:43 +0200
 *  Author(s): Ken Dahm
 */
 
 #ifndef RTFACT__EFFECTEDGEDETECTION_HPP
#define RTFACT__EFFECTEDGEDETECTION_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/ShaderEffect/DefaultEffectRuntime.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

class EffectEdgeDetection :
    public DefaultEffectRuntime
{


public:
    EffectEdgeDetection()
    {
    }

    DECLARE_GET_AND_SET_PARAMETER()

    template <>
    const bool getParameter<GLfloat>(
        const Semantic aParameter,
        GLfloat &aValue) const
    {
		return false;
    }

    template <>
    const bool setParameter<GLfloat>(
        const Semantic aParameter,
        const GLfloat &aValue)
    {
        return false;
    }

    const bool updateParameter(
        const Parameters &aParameters,
        const Semantic aParameter) const
    {

        return false;
    }

    void updateParameters(
        const Parameters &aParameters)
    {
    }

    static const std::string getName()
    {
        return "EdgeDetection";
    }

    static const std::string getFileName()
    {
        return "edge_detector.cgfx";
    }
};

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__EFFECTGAMMA_HPP
