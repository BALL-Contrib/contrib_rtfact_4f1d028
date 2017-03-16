/* 
 *  EffectSSAOApply.hpp
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
 *  Author(s): Lukas Marsalek
 */
 
 #ifndef RTFACT__EFFECTSSAOAPPLY_HPP
#define RTFACT__EFFECTSSAOAPPLY_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/ShaderEffect/DefaultEffectRuntime.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Shader {

class EffectSSAOApply :
    public DefaultEffectRuntime
{
public:
    EffectSSAOApply()
    {}

    static const std::string getName()
    {
        return "SSAO:Apply";
    }
    
    static const std::string getFileName()
    {
        return "ssao_apply.cgfx";
    }
};

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__EFFECTSSAOAPPLY_HPP
