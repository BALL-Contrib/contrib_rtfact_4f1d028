/* 
 *  BasicSurfaceRayTracer.hpp
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
 
 #ifndef RTFACT__BASICSURFACERAYTRACER_HPP
#define RTFACT__BASICSURFACERAYTRACER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Renderer/RayTracer.hpp>
#include <RTfact/Utils/ImageClipRegion.hpp>
#include <RTfact/Utils/FramebufferUtils.hpp>

RTFACT_NAMESPACE_BEGIN

class BasicSurfaceRayTracer :
    public RayTracer
{
protected:
    uint mMaxDepth;

public:

    BasicSurfaceRayTracer() : mMaxDepth(0)
    {
    }

    void setMaxDepth(uint aMaxDepth)
    {
        mMaxDepth = aMaxDepth;
    }

    uint getMaxDepth() const
    {
        return mMaxDepth;
    }

    template<
        uint taSize,
        class tCamera,
        class tScene,
        class tPrimitive,
        class tIntersector,
        class tPixelSampler,
        class tIntegrator,
        class tFramebuffer>
    void render(
        const tCamera& aCamera,
        tScene& aScene,
        tPrimitive& aPrimitive,
        tIntersector& aIntersector,
        tPixelSampler& aSampler,
        tIntegrator& aIntegrator,
        tFramebuffer& aFrameBuffer,
        const ImageClipRegion& aClipRegion)
    {
        typename tPixelSampler::template Iterator<taSize> it =
            aSampler.template getIterator<taSize>(aClipRegion);

        typename tPixelSampler::template Sample<taSize> rayPacketSample;

        while(it.getNextSample(rayPacketSample))
        {
            RayPacket<taSize> rayPacket = aCamera.generateRay(rayPacketSample);
            rayPacket.maxDepth = mMaxDepth;

            ActiveMask<taSize> rayMask;
            rayMask.setTrueAndUpdateActiveContainers();

            FBData<taSize> result(aFrameBuffer.getSupportedData());

            aIntegrator.template evaluate<true>(
                rayPacketSample, rayPacket, rayMask, aScene,
                aPrimitive, aIntersector, result);

            it.write(rayPacketSample, result, aFrameBuffer);
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__BASICSURFACERAYTRACER_HPP
