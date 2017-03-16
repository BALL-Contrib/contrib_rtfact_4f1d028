/* 
 *  IParameterizable.hpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */
 
 /*
 * Parameterizable.hpp
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef IPARAMETERIZABLE_HPP_
#define IPARAMETERIZABLE_HPP_

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>


#include <RTpie/Data/BasicData.hpp>
#include <RTpie/Data/IImage2D.hpp>


RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IPARAMETERIZABLE_ID \
		{ 0xafacd6fb, 0x5117, 0x4dfe, { 0xb5, 0xd5, 0x8e, 0x36, 0xa4, 0x60, 0x6c, 0xc9 } }


/**
 *  This is the base interface for ILight, IAppearance and any interface that support generic parameters
 *
 *  This Interface provides several functions to set and receive Parameters.
 *  For each supported type, there is a getter and setter function.
 *
 *  The following types are supported:
 *  int             - SetParami, GetParami
 *  bool            - SetParamb, GetParamb
 *  float           - SetParamf, GetParamf
 *  3 floats        - SetParam3f, GetParam3f
 *  const char*     - SetParams, GetParams
 *  unsigned int    - SetParamu, GetParamu
 *  RTpie::IImage2D*  - SetParamImg, GetParamImg
 *
 *  TODO: Explain BeginParams() and EndParams()
 */
struct IParameterizable : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IPARAMETERIZABLE_ID)

    virtual HRESULT BeginParams() = 0;

    virtual HRESULT EndParams() = 0;

    virtual HRESULT SetParami(const char *aName, const int &aValue) = 0;

    virtual HRESULT SetParamu(const char *aName, const unsigned int &aValue) = 0;

    virtual HRESULT SetParamb(const char *aName, const bool &aValue) = 0;

    virtual HRESULT SetParamf(const char *aName, const float &aValue) = 0;

    virtual HRESULT SetParam3f(const char *aName, const float &aV1, const float &aV2, const float &aV3 ) = 0;

    virtual HRESULT SetParams(const char *aName, const char *aValue) = 0;

    virtual HRESULT SetParamImg(const char *aName, IImage2D *aValue) = 0;

    virtual HRESULT GetParami(const char *aName, int *_retval) = 0;

    virtual HRESULT GetParamu(const char *aName, unsigned int *_retval) = 0;

    virtual HRESULT GetParamb(const char *aName, bool *_retval) = 0;

    virtual HRESULT GetParamf(const char *aName, float *_retval) = 0;

    virtual HRESULT GetParam3f(const char *aName, float *aV1, float *aV2, float *aV3 ) = 0;

    virtual HRESULT GetParams(const char *aName, char **_retval) = 0;

    virtual HRESULT GetParamImg(const char *aName, IImage2D**_retval) = 0;

    virtual HRESULT HasParam(const char *aName, bool *_retval) = 0;
};

RTFACT_RTPIE_NAMESPACE_END

#endif /* PARAMETERIZABLE_HPP_ */
