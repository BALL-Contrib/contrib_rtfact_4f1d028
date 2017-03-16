/*
 *  RTCOM.hpp
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
 *  Created on: 2010-09-15 17:59:35 +0200
 *  Author(s): Felix Klein
 */

#ifndef RTCOM_HPP_
#define RTCOM_HPP_




#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#   ifndef NOMINMAX
#     define NOMINMAX 1
#   endif

#include "windows.h"

#else

typedef long LONG;
typedef unsigned long ULONG;
typedef LONG HRESULT;

#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;

#endif

typedef GUID IID;

inline bool operator== (const GUID &a, const GUID &b){
    return a.Data1 == b.Data1 &&
            a.Data2 == b.Data2 &&
            a.Data3 == b.Data3 &&
            a.Data4[0] == b.Data4[0] &&
            a.Data4[1] == b.Data4[1] &&
            a.Data4[2] == b.Data4[2] &&
            a.Data4[3] == b.Data4[3] &&
            a.Data4[4] == b.Data4[4] &&
            a.Data4[5] == b.Data4[5] &&
            a.Data4[6] == b.Data4[6] &&
            a.Data4[7] == b.Data4[7];
}

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

//
// and the inverse
//
#define FAILED(hr) (((HRESULT)(hr)) < 0)

//
// Success codes
//
#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)


#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)

#define NOERROR             0
#if defined(_WIN32) && !defined(_MAC)
//
// MessageId: E_NOTIMPL
//
// MessageText:
//
// Not implemented
//
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)

//
// MessageId: E_OUTOFMEMORY
//
// MessageText:
//
// Ran out of memory
//
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)

//
// MessageId: E_INVALIDARG
//
// MessageText:
//
// One or more arguments are invalid
//
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)

//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
// No such interface supported
//
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)

//
// MessageId: E_POINTER
//
// MessageText:
//
// Invalid pointer
//
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)

//
// MessageId: E_HANDLE
//
// MessageText:
//
// Invalid handle
//
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)

//
// MessageId: E_ABORT
//
// MessageText:
//
// Operation aborted
//
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)

//
// MessageId: E_FAIL
//
// MessageText:
//
// Unspecified error
//
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)

//
// MessageId: E_ACCESSDENIED
//
// MessageText:
//
// General access denied error
//
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)

#else
//
// MessageId: E_NOTIMPL
//
// MessageText:
//
// Not implemented
//
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80000001L)

//
// MessageId: E_OUTOFMEMORY
//
// MessageText:
//
// Ran out of memory
//
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x80000002L)

//
// MessageId: E_INVALIDARG
//
// MessageText:
//
// One or more arguments are invalid
//
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80000003L)

//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
// No such interface supported
//
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80000004L)

//
// MessageId: E_POINTER
//
// MessageText:
//
// Invalid pointer
//
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80000005L)

//
// MessageId: E_HANDLE
//
// MessageText:
//
// Invalid handle
//
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80000006L)

//
// MessageId: E_ABORT
//
// MessageText:
//
// Operation aborted
//
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80000007L)

//
// MessageId: E_FAIL
//
// MessageText:
//
// Unspecified error
//
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80000008L)

//
// MessageId: E_ACCESSDENIED
//
// MessageText:
//
// General access denied error
//
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80000009L)

#endif //WIN32

#define REFIID const IID &


#endif


// declaration tools for interface ids
#define RTPIE_DECLARE_STATIC_IID_ACCESSOR(the_iid)                    \
static const IID kIID;                                                \
RTFACT_RTPIE_API static const IID& GetIID();

#define RTPIE_DEFINE_STATIC_IID_ACCESSOR(the_interface, the_iid)      \
const IID the_interface ::kIID = the_iid; \
const RTFACT_RTPIE_API IID& the_interface ::GetIID() {return kIID;}






#endif /* RTCOM_HPP_ */
