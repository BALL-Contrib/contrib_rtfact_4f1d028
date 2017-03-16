/*
 *  ShadingInfoScene.hpp
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

 #ifndef RTFACT__SHADINGINFOSCENE_HPP
#define RTFACT__SHADINGINFOSCENE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>

#include <RTfact/Concept/Scene.hpp>

RTFACT_NAMESPACE_BEGIN

struct SceneProperties
{
    enum
    {
        INVERT_NORMAL    = 1,
        HAS_TRANSPARENCY = 1 << 1
    };
};

template<
    class tAppearanceConfig,
    class tLightConfig,
    class tEnvironmentTexture,
    umask taProperties = 0>
class RTFACT_ALIGN_SIMD ShadingInfoScene :
    public Scene
{
public:

    typedef typename tAppearanceConfig::AppearanceBase AppearanceBase;
    typedef typename tLightConfig::LightBase           LightBase;

    typedef Vector<typename Traits<AppearanceBase>::Ptr> AppearanceVector;
    typedef Vector<typename Traits<LightBase>::Ptr>      LightVector;
    typedef tEnvironmentTexture                          EnvironmentTexture;

public:

    RTFACT_STATIC_CONSTANT(PROPERTIES, taProperties);

    AppearanceVector   appearances;
    LightVector        lights;
    EnvironmentTexture environmentTexture;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SHADINGINFOSCENE_HPP
