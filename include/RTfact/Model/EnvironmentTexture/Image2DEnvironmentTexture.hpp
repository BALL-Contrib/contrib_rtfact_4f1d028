/*
 *  Image2DEnvironmentTexture.hpp
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
 *  Created on: 2009-02-27 16:31:30 +0100
 *  Author(s): Iliyan Georgiev, Lukas Marsalek, Petr Kmoch, Felix Klein
 */

 #ifndef RTFACT__FLOAT3ENVIRONMENTTEXTURE_HPP
#define RTFACT__FLOAT3ENVIRONMENTTEXTURE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/EnvironmentTexture.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Utils/RayPacket.hpp>
#include <RTfact/Utils/EnvMapConverter.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tImage2D,
    class tImageSampler>
class Image2DEnvironmentTexture :
    public EnvironmentTexture
{
public:

    typedef tImage2D                    Image;
    typedef typename Traits<Image>::Ptr ImagePtr;

protected:

    Vec3f1   mColor;
    float    mAlpha;
    uint     mUpDimension;
    bool     mUpDimensionFlip;
    uint     mDirIndex[3];
    float    mDirFactor[3];
    ImagePtr mImage;

    void updateIndicesFactors()
    {
        float upSign = mUpDimensionFlip ? -1.f : 1.f;

        switch(mUpDimension)
        {
        case 1:
            mDirIndex[0] = 0;
            mDirIndex[1] = 1;
            mDirIndex[2] = 2;
            mDirFactor[0] = -upSign;
            mDirFactor[1] = upSign;
            mDirFactor[2] = -upSign;
            break;
        case 2:
            mDirIndex[0] = 0;
            mDirIndex[1] = 2;
            mDirIndex[2] = 1;
            mDirFactor[0] = upSign;
            mDirFactor[1] = upSign;
            mDirFactor[2] = -upSign;
            break;
        case 0:
        default:
            mDirIndex[0] = 1;
            mDirIndex[1] = 0;
            mDirIndex[2] = 2;
            mDirFactor[0] = -upSign;
            mDirFactor[1] = upSign;
            mDirFactor[2] = upSign;
        }
    }

    void convert()
    {
        Image copy(mImage->getFirstComponent(0, 0), mImage->getComponentCount(), mImage->getResX(), mImage->getResY(), true);
        EnvMapConverter mapConverter;
        mapConverter.convert(*mImage, copy);
    }

public:

    typedef float Component;

    RTFACT_ALIGNED_NEW_OPERATORS(RTFACT_SIMD_WIDTH_BYTES);

    RTFACT_DEFINE_PROPERTY(ImagePtr&, Image);

    RTFACT_DEFINE_GET_PROPERTY(uint, UpDimension);
    RTFACT_DEFINE_GET_PROPERTY(bool, UpDimensionFlip);

    void setColor(const FPacketC& aColor)
    {
        mColor.setXf(aColor[0]);
        mColor.setYf(aColor[1]);
        mColor.setZf(aColor[2]);
        mAlpha = aColor[3];
    }

    FPacketC getColor()
    {
        return FPacketC(mColor.getXf(), mColor.getYf(), mColor.getZf(), mAlpha);
    }

    void setUpDimension(
        const uint aUpDimension)
    {
        if(mUpDimension != aUpDimension)
        {
            mUpDimension = aUpDimension;
            updateIndicesFactors();
        }
    }

    void setUpDimensionFlip(
        const bool aFlip)
    {
        if(mUpDimensionFlip != aFlip)
        {
            mUpDimensionFlip = aFlip;
            updateIndicesFactors();
        }
    }

    RTFACT_FORCE_INLINE bool hasTransparency() const
    {
        return ((mImage != RTFACT_NULL) && (mImage->getComponentCount() > 3)) ||
               ((mImage == RTFACT_NULL) && (mAlpha != 1.f));
    }

    Image2DEnvironmentTexture() :
        mColor(0.f, 0.f, 0.f),
        mAlpha(1.f),
        mImage(0),
        mUpDimension(1),
        mUpDimensionFlip(false)
    {
        updateIndicesFactors();
    }

    Image2DEnvironmentTexture(
        const ImagePtr& aImage
    ) :
        mImage(aImage),
        mUpDimension(1),
        mUpDimensionFlip(false)
    {
        updateIndicesFactors();
        convert();
    }

    Image2DEnvironmentTexture(
        const uint32 aUpDimension,
        const bool aUpDimensionFlip = false
    ) :
        mImage(0),
        mUpDimension(aUpDimension),
        mUpDimensionFlip(aUpDimensionFlip)
    {
        updateIndicesFactors();
    }

    Image2DEnvironmentTexture(
        const ImagePtr& aImage,
        const uint aUpDimension,
        const bool aUpDimensionFlip = false
    ) :
        mImage(aImage),
        mUpDimension(aUpDimension),
        mUpDimensionFlip(aUpDimensionFlip)
    {
        updateIndicesFactors();
        convert();
    }

    template<
        bool taGetTransparency,
        bool taCommonOrg,
        uint taSize>
    void sample(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oColor,
        Packet<taSize, float>* oAlpha = RTFACT_NULL) const
    {
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename ActiveMask<taSize>::Container    t_PacketMaskC;

        RTFACT_ASSERT(!taGetTransparency || oAlpha != RTFACT_NULL);

        if(mImage)
        {
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                const t_FPacketC dirX = aRayPacket.dir(i).get(mDirIndex[0]) * mDirFactor[0];
                const t_FPacketC dirY = aRayPacket.dir(i).get(mDirIndex[1]) * mDirFactor[1];
                const t_FPacketC dirZ = aRayPacket.dir(i).get(mDirIndex[2]) * mDirFactor[2];

                const t_FPacketC theta = Math::acos(dirY);
                const t_FPacketC phi = Math::atan2(dirX, dirZ);

                const t_FPacketC u = phi * t_FPacketC::replicate(RTFACT_INV_2_PI);
                const t_FPacketC v = t_FPacketC::C_1 - theta * t_FPacketC::replicate(RTFACT_INV_PI);

                if(taGetTransparency && hasTransparency())
                {
                    tImageSampler::template sample<true, true>(
                        *mImage, u, v, aMask(i), &oColor(i), &(*oAlpha)(i));
                }
                else
                {
                    tImageSampler::template sample<true, false>(
                        *mImage, u, v, aMask(i), &oColor(i));
                    if (taGetTransparency)
                    {
                        (*oAlpha)(i) = aMask(i).iif(t_FPacketC::C_1, (*oAlpha)(i));
                    }
                }
            }
        }
        else
        {
            const t_Vec3fC color = t_Vec3fC::replicate(mColor);
            const t_FPacketC alphaC = t_FPacketC::replicate(mAlpha);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                oColor(i) = aMask(i).iif(color, oColor(i));

                if(taGetTransparency && hasTransparency())
                {
                    (*oAlpha)(i) = aMask(i).iif(alphaC, (*oAlpha)(i));
                }
                else
                {
                    if (taGetTransparency)
                    {
                        (*oAlpha)(i) = aMask(i).iif(t_FPacketC::C_1, (*oAlpha)(i));
                    }
                }
            }
            return;
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FLOAT3ENVIRONMENTTEXTURE_HPP
