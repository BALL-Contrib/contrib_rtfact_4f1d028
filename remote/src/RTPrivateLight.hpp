/* 
 *  RTPrivateLight.hpp
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
 *  Created on: 2009-09-30 14:15:34 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_REMOTE__PRIVATE_LIGHTSOURCE_HPP
#define RTFACT_REMOTE__PRIVATE_LIGHTSOURCE_HPP

#include <RTremote/Config/Common.hpp>

#include <RTremote/RTLight.hpp>

#include <RTfact/Concept/Light.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTPrivateLight :
    public RTLight
{
    friend class Renderer;

protected:

    virtual Traits<Light>::Ptr getRTfactLight() = 0;

	virtual void updateTransformed(const RTfact::Transform&) = 0;

public:

    virtual ~RTPrivateLight()
    {}
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__LIGHTSOURCE_HPP
