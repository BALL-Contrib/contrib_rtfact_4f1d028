/* 
 *  Texture.hpp
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
 *  Created on: 2008-07-16 17:43:42 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__TEXTURE_HPP
#define RTFACT__TEXTURE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/SurfaceIntersection.hpp>

RTFACT_NAMESPACE_BEGIN

class Texture
{
public:

    bool hasTransparency() const;

    template<
        bool taGetColor,
        bool taGetTransparency,
        uint taSize>
    void sample(
        SurfaceIntersection<taSize>& aIntersection,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>* oColor,
        Packet<taSize, float>* oAlpha = RTFACT_NULL);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TEXTURE_HPP
