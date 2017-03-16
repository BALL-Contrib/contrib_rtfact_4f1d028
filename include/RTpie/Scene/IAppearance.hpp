/* 
 *  IAppearance.hpp
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
 
 #ifndef RTFACT_RTPIE__IAPPEARANCE_HPP
#define RTFACT_RTPIE__IAPPEARANCE_HPP

#include <RTpie/Config/Common.hpp>

#include <RTpie/Base/IParameterizable.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IAPPEARANCE_ID \
		{ 0x509e2803, 0x546a, 0x4bf0, { 0x9a, 0x3a, 0xfb, 0xec, 0x72, 0x94, 0xca, 0x61 } }


/**
 *  The IAppearance interface is used to manage a Appearances of the scene
 *
 *  Appearances are created from IScene via CreateAppearance(...),
 *  Each appearance is from a certain type, that can be accessed via GetType(...).
 *  Once created, the type of an appearance cannot be changed.
 *
 *  Parameters of appearances are generic and can be set via the IParameterizable interface.
 */
struct IAppearance : public IParameterizable
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IAPPEARANCE_ID)

    /**
     * Get the type of the lightsource.
     * Possible Types are: "PhongShader", "PlasticShader", "GlassShader"
     * @return HRESULT
     */
    virtual HRESULT GetType(const char **_retval) = 0;
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__APPEARANCE_HPP
