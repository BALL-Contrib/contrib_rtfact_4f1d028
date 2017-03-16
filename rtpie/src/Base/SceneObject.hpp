/* 
 *  SceneObject.hpp
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
 *  Created on: 2010-09-26 19:11:26 +0200
 *  Author(s): Felix Klein
 */
 
 /*
 * RenderGeometry.hpp
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef RENDEROBJECT_HPP_
#define RENDEROBJECT_HPP_

#include "Config/InternalCommon.hpp"

RTFACT_RTPIE_FORWARD(SceneObject)

#include "Base/Base.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

class Scene;

class SceneObject : public Base {

public:
    SceneObject(Scene *aScene): mScene(aScene) {}

    Scene *mScene;


};

RTFACT_RTPIE_NAMESPACE_END


#endif /* RENDEROBJECT_HPP_ */
