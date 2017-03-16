/*
 *  BaseHandle.hpp
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
 *  Created on: 2010-09-26 18:55:36 +0200
 *  Author(s): Felix Klein
 */

 /*
 * IUnknown.h
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef RTFACT_HANDLE_BASE_HPP
#define RTFACT_HANDLE_BASE_HPP

#include <RTpieCpp/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>

RTFACT_HANDLE_NAMESPACE_BEGIN

class BaseHandle
{
public:

    bool operator==(const BaseHandle& aOther){
        return mHandle == aOther.mHandle;
    }
    bool operator!=(const BaseHandle& aOther){
        return mHandle != aOther.mHandle;
    }

    /* FIXME: Copy operator from RTSG1 */
    operator bool() const{
        return (mHandle != 0);
    }

    void clear(){
        if(mHandle)
            mHandle->Release();
        mHandle = 0;
    }

    ~BaseHandle(){
        if(mHandle)
            mHandle->Release();
    }

protected:
    RTfact::RTpie::IUnknown *mHandle;

    BaseHandle(RTfact::RTpie::IUnknown *aUnknown) : mHandle(aUnknown)
    {
    }

    BaseHandle() : mHandle(0) {}

    BaseHandle(const BaseHandle& aHandle) : mHandle(aHandle.mHandle)
    {
        if(mHandle)
            mHandle->AddRef();
    }

    BaseHandle& operator=(const BaseHandle& aHandle){
        if(mHandle)
            mHandle->Release();
        mHandle = aHandle.mHandle;
        if(mHandle)
            mHandle->AddRef();
        return *this;
    }






};

RTFACT_HANDLE_NAMESPACE_END


#endif /* RTFACT_HANDLE_BASE_HPP */
