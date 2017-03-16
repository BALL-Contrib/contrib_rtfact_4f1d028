/* 
 *  Image2D.hpp
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
 *  Created on: 2010-09-28 20:24:09 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_RTPIE__FRAMEBUFFER_HPP
#define RTFACT_RTPIE__FRAMEBUFFER_HPP

#include "Config/InternalCommon.hpp"

#include "RTpie/Data/IFrameBuffer.hpp"

RTFACT_RTPIE_FORWARD(Buffer)

#include "Base/Base.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

class Buffer :
    public Base, public IBuffer
{
    friend class Scene;
    friend class Renderer;

public:

    uint mLength;
    const void* mData;
    bool mAccessAllowed;
    // mSharedData=true => Data shared with other code. Do not memory manage!
    bool mSharedData;

public:

    // IUnknown

    RTFACT_RTPIE_FORWARD_BASE

    RTFACT_RTPIE_DECLARE_QI

    virtual HRESULT GetData(const void **_retval)
    {
        if(!mAccessAllowed)
            return E_FAIL;

        *_retval = mData;
        return S_OK;
    }

    virtual HRESULT GetDataLength(unsigned int *_retval)
    {
        *_retval = mLength;
        return S_OK;
    }

    virtual HRESULT StartMap()
    {
        mAccessAllowed = true;
        return S_OK;
    }

    virtual HRESULT EndMap()
    {
        mAccessAllowed = false;
        return S_OK;
    }

    // Internal

    Buffer(
        const uint aLength
    ) :
        mLength(aLength),
        mData(new char[aLength]),
        mAccessAllowed(false),
        mSharedData(false)
    {
    }

    Buffer(
        const uint aLength,
        const void *aDataSrc,
        bool aCopyData
    ) :
        mLength(aLength),
        mAccessAllowed(false),
        mSharedData(!aCopyData)
    {
        if(mSharedData)
            mData = aDataSrc;
        else{
            void *data = new char[aLength];
            memcpy(data, aDataSrc, aLength);
            mData = data;
        }
    }

    virtual ~Buffer()
    {
        if(!mSharedData)
            delete[] mData;

    }

};

#define RTFACT_RTPIE_FORWARD_BUFFER                            \
    virtual HRESULT GetData(void **_retval){                        \
       return Buffer::GetData(_retval);}                       \
    virtual HRESULT GetDataLength(unsigned int *_retval){           \
       return Buffer::GetDataLength(_retval);}                 \
    virtual HRESULT StartMap(){                                     \
       return Buffer::StartMap();}                             \
    virtual HRESULT EndMap(){                                       \
       return Buffer::EndMap();}                               \


RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGEIMPL_HPP
