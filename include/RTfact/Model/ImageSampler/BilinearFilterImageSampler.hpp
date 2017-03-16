/* 
 *  BilinearFilterImageSampler.hpp
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
 *  Created on: 2008-12-12 01:59:26 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__BILINEARFILTERINGIMAGESAMPLER_HPP
#define RTFACT__BILINEARFILTERINGIMAGESAMPLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Image.hpp>
#include <RTfact/Concept/ImageSampler.hpp>

#define RTFACT__COMPONENT_TO_FLOAT(aValue)                                     \
    ImageTraits<tImage2D>::template convertComponent<float>(aValue)

RTFACT_NAMESPACE_BEGIN

class BilinearFilterImageSampler :
    public ImageSampler
{
public:

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
        Packet<taSize, float>* oAlpha = RTFACT_NULL)
    {
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Mask<taSize>::Container          t_MaskC;

        // assertion(s) cannot be made compile-time
        RTFACT_IMAGE_SAMPLER_ASSERT(
            aImage.getComponentCount() >= (taGetTransparency ? 4 : 3));

        RTFACT_IMAGE_SAMPLER_STATIC_ASSERT(taGetTransparency || taGetColor);

        RTFACT_IMAGE_SAMPLER_ASSERT(!taGetColor        || oColor != RTFACT_NULL);
        RTFACT_IMAGE_SAMPLER_ASSERT(!taGetTransparency || oAlpha != RTFACT_NULL);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            const t_FPacketC coordXf = aS * static_cast<float>(aImage.getResX());
            const t_FPacketC coordYf = aT * static_cast<float>(aImage.getResY());

            const t_IPacketC coordX = Math::truncateToInt(coordXf);
            const t_IPacketC coordY = Math::truncateToInt(coordYf);

            const t_FPacketC fractionX = coordXf - Math::toFloat(coordX);
            const t_FPacketC fractionY = coordYf - Math::toFloat(coordY);

            const t_FPacketC oppositeX = t_FPacketC::C_1 - fractionX;
            const t_FPacketC oppositeY = t_FPacketC::C_1 - fractionY;

            const t_FPacketC fractionXFractionY = fractionX * fractionY;
            const t_FPacketC fractionXOppositeY = fractionX * oppositeY;
            const t_FPacketC oppositeXOppositeY = oppositeX * oppositeY;
            const t_FPacketC oppositeXFractionY = oppositeX * fractionY;

            t_IPacketC coordX1 = coordX + t_IPacketC::C_1;
            coordX1 = (coordX1 == static_cast<int>(aImage.getResX())).iif(
                t_IPacketC::C_0(), coordX1);

            t_IPacketC coordY1 = coordY + t_IPacketC::C_1;
            coordY1 = (coordY1 == static_cast<int>(aImage.getResY())).iif(
                t_IPacketC::C_0(), coordY1);

            t_Vec3fC texels[4];
            t_FPacketC alphas[4];

            RTFACT_PACKET_FOR_ALL(j, t_Vec3fC::SIZE)
            {
                if(aMask(i)[j])
                {
                    const typename tImage2D::Component* components[4] =
                    {
                        aImage.getFirstComponent(coordX[j],  coordY[j]),
                        aImage.getFirstComponent(coordX1[j], coordY[j]),
                        aImage.getFirstComponent(coordX[j],  coordY1[j]),
                        aImage.getFirstComponent(coordX1[j], coordY1[j])
                    };

                    for(uint k = 0; k < 4; ++k)
                    {
                        if(taGetColor)
                        {
                            texels[k].x[j] = RTFACT__COMPONENT_TO_FLOAT(components[k][0]);
                            texels[k].y[j] = RTFACT__COMPONENT_TO_FLOAT(components[k][1]);
                            texels[k].z[j] = RTFACT__COMPONENT_TO_FLOAT(components[k][2]);
                        }

                        if(taGetTransparency)
                        {
                            alphas[k][j] = RTFACT__COMPONENT_TO_FLOAT(components[k][3]);
                        }
                    }
                }
            }

            if(taGetColor)
            {
                const t_Vec3fC color =
                    texels[0] * oppositeXOppositeY +
                    texels[1] * fractionXOppositeY +
                    texels[2] * oppositeXFractionY +
                    texels[3] * fractionXFractionY;

                (*oColor)(i) = aMask(i).iif(color, (*oColor)(i));
            }

            if(taGetTransparency)
            {
                const t_FPacketC alpha =
                    alphas[0] * oppositeXOppositeY +
                    alphas[1] * fractionXOppositeY +
                    alphas[2] * oppositeXFractionY +
                    alphas[3] * fractionXFractionY;

                (*oAlpha)(i) = aMask(i).iif(alpha, (*oAlpha)(i));
            }
        }
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__COMPONENT_TO_FLOAT

#endif // RTFACT__BILINEARFILTERINGIMAGESAMPLER_HPP
