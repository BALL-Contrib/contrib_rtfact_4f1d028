/* 
 *  NoLightAttenuation.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2009-01-22 23:53:27 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__NOLIGHTATTENUATION_HPP
#define RTFACT__NOLIGHTATTENUATION_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

class NoLightAttenuation :
    public LightAttenuation
{
public:

    template<
        uint taSize>
    RTFACT_FORCE_INLINE const Packet<taSize, float> getFactor(
        const Packet<taSize, float>& aDistance) const
    {
        return Packet<taSize, float>::C_1;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__NOLIGHTATTENUATION_HPP
