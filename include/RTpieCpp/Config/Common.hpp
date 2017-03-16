/* 
 *  Common.hpp
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
 *  Created on: 2010-09-15 18:03:38 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_HANDLE__COMMON_HPP
#define RTFACT_HANDLE__COMMON_HPP

#include <RTpie/Config/Common.hpp>

// Determine Operating System
#ifdef _WIN32
#   define RTFACT_HANDLE_OS_WIN
#elif defined(__CYGWIN__) ||                                                   \
      defined(__linux__) ||                                                    \
      defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
      defined(__hpux) ||                                                       \
      defined(__sgi) ||                                                        \
      defined(__sun) ||                                                        \
      defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#   define RTFACT_HANDLE_OS_POSIX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#   define RTFACT_HANDLE_OS_MACOS
#else
#   pragma message("Warning: Cannot determine OS. Using default: POSIX")
#   define RTFACT_HANDLE_OS_POSIX
#endif

#define RTFACT_HANDLE_NAMESPACE_BEGIN namespace RTpieCpp {
#define RTFACT_HANDLE_NAMESPACE_END  }

// ------------------------------------------------------------
// Exceptions
// ------------------------------------------------------------

#include <stdexcept>

RTFACT_HANDLE_NAMESPACE_BEGIN

inline void ThrowException(HRESULT aResult)
{
    if(aResult == E_NOTIMPL )
        throw std::runtime_error("API Function not implemented.");
    else if(aResult == E_OUTOFMEMORY)
        throw std::runtime_error("Out of Memory.");
    else if(aResult == E_INVALIDARG)
        throw std::runtime_error("Invalid Argument");
    else
        throw std::runtime_error("Unspecified Error");
}

RTFACT_HANDLE_NAMESPACE_END

// ------------------------------------------------------------
// Helper Macros
// ------------------------------------------------------------



#define WRAPPER_FWD( wrapper_name ) \
RTFACT_HANDLE_NAMESPACE_BEGIN \
class wrapper_name;  \
RTFACT_HANDLE_NAMESPACE_END



#define WRAPPER_START( wrapper_name, parent_wrapper )    \
RTFACT_HANDLE_NAMESPACE_BEGIN    \
    \
class wrapper_name : public parent_wrapper \
{   \
    public:  \
    \
    wrapper_name (IMPL_INTERFACE *aHandle) : parent_wrapper (aHandle) {}  \
    public: \
    \
    wrapper_name () {}  \
    \
    IMPL_INTERFACE * get()  const \
    {                       \
        return static_cast<IMPL_INTERFACE * >(mHandle); \
    }

#define WRAPPER_END \
};       \
RTFACT_HANDLE_NAMESPACE_END

#define IMPLEMENT_RESULT_CHECK  \
    if(!SUCCEEDED(rv))      \
        ThrowException(rv);

#define IMPLEMENT_WRAPPER_NO_RETURN( function_call ) \
{   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> function_call ; \
    IMPLEMENT_RESULT_CHECK          \
}

#define IMPLEMENT_WRAPPER_FLOAT3(name)  \
{                                       \
    float3 result;                      \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( &result.x, &result.y, &result.z );    \
    IMPLEMENT_RESULT_CHECK              \
    return result; \
}

#define IMPLEMENT_WRAPPER_FLOAT_ARG1(name, arg1)  \
{                                       \
    float3 result;                      \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name (arg1, &result.x, &result.y, &result.z );    \
    IMPLEMENT_RESULT_CHECK              \
    return result; \
}

#define IMPLEMENT_WRAPPER_ARG0(t_result, name)    \
{   \
    t_result result;   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( &result ); \
    IMPLEMENT_RESULT_CHECK          \
    return result; \
}

#define IMPLEMENT_WRAPPER_ARG1(t_result, name, arg1)    \
{   \
    t_result result;   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( arg1, &result ); \
    IMPLEMENT_RESULT_CHECK          \
    return result; \
}

#define IMPLEMENT_WRAPPER_ARG2(t_result, name, arg1, arg2)    \
{   \
    t_result result;   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( arg1, arg2, &result ); \
    IMPLEMENT_RESULT_CHECK          \
    return result; \
}

#define IMPLEMENT_WRAPPER_ARG3(t_result, name, arg1, arg2, arg3)    \
{   \
    t_result result;   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( arg1, arg2, arg3, &result ); \
    IMPLEMENT_RESULT_CHECK          \
    return result; \
}

#define IMPLEMENT_WRAPPER_ARG4(t_result, name, arg1, arg2, arg3, arg4)    \
{   \
    t_result result;   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( arg1, arg2, arg3, arg4, &result ); \
    IMPLEMENT_RESULT_CHECK          \
    return result; \
}

#define IMPLEMENT_WRAPPER_ARG5(t_result, name, arg1, arg2, arg3, arg4, arg5)    \
{                                                                               \
    t_result result;   \
    HRESULT rv = static_cast< IMPL_INTERFACE * >(mHandle)-> name ( arg1, arg2, arg3, arg4, arg5, &result ); \
    IMPLEMENT_RESULT_CHECK          \
    return result; \
}






#endif // RTFACT_HANDLE__COMMON_HPP
