/* 
 *  ILight.hpp
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
 
 #ifndef RTFACT_RTPIE__ILIGHT_HPP
#define RTFACT_RTPIE__ILIGHT_HPP

#include <RTpie/Config/Common.hpp>

#include <RTpie/Base/IParameterizable.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_ILIGHT_ID \
		{ 0x50531ad2, 0xf0b0, 0x416e, { 0xa3, 0xc4, 0xa6, 0xfc, 0xe2, 0x9a, 0x5d, 0xce } }


/**
 *  The ILight interface is used to manage a light source of the scene
 *
 *  Light sources are created from IScene via CreateLightsource(...),
 *  Each light source is from a certain type, that can be accessed via GetType(...).
 *  Once created, the type of a light source cannot be changed.
 *
 *  Parameters of light sources are generic and can be set via the IParameterizable interface.
 */
struct ILight : public IParameterizable
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_ILIGHT_ID)

    /**
     * Switch the the light source on.
     * @return HRESULT
     */
    virtual HRESULT SwitchOn() = 0;

    /**
     * Switch the the light source off.
     * @return HRESULT
     */
    virtual HRESULT SwitchOff() = 0;

    /**
     * Get the type of the lightsource.
     * Possible Types are: "PointLight", "SpotLight", "DirectionalLight"
     * @return HRESULT
     */
    virtual HRESULT GetType(const char **_retval) = 0;

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGE_HPP

