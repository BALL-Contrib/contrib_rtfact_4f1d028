/* 
 *  RTAppearance.hpp
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
 *  Created on: 2008-11-27 16:51:44 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Felix Klein
 */
 
 #ifndef RTFACT_REMOTE__APPEARANCE_HPP
#define RTFACT_REMOTE__APPEARANCE_HPP

#include <RTremote/Config/Common.hpp>

#include <RTremote/Parameterizable.hpp>

// Internal Includes
//#include <RTremote/Renderer.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTFACT_REMOTE_API RTAppearance :
    public Parameterizable
{
    friend class Renderer;
	friend class Geometry;

protected:

    uint mID;

    uint getID() const
    {
        return mID;
    }

    RTAppearance(
        const uint aID
    ) :
        mID(aID)
    {}

public:

    virtual ~RTAppearance()
    {}
};

RTFACT_REMOTE_NAMESPACE_END

#ifdef RTFACT_REMOTE_OS_WIN

namespace boost {

inline void intrusive_ptr_add_ref(
   RTfact::Remote::RTAppearance* aObj)
{
    intrusive_ptr_add_ref((RTfact::Remote::ReferenceCounted*)aObj);
}

inline void intrusive_ptr_release(
    RTfact::Remote::RTAppearance* aObj)
{
    intrusive_ptr_release((RTfact::Remote::ReferenceCounted*)aObj);
}

}   //namespace boost

#endif

#endif // RTFACT_REMOTE__APPEARANCE_HPP
