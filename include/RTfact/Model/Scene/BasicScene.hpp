/* 
 *  BasicScene.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__BASICSCENE_HPP
#define RTFACT__BASICSCENE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/IO/SerializationUtils.hpp>

#include <RTfact/Concept/Scene/ShadingInfoScene.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tAppearanceConfig,
    class tLightConfig,
    class tPrimitiveConfig,
    class tEnvironmentTexture,
    umask taProperties = 0>
class BasicScene :
    public ShadingInfoScene<
        tAppearanceConfig, tLightConfig, tEnvironmentTexture, taProperties>
{
    typedef ShadingInfoScene<
        tAppearanceConfig, tLightConfig, tEnvironmentTexture, taProperties> t_Base;

public:

    typedef typename t_Base::AppearanceBase          AppearanceBase;
    typedef typename t_Base::LightBase               LightBase;
    typedef typename tPrimitiveConfig::PrimitiveBase PrimitiveBase;
    
    typedef Vector<PrimitiveBase>                    PrimitiveVector;
    
    PrimitiveVector primitives;

    void serialize(
        IO::Serializer& aSerializer)
    {
        aSerializer & primitives;
    }
};

RTFACT_NAMESPACE_END

//RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_TEMPLATE_CLASS(RTfact::BasicScene, (class) (class) (class))

#endif // RTFACT__BASICSCENE_HPP
