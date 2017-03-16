/* 
 *  ParameterizableHandle.hpp
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
 
 #ifndef RTFACT_HANDLE_PARAMETERIZABLE_HPP
#define RTFACT_HANDLE_PARAMETERIZABLE_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Base/IParameterizable.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IParameterizable
WRAPPER_FWD(ParameterizableHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Data/Image2DHandle.hpp>
#include <RTpieCpp/Data/Float3.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IParameterizable
WRAPPER_START(ParameterizableHandle, BaseHandle)

    void beginParams()
    IMPLEMENT_WRAPPER_NO_RETURN( BeginParams() )

    void endParams()
    IMPLEMENT_WRAPPER_NO_RETURN( EndParams() )

    void setParami(const char *aName, const int &aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParami(aName, aValue) )

    void setParamu(const char *aName, const unsigned int &aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParamu(aName, aValue) )

    void setParamb(const char *aName, const bool &aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParamb(aName, aValue) )

    void setParamf(const char *aName, const float &aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParamf(aName, aValue) )

    void setParam3f(const char *aName, const float3 &aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParam3f(aName, aValue.x, aValue.y, aValue.z) )

    void setParams(const char *aName, const char *aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParams(aName, aValue) )

    void setParamImg(const char *aName, Image2DHandle aValue)
    IMPLEMENT_WRAPPER_NO_RETURN( SetParamImg(aName, aValue.get()) )

    int getParami(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(int, GetParami, aName)

    unsigned int getParamu(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(unsigned int, GetParamu, aName)

    bool getParamb(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(bool, GetParamb, aName)

    float getParamf(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(float, GetParamf, aName)

    float3 getParam3f(const char *aName)
    IMPLEMENT_WRAPPER_FLOAT_ARG1(GetParam3f, aName)

    char* getParams(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(char*, GetParams, aName)

    Image2DHandle getParamImg(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::IImage2D*, GetParamImg, aName)

    bool hasParam(const char *aName)
    IMPLEMENT_WRAPPER_ARG1(bool, HasParam, aName)

WRAPPER_END
#undef IMPL_INTERFACE

#endif // RTFACT_HANDLE_PARAMETERIZABLE_HPP
