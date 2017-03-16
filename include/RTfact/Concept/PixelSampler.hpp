/* 
 *  PixelSampler.hpp
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
 
 #ifndef RTFACT__SAMPLER_HPP
#define RTFACT__SAMPLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Framebuffer.hpp>
#include <RTfact/Concept/Scene.hpp>

#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Math/MathUtils.hpp>
#include <RTfact/Utils/FramebufferUtils.hpp>

RTFACT_NAMESPACE_BEGIN

class PixelSampler
{
public:

    template<
        uint taSize>
    struct Sample
    {
        enum
        {
            SIZE = taSize,
            SIZE_SQRT = Math::SquareRoot<taSize>::value
        };

        Packet<taSize, float> imageX;
        Packet<taSize, float> imageY;
        Packet<taSize, float> deltaX;
        Packet<taSize, float> deltaY;
    };

    template<
        uint taSize>
    class Iterator
    {
    protected:

        const float mStartPixelX;
        const float mEndPixelX;
        const float mStartPixelY;
        const float mEndPixelY;
        const uint  mIEndPixelX;
        const uint  mIEndPixelY;

        RTFACT_PIXEL_SAMPLER_INLINE Iterator(
            const ImageClipRegion& aClipRegion
        ) :
            mStartPixelX(static_cast<float>(aClipRegion.left)),
            mEndPixelX(static_cast<float>(aClipRegion.right)),
            mStartPixelY(static_cast<float>(aClipRegion.top)),
            mEndPixelY(static_cast<float>(aClipRegion.bottom)),
            mIEndPixelX(aClipRegion.right),
            mIEndPixelY(aClipRegion.bottom)
        {}

    public:

        RTFACT_PIXEL_SAMPLER_INLINE bool getNextSample(
            PixelSampler::Sample<taSize>& oSample);

        template<
            class tFrameBuffer>
        RTFACT_PIXEL_SAMPLER_INLINE void write(
            const Sample<taSize>& aSample,
            const FBData<taSize>& aData,
            tFrameBuffer& aFrameBuffer) const;
    };

    template<
        uint taSize>
    RTFACT_PIXEL_SAMPLER_INLINE Iterator<taSize> getIterator(
        const ImageClipRegion& aClipRegion) const;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SAMPLER_HPP
