/* 
 *  EnvironmentTexture.hpp
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
 *  Created on: 2008-11-07 18:41:48 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__ENVIRONMENTTEXTURE_HPP
#define RTFACT__ENVIRONMENTTEXTURE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/RayPacket.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

RTFACT_NAMESPACE_BEGIN

class EnvironmentTexture
{
public:

    bool hasTransparency() const;

    template<
        bool taGetTransparency,
        bool taCommonOrg,
        uint taSize>
    void sample(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oAlpha = RTFACT_NULL) const;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__ENVIRONMENTTEXTURE_HPP
