/*
 *  Image2DFramebuffer.hpp
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
 *  Created on: 2009-03-10 12:51:57 +0100
 *  Author(s): Iliyan Georgiev, Ken Dahm, Lukas Marsalek, Petr Kmoch
 */

 #ifndef RTFACT_BASICCOMPONENT3FRAMEBUFFER_HPP_INCLUDED
#define RTFACT_BASICCOMPONENT3FRAMEBUFFER_HPP_INCLUDED

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Framebuffer.hpp>
#include <RTfact/Utils/ImageTraitUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tColorImage2D,
    class tDistanceImage2D = tColorImage2D,
	class tNormalImage2D = tColorImage2D,
	class tAmbientOcclusionImage2D = tColorImage2D,
    class tColorBleedingImage2D = tColorImage2D>
class RTFACT_ALIGN_SIMD Image2DFramebuffer :
    public Framebuffer
{
    #define RTFACT__FLOAT_TO_COMPONENT(aValue)                                 \
        ImageComponentConverter<float, tComponent>::convert(aValue)

    #define RTFACT__COMPONENT_TO_FLOAT(aValue)                                 \
        ImageComponentConverter<tComponent, float>::convert(aValue)

public:

    typedef tColorImage2D                                  ColorImage;
    typedef typename Traits<tColorImage2D>::Ptr            ColorImagePtr;
    typedef typename ImageTraits<tColorImage2D>::Component ColorComponent;

    typedef tDistanceImage2D                                  DistanceImage;
    typedef typename Traits<tDistanceImage2D>::Ptr            DistanceImagePtr;
    typedef typename ImageTraits<tDistanceImage2D>::Component DistanceComponent;

	typedef tNormalImage2D									NormalImage;
	typedef typename Traits<tNormalImage2D>::Ptr			NormalImagePtr;
	typedef typename ImageTraits<tNormalImage2D>::Component NormalComponent;

	typedef tAmbientOcclusionImage2D									AmbientOcclusionImage;
	typedef typename Traits<tAmbientOcclusionImage2D>::Ptr				AmbientOcclusionImagePtr;
	typedef typename ImageTraits<tAmbientOcclusionImage2D>::Component	AmbientOcclusionComponent;

    typedef tColorBleedingImage2D                                   ColorBleedingImage;
    typedef typename Traits<tColorBleedingImage2D>::Ptr				ColorBleedingImagePtr;
	typedef typename ImageTraits<tColorBleedingImage2D>::Component	ColorBleedingComponent;

protected:

    ColorImagePtr mColorImage;
    DistanceImagePtr mDistanceImage;
	NormalImagePtr mNormalImage;
	AmbientOcclusionImagePtr mAmbientOcclusionImage;
    ColorBleedingImagePtr mColorBleedingImage;
    ImageClipRegion mImageClipRegion;

    umask mSupportedData;

    template<
        bool taAccumulate,
        class tComponent>
    RTFACT_FORCE_INLINE void write(
        tComponent* aPtr,
        const float aValue,
        const float aWeight,
        const float aExistingWeigth)
    {
        if(taAccumulate)
        {
            *aPtr = RTFACT__FLOAT_TO_COMPONENT(
                aExistingWeigth * RTFACT__COMPONENT_TO_FLOAT(*aPtr) +
                aWeight * aValue);
        }
        else
        {
            *aPtr = RTFACT__FLOAT_TO_COMPONENT(aValue);
        }
    }

    template<
        bool taAccumulate,
        class tComponent>
    RTFACT_FORCE_INLINE void write(
        tComponent* aPtr,
        const float aValue1,
        const float aValue2,
        const float aValue3,
        const float aWeight,
        const float aExistingWeigth)
    {
        if(taAccumulate)
        {
            aPtr[0] = RTFACT__FLOAT_TO_COMPONENT(
                aExistingWeigth * RTFACT__COMPONENT_TO_FLOAT(aPtr[0]) +
                aWeight * aValue1);

            aPtr[1] = RTFACT__FLOAT_TO_COMPONENT(
                aExistingWeigth * RTFACT__COMPONENT_TO_FLOAT(aPtr[1]) +
                aWeight * aValue2);

            aPtr[2] = RTFACT__FLOAT_TO_COMPONENT(
                aExistingWeigth * RTFACT__COMPONENT_TO_FLOAT(aPtr[2]) +
                aWeight * aValue3);
        }
        else
        {
            aPtr[0] = RTFACT__FLOAT_TO_COMPONENT(aValue1);
            aPtr[1] = RTFACT__FLOAT_TO_COMPONENT(aValue2);
            aPtr[2] = RTFACT__FLOAT_TO_COMPONENT(aValue3);
        }
    }

    template<
        bool taAccumulate,
        uint taSize>
    RTFACT_FRAMEBUFFER_INLINE void writeProtected(
        const uint aXBegin,
        const uint aYBegin,
        const uint aXEnd,
        const uint aYEnd,
        const FBData<taSize>& aData,
        const float aWeight,
        const float aExistingWeight)
    {
        RTFACT_FRAMEBUFFER_ASSERT(
            RTFACT_CHECK_UMASK(mSupportedData, FBSupportedData::COLOR));

        const uint colorComponentCount = mColorImage->getComponentCount();

        const bool writeAlpha =
            RTFACT_CHECK_UMASK(aData.supportedData, FBSupportedData::ALPHA) &&
            (colorComponentCount == 4);

        const bool writeDistance =
            mDistanceImage != RTFACT_NULL &&
            RTFACT_CHECK_UMASK(aData.supportedData, FBSupportedData::DISTANCE);

		const bool writeNormal =
			mNormalImage != RTFACT_NULL &&
			RTFACT_CHECK_UMASK(aData.supportedData, FBSupportedData::NORMAL);

		const bool writeAmbientOcclusion =
			mAmbientOcclusionImage != RTFACT_NULL &&
			RTFACT_CHECK_UMASK(aData.supportedData, FBSupportedData::AMBIENTOCCLUSION);

        const bool writeColorBleeding =
			mColorBleedingImage != RTFACT_NULL &&
			RTFACT_CHECK_UMASK(aData.supportedData, FBSupportedData::COLORBLEEDING);

        if(taSize == 1)
        {
            ColorComponent* colorBufferPtr = mColorImage->getFirstComponent(aXBegin, aYBegin);

            write<taAccumulate>(
                colorBufferPtr,
                aData.color.xAt(0), aData.color.yAt(0), aData.color.zAt(0),
                aWeight, aExistingWeight);

            if(writeAlpha)
            {
                write<taAccumulate>(
                    colorBufferPtr + 3, aData.alpha[0],
                    aWeight, aExistingWeight);
            }

            if(writeDistance)
            {
                //Makes no sense to accumulate normal buffer, but not doing it results in flicker on geometry edges. Hmm.
                write<taAccumulate>(
                    mDistanceImage->getFirstComponent(aXBegin, aYBegin),
                    aData.distance[0], aWeight, aExistingWeight);
            }

            if (writeNormal)
            {
                //Makes no sense to accumulate normal buffer, but not doing it results in flicker on geometry edges. Hmm.
                write<taAccumulate>(
                    mNormalImage->getFirstComponent(aXBegin, aYBegin),
                    fabsf(aData.normal.xAt(0)), fabsf(aData.normal.yAt(0)), fabsf(aData.normal.zAt(0)),
                    aWeight, aExistingWeight);
            }


            if (writeAmbientOcclusion)
            {
	            write<taAccumulate>(
                    mAmbientOcclusionImage->getFirstComponent(aXBegin, aYBegin),
                    aData.ambientOcclusion[0], aWeight, aExistingWeight);
            }

            if (writeColorBleeding)
            {
                write<taAccumulate>(
                    mColorBleedingImage->getFirstComponent(aXBegin, aYBegin),
                    fabsf(aData.colorBleeding.xAt(0)), fabsf(aData.colorBleeding.yAt(0)), fabsf(aData.colorBleeding.zAt(0)),
                    aWeight, aExistingWeight);
            }
        }
        else
        {
            const uint deltaX = Vec3f<taSize>::SIZE_SQRT - (aXEnd - aXBegin);

            for(uint i = 0, y = aYBegin; y < aYEnd; ++y)
            {
                ColorComponent* colorBufferPtr =
                    mColorImage->getFirstComponent(aXBegin, y);

                for(uint x = aXBegin; x < aXEnd; ++x, ++i)
                {
                    write<taAccumulate>(
                        colorBufferPtr,
                        aData.color.xAt(i), aData.color.yAt(i), aData.color.zAt(i),
                        aWeight, aExistingWeight);

                    if(writeAlpha)
                    {
                        write<taAccumulate>(
                            colorBufferPtr + 3, aData.alpha[i],
                            aWeight, aExistingWeight);
                    }

                    colorBufferPtr += colorComponentCount;
                }

                i += deltaX;
            }

            if(writeDistance)
            {
                for(uint i = 0, y = aYBegin; y < aYEnd; ++y)
                {
                    DistanceComponent* distanceBufferPtr =
                            mDistanceImage->getFirstComponent(aXBegin, y);

                    for(uint x = aXBegin; x < aXEnd; ++x, ++i)
                    {
                        //Makes no sense to accumulate normal buffer, but not doing it results in flicker on geometry edges. Hmm.
                        write<taAccumulate>(
                            distanceBufferPtr, aData.distance[i],
                            aWeight, aExistingWeight);

                        ++distanceBufferPtr;
                    }

                    i += deltaX;
                }
            }

            if (writeNormal)
            {
                for(uint i = 0, y = aYBegin; y < aYEnd; ++y)
                {
                    NormalComponent* normalBufferPtr =
                        mNormalImage->getFirstComponent(aXBegin, y);

                    for(uint x = aXBegin; x < aXEnd; ++x, ++i)
                    {
                        //Makes no sense to accumulate normal buffer, but not doing it results in flicker on geometry edges. Hmm.
                        write<taAccumulate>(
                            normalBufferPtr,
                            fabsf(aData.normal.xAt(i)), fabsf(aData.normal.yAt(i)), fabsf(aData.normal.zAt(i)),
                            aWeight, aExistingWeight);

                        normalBufferPtr += mNormalImage->getComponentCount();
                    }

                    i += deltaX;
                }
            }

            if (writeAmbientOcclusion)
            {
                for(uint i = 0, y = aYBegin; y < aYEnd; ++y)
                {
                    AmbientOcclusionComponent* ambientOcclusionBufferPtr =
                        mAmbientOcclusionImage->getFirstComponent(aXBegin, y);

                    for(uint x = aXBegin; x < aXEnd; ++x, ++i)
                    {
                        write<taAccumulate>(
                            ambientOcclusionBufferPtr,
                            aData.ambientOcclusion[i],
                            aWeight, aExistingWeight);

                        ++ambientOcclusionBufferPtr;
                    }

                    i += deltaX;
                }
            }

            if (writeColorBleeding)
            {
                for(uint i = 0, y = aYBegin; y < aYEnd; ++y)
                {
                    ColorBleedingComponent* colorBleedingBufferptr =
                        mColorBleedingImage->getFirstComponent(aXBegin, y);

                    for(uint x = aXBegin; x < aXEnd; ++x, ++i)
                    {
                        write<taAccumulate>(
                            colorBleedingBufferptr,
                            fabsf(aData.colorBleeding.xAt(i)), fabsf(aData.colorBleeding.yAt(i)), fabsf(aData.colorBleeding.zAt(i)),
                            aWeight, aExistingWeight);

                        colorBleedingBufferptr += mColorBleedingImage->getComponentCount();
                    }

                    i += deltaX;
                }
            }
        }
    }

public:

    Image2DFramebuffer() :
        mColorImage(RTFACT_NULL),
        mDistanceImage(RTFACT_NULL),
		mNormalImage(RTFACT_NULL),
		mAmbientOcclusionImage(RTFACT_NULL),
        mColorBleedingImage(RTFACT_NULL),
        mImageClipRegion(),
        mSupportedData(0)
    {}

    RTFACT_DEFINE_GET_PROPERTY(ColorImagePtr&,    ColorImage);
    RTFACT_DEFINE_GET_PROPERTY(DistanceImagePtr&, DistanceImage);
	RTFACT_DEFINE_GET_PROPERTY(NormalImagePtr&,   NormalImage);
	RTFACT_DEFINE_GET_PROPERTY(AmbientOcclusionImagePtr&, AmbientOcclusionImage);
    RTFACT_DEFINE_GET_PROPERTY(ColorBleedingImagePtr&, ColorBleedingImage);

    void setColorImage(
        const ColorImagePtr& aImage)
    {
        RTFACT_FRAMEBUFFER_ASSERT(
            aImage == NULL ||
            aImage->getComponentCount() == 3 ||
            aImage->getComponentCount() == 4);

        mColorImage = aImage;

        if(aImage == NULL)
        {
            mSupportedData &= ~(FBSupportedData::COLOR | FBSupportedData::ALPHA);
        }
        else
        {
            mSupportedData |= FBSupportedData::COLOR;

            if(aImage->getComponentCount() == 4)
            {
                mSupportedData |= FBSupportedData::ALPHA;
            }
        }

        mImageClipRegion = ImageClipRegion(0, 0, aImage->getResX(), aImage->getResY());
    }

    void setColorImage(
        const ColorImagePtr& aImage,
        const ImageClipRegion aImageClipRegion)
    {
        RTFACT_FRAMEBUFFER_ASSERT(
            aImage == NULL ||
            aImage->getComponentCount() == 3 ||
            aImage->getComponentCount() == 4);

        mColorImage = aImage;

        if(aImage == NULL)
        {
            mSupportedData &= ~(FBSupportedData::COLOR | FBSupportedData::ALPHA);
        }
        else
        {
            mSupportedData |= FBSupportedData::COLOR;

            if(aImage->getComponentCount() == 4)
            {
                mSupportedData |= FBSupportedData::ALPHA;
            }
        }

        mImageClipRegion = aImageClipRegion;
    }

    void setDistanceImage(
        const DistanceImagePtr& aImage)
    {
        RTFACT_FRAMEBUFFER_ASSERT(
            aImage == NULL ||
            aImage->getComponentCount() == 1);

        mDistanceImage = aImage;

        if(aImage == NULL)
        {
            mSupportedData &= ~FBSupportedData::DISTANCE;
        }
        else
        {
            mSupportedData |= FBSupportedData::DISTANCE;
        }
    }

	void setNormalImage(
		const NormalImagePtr& aImage)
	{
		RTFACT_FRAMEBUFFER_ASSERT(
            aImage == NULL ||
            aImage->getComponentCount() == 3 ||
            aImage->getComponentCount() == 4);

        mNormalImage = aImage;

        if(aImage == NULL)
        {
            mSupportedData &= ~FBSupportedData::NORMAL;
        }
        else
        {
            mSupportedData |= FBSupportedData::NORMAL;
        }
	}

	void setAmbientOcclusionImage(
		const AmbientOcclusionImagePtr& aImage)
	{
		RTFACT_FRAMEBUFFER_ASSERT(
            aImage == NULL ||
            aImage->getComponentCount() == 1);

        mAmbientOcclusionImage = aImage;

        if(aImage == NULL)
        {
			mSupportedData &= ~FBSupportedData::AMBIENTOCCLUSION;
        }
        else
        {
            mSupportedData |= FBSupportedData::AMBIENTOCCLUSION;
        }
	}

    void setColorBleedingImage(
		const ColorBleedingImagePtr& aImage)
	{
		RTFACT_FRAMEBUFFER_ASSERT(
            aImage == NULL ||
            aImage->getComponentCount() == 3 ||
            aImage->getComponentCount() == 4);

        mColorBleedingImage = aImage;

        if(aImage == NULL)
        {
            mSupportedData &= ~FBSupportedData::COLORBLEEDING;
        }
        else
        {
            mSupportedData |= FBSupportedData::COLORBLEEDING;
        }
	}

    RTFACT_FORCE_INLINE umask getSupportedData() const
    {
        return mSupportedData;
    }

    template<
        uint taSize>
    RTFACT_FORCE_INLINE void write(
        const uint aXBegin,
        const uint aYBegin,
        const uint aXEnd,
        const uint aYEnd,
        const FBData<taSize>& aData,
        const float aWeight = 1.f)
    {
        RTFACT_FRAMEBUFFER_ASSERT(mColorImage != RTFACT_NULL);
        RTFACT_ASSERT(aXBegin < mColorImage->getResX() && aYBegin < mColorImage->getResY());
        RTFACT_ASSERT(aXEnd  <= mColorImage->getResX() && aYEnd  <= mColorImage->getResY());

        if(aWeight == 1.f)
        {
            //writeProtected<false>(aXBegin, aYBegin, aXEnd, aYEnd, aData, 1.f, 0.f);
            writeProtected<false>(aXBegin - mImageClipRegion.left, aYBegin - mImageClipRegion.top, aXEnd - mImageClipRegion.left, aYEnd - mImageClipRegion.top, aData, 1.f, 0.f);
        }
        else
        {
            //writeProtected<true>(aXBegin, aYBegin, aXEnd, aYEnd, aData, aWeight, 1.f - aWeight);
            writeProtected<true>(aXBegin - mImageClipRegion.left, aYBegin - mImageClipRegion.top, aXEnd - mImageClipRegion.left, aYEnd - mImageClipRegion.top, aData, aWeight, 1.f - aWeight);
        }
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__FLOAT_TO_COMPONENT
#undef RTFACT__COMPONENT_TO_FLOAT

#endif // RTFACT_BASICCOMPONENT3FRAMEBUFFER_HPP_INCLUDED
