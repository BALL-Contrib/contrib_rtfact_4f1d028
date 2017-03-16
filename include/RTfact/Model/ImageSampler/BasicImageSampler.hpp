/* 
 *  BasicImageSampler.hpp
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
 
 #ifndef RTFACT__BASICIMAGESAMPLER_HPP
#define RTFACT__BASICIMAGESAMPLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Image.hpp>
#include <RTfact/Concept/ImageSampler.hpp>

#define RTFACT__COMPONENT_TO_FLOAT(aValue)                                     \
    ImageTraits<tImage2D>::template convertComponent<float>(aValue)

RTFACT_NAMESPACE_BEGIN

class BasicImageSampler :
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

        RTFACT_IMAGE_SAMPLER_STATIC_ASSERT(taGetTransparency || taGetColor);

        RTFACT_IMAGE_SAMPLER_ASSERT(!taGetColor        || oColor != RTFACT_NULL);
        RTFACT_IMAGE_SAMPLER_ASSERT(!taGetTransparency || oAlpha != RTFACT_NULL);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            const t_IPacketC coord1 = Math::truncateToInt(
                aS * static_cast<float>(aImage.getResX()));

            const t_IPacketC coord2 = Math::truncateToInt(
                aT * static_cast<float>(aImage.getResY()));

            RTFACT_PACKET_FOR_ALL(j, t_Vec3fC::SIZE)
            {
                if(aMask(i)[j])
                {
                    const typename tImage2D::Component* components =
                        aImage.getFirstComponent(coord1[j], coord2[j]);

                    if(taGetColor)
                    {
                        (*oColor)(i).x[j] = RTFACT__COMPONENT_TO_FLOAT(components[0]);
                        (*oColor)(i).y[j] = RTFACT__COMPONENT_TO_FLOAT(components[1]);
                        (*oColor)(i).z[j] = RTFACT__COMPONENT_TO_FLOAT(components[2]);
                    }

                    if(taGetTransparency)
                    {
                        (*oAlpha)(i)[j] = RTFACT__COMPONENT_TO_FLOAT(components[3]);
                    }
                }
            }
        }
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__COMPONENT_TO_FLOAT

#endif // RTFACT__BASICIMAGESAMPLER_HPP
