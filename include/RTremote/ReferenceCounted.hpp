/*
 *  ReferenceCounted.hpp
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

 #ifndef RTFACT__REMOTE_REFERENCECOUNTED_HPP
#define RTFACT__REMOTE_REFERENCECOUNTED_HPP

#include <RTremote/Config/Common.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTFACT_REMOTE_API ReferenceCounted
{
public:

    size_t mRefCount;

public:

    ReferenceCounted() :
        mRefCount(0)
    {}

    virtual size_t addRef()
    {
        return mRefCount++;
    }

    virtual size_t getReferenceCounted() const
    {
        return mRefCount;
    }

    virtual size_t release()
    {
        size_t refCnt = mRefCount;

        if(--mRefCount == 0)
        {
            delete this;
        }

        return refCnt;
    }

    virtual ~ReferenceCounted()
    {}
};

inline void intrusive_ptr_add_ref(
   RTfact::Remote::ReferenceCounted* aObj)
{
    aObj->addRef();
}

inline void intrusive_ptr_release(
    RTfact::Remote::ReferenceCounted* aObj)
{
    aObj->release();
}

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT__REFERENCECOUNTED_HPP
