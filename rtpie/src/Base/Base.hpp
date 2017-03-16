/*
 *  Base.hpp
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
 * Base.hpp
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef BASE_HPP_
#define BASE_HPP_

#include "Config/InternalCommon.hpp"

#include <RTpie/Base/IUnknown.hpp>

#include <tbb/atomic.h>

RTFACT_RTPIE_NAMESPACE_BEGIN


// Courtesy to Dmitri
class AtomicCounter
{
public:

    typedef long ValueType;

    AtomicCounter() : counter()
    {
        // tbb::atomic<T> is not always zero initialized
        counter = 0;
    }

    explicit AtomicCounter(const ValueType x) : counter()
    {
        counter = x;
    }

    ValueType fetch_and_add(const ValueType y)
    {
        return counter.fetch_and_add(y);
    }

    ValueType operator++() { return counter.operator++(); }

    ValueType operator--() { return counter.operator--(); }

    operator ValueType() const { return counter.operator ValueType(); }

private:
    tbb::atomic<ValueType> counter;
};

class Base {
public:
    AtomicCounter mRefCount;

    Base() : mRefCount(0) {}

    virtual ~Base(){}

    virtual HRESULT QueryInterface(REFIID aId, void **_retval)
    {
        _retval = 0;
        return E_NOINTERFACE;
    }

    virtual ULONG AddRef()
    {
        return ++mRefCount;
    }

    // TODO Check if return value is correct
    virtual ULONG Release()
    {
        ULONG refCnt = --mRefCount;

        if(mRefCount == 0)
        {
            preDeletion();
            delete this;
        }

        return refCnt;
    }

    // Overwrite this functions to do complicated computations PRIOR to deletion
    virtual void preDeletion(){}

};

#define RTFACT_RTPIE_FORWARD_BASE                  \
        virtual ULONG AddRef() { return Base::AddRef(); }       \
        virtual ULONG Release() { return Base::Release(); }

#define RTFACT_RTPIE_DECLARE_QI                                 \
        virtual HRESULT QueryInterface(REFIID aId, void **_retval); \

#define RTFACT_RTPIE_QI_START(className)                            \
	    HRESULT className ::QueryInterface(REFIID aId, void **_retval) \
        {                                                          \
            *_retval = 0;                                          \
            if(aId == IUnknown::GetIID())                          \
                *_retval = static_cast<IUnknown*>(this);


#define RTFACT_RTPIE_QI_START_AMBIGOUS(className, ambigous)                 \
        HRESULT className ::QueryInterface(REFIID aId, void **_retval)      \
        {                                                                   \
            *_retval = 0;                                                   \
            if(aId == IUnknown::GetIID())                                   \
                *_retval = static_cast<IUnknown*>( static_cast< ambigous *>(this) );


#define RTFACT_RTPIE_QI_ADD(interface)                   \
            else if(aId == interface ::GetIID())                  \
                *_retval = static_cast< interface *>(this);

#define RTFACT_RTPIE_QI_ADD_AMBIGOUS(interface, ambigous)                   \
            else if(aId == interface ::GetIID())                  \
                *_retval = static_cast<IUnknown*>(static_cast< ambigous *>(this));

#define RTFACT_RTPIE_QI_END                            \
            if(!(*_retval))                            \
                return E_NOINTERFACE;                  \
            AddRef();                                  \
            return S_OK;                               \
        }

inline void intrusive_ptr_add_ref(
   RTfact::RTpie::Base* aObj)
{
    aObj->AddRef();
}

inline void intrusive_ptr_release(
    RTfact::RTpie::Base* aObj)
{
    aObj->Release();
}

RTFACT_RTPIE_NAMESPACE_END

#endif /* BASE_HPP_ */
