/* 
 *  Image2DTexture.hpp
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
 *  Created on: 2009-02-27 16:31:30 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__COMPONENT3IMAGETEXTURE2D_H
#define RTFACT__COMPONENT3IMAGETEXTURE2D_H

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Texture.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tImage2D,
    class tImageSampler>
class Image2DTexture :
    public Texture
{
public:

    typedef typename Traits<tImage2D>::Ptr ImagePtr;

protected:

    ImagePtr mImage;

public:

    Image2DTexture()
    {}

    Image2DTexture(
        const ImagePtr& aImage
    ) :
        mImage(aImage)
    {}

    RTFACT_DEFINE_PROPERTY(ImagePtr&, Image);

    RTFACT_FORCE_INLINE ImagePtr& getImage()
    {
        return mImage;
    }

    RTFACT_FORCE_INLINE bool hasTransparency() const
    {
        return (mImage != RTFACT_NULL) && (mImage->getComponentCount() > 3);
    }

    template<
        bool taGetColor,
        bool taGetTransparency,
        uint taSize>
    RTFACT_TEXTURE_INLINE void sample(
        SurfaceIntersection<taSize>& aIntersection,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>* oColor,
        Packet<taSize, float>* oAlpha = RTFACT_NULL) const
    {
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Mask<taSize>::Container          t_MaskC;

        RTFACT_TEXTURE_STATIC_ASSERT(taGetTransparency || taGetColor);

        RTFACT_TEXTURE_ASSERT(!taGetColor        || oColor != RTFACT_NULL);
        RTFACT_TEXTURE_ASSERT(!taGetTransparency || oAlpha != RTFACT_NULL);

        if(mImage != RTFACT_NULL)
        {
            const t_FPacketC resXC =
                t_FPacketC::replicate(static_cast<float>(mImage->getResX()));

            const t_FPacketC resYC =
                t_FPacketC::replicate(static_cast<float>(mImage->getResY()));

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                t_FPacketC coordXFraction =
                    Math::fraction(aIntersection.texCoord1(i));

                coordXFraction = (coordXFraction < t_FPacketC::C_0()).iif(
                    t_FPacketC::C_1 + coordXFraction,
                    coordXFraction);

                coordXFraction = Math::min(coordXFraction, t_FPacketC::C_1_MINUS_EPS);

                t_FPacketC coordYFraction =
                    Math::fraction(aIntersection.texCoord2(i));

                coordYFraction = (coordYFraction < t_FPacketC::C_0()).iif(
                    t_FPacketC::C_1 + coordYFraction,
                    coordYFraction);

                coordYFraction = Math::min(coordYFraction, t_FPacketC::C_1_MINUS_EPS);

                if(taGetTransparency)
                {
                    if(taGetColor)
                    {
                        tImageSampler::template sample<
                            taGetColor, taGetTransparency>(
                                *mImage, coordXFraction, coordYFraction, aMask(i),
                                &(*oColor)(i), &(*oAlpha)(i));
                    }
                    else
                    {
                        tImageSampler::template sample<
                            taGetColor, taGetTransparency>(
                                *mImage, coordXFraction, coordYFraction, aMask(i),
                                (t_Vec3fC*) 0, &(*oAlpha)(i));
                    }
                }
                else
                {
                    tImageSampler::template sample<
                        taGetColor, taGetTransparency>(
                            *mImage, coordXFraction, coordYFraction, aMask(i),
                            &(*oColor)(i));
                }
            }
        }
        else
        {
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                if(taGetColor)
                {
                    (*oColor)(i) = aMask(i).iif(t_Vec3fC::C_1_1_1, (*oColor)(i));
                }

                if(taGetTransparency)
                {
                    (*oAlpha)(i) = aMask(i).iif(t_FPacketC::C_1, (*oAlpha)(i));
                }
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__COMPONENT3IMAGETEXTURE2D_H
