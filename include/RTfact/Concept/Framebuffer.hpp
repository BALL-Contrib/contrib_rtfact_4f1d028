/* 
 *  Framebuffer.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__FRAMEBUFFER_HPP
#define RTFACT__FRAMEBUFFER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Utils/FramebufferUtils.hpp>
#include <RTfact/Utils/ImageClipRegion.hpp>

RTFACT_NAMESPACE_BEGIN

class Framebuffer
{
public:

    void prePaint()
    {}

    void postPaint()
    {}

    umask getSupportedData() const;

    template<
        uint taSize>
    RTFACT_FRAMEBUFFER_INLINE void write(
        const uint aXBegin,
        const uint aYBegin,
        const uint aXEnd,
        const uint aYEnd,
        const FBData<taSize>& aData,
        const float aWeight = 1.f);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FRAMEBUFFER_HPP
