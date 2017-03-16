/* 
 *  RTPrivateAppearance.hpp
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
 *  Created on: 2009-09-30 14:15:34 +0200
 *  Author(s): Felix Klein, Petr Kmoch
 */
 
 #ifndef RTFACT_REMOTE__PRIVATE_APPEARANCE_HPP
#define RTFACT_REMOTE__PRIVATE_APPEARANCE_HPP

#include <RTremote/Config/Common.hpp>

#include <RTremote/RTAppearance.hpp>

// Internal Includes
#include <RTfact/Concept/Appearance.hpp>
//#include <RTremote/Renderer.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTPrivateAppearance :
    public RTAppearance
{
    friend class Renderer;
	friend class Geometry;

protected:


    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance() = 0;

    RTPrivateAppearance(
        const uint aID
    ) :
        RTAppearance(aID)
    {}

public:

    virtual ~RTPrivateAppearance()
    {}

    virtual bool hasNewAppearance(){
        return false;
    }

    virtual void setAppearanceUpdated()
    {}
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__APPEARANCE_HPP
