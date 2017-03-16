/* 
 *  InterfaceIIDs.cpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */
 
 /*
 * InterfaceIIDs.cpp
 *
 *  Created on: Sep 25, 2010
 *      Author: lachsen
 */

#define RTFACT_RTPIE_LIB

#include "RTpie/Base/IUnknown.hpp"
#include "RTpie/Base/IParameterizable.hpp"

#include "RTpie/Data/ICamera.hpp"
#include "RTpie/Data/IImage2D.hpp"
#include "RTpie/Data/IFrameBuffer.hpp"

#include "RTpie/Scene/IAppearance.hpp"
#include "RTpie/Scene/IInstance.hpp"
#include "RTpie/Scene/ILight.hpp"
#include "RTpie/Scene/IMesh.hpp"
#include "RTpie/Scene/IGeometry.hpp"
#include "RTpie/Scene/IScene.hpp"

#include "RTpie/Tasks/IIntersectTask.hpp"
#include "RTpie/Tasks/IPickTask.hpp"
#include "RTpie/Tasks/IRayGenerationTask.hpp"
#include "RTpie/Tasks/IRayPickTask.hpp"
#include "RTpie/Tasks/IRenderTask.hpp"

#include "RTpie/IRayTracer.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

RTPIE_DEFINE_STATIC_IID_ACCESSOR(IUnknown, RTPIE_IUNKNOWN_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IParameterizable, RTPIE_IPARAMETERIZABLE_ID )

RTPIE_DEFINE_STATIC_IID_ACCESSOR(ICamera, RTPIE_ICAMERA_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IBuffer, RTPIE_IBUFFER_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IImage2D, RTPIE_IIMAGE_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IFrameBuffer, RTPIE_IRENDERBUFFER_ID )

RTPIE_DEFINE_STATIC_IID_ACCESSOR(IAppearance, RTPIE_IAPPEARANCE_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IInstance, RTPIE_IINSTANCE_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(ILight, RTPIE_ILIGHT_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IMesh, RTPIE_IMESH_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IGeometry, RTPIE_IOBJECT_ID )

RTPIE_DEFINE_STATIC_IID_ACCESSOR(IIntersectTask, RTPIE_IINTERSECTTASK_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IPickTask, RTPIE_IPICKTASK_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IRayGenerationTask, RTPIE_IRAYGENERATIONTASK_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IRayPickTask, RTPIE_IRAYPICKTASK_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IRenderTask, RTPIE_IRENDERTASK_ID )

RTPIE_DEFINE_STATIC_IID_ACCESSOR(IRayTracer, RTPIE_IRAYTRACER_ID )
RTPIE_DEFINE_STATIC_IID_ACCESSOR(IScene, RTPIE_ISCENE_ID )

RTFACT_RTPIE_NAMESPACE_END
