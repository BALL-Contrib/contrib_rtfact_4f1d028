/* 
 *  ImageSampler.hpp
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
 *  Created on: 2008-12-12 01:59:26 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__IMAGESAMPLER_HPP
#define RTFACT__IMAGESAMPLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Packet.hpp>

RTFACT_NAMESPACE_BEGIN

class ImageSampler
{
public:

    // aX and aY should be normalized, i.e. in [0, 1)
    template<
        bool taGetColor,
        bool taGetTransparency,
        class tImage2D,
        uint taSize>
    static RTFACT_IMAGE_SAMPLER_INLINE void sample(
        const tImage2D& aImage,
        const Packet<taSize, float>& aS,
        const Packet<taSize, float>& aT,
        const Mask<taSize>& aMask,
        Vec3f<taSize>* oColor,
        Packet<taSize, float>* oAlpha = RTFACT_NULL);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__IMAGESAMPLER_HPP
