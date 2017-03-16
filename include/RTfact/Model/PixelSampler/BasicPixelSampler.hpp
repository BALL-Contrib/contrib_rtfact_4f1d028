/*
 *  BasicPixelSampler.hpp
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
 *  Created on: 2009-01-30 02:45:39 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Lukas Marsalek
 */

 #ifndef RTFACT__PIXELCENTERSAMPLER_HPP
#define RTFACT__PIXELCENTERSAMPLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/PixelSampler.hpp>

RTFACT_NAMESPACE_BEGIN

class BasicPixelSampler :
    public PixelSampler
{
protected:

    float mSampleSeedX;
    float mSampleSeedY;
    float mSampleWeight;
    uint mOverSamplingRate;

public:

    template<
        uint taSize>
    class Iterator :
        public PixelSampler::Iterator<taSize>
    {
    protected:

        typedef Packet<taSize, float> t_Packet;
        typedef PixelSampler::Iterator<taSize> t_Base;

        using t_Base::mStartPixelX;
        using t_Base::mEndPixelX;
        using t_Base::mStartPixelY;
        using t_Base::mEndPixelY;
        using t_Base::mIEndPixelX;
        using t_Base::mIEndPixelY;

        const float mSampleWeight;
        const uint mOverSamplingRate;
        float mTileSize;
        const float mSizeSqrt;

        float mTileOffsetX;
        float mTileOffsetY;

        float mCurrentPixelX;
        float mCurrentPixelY;

        float mDelta;

        Sample<taSize> mSampleTemplate;
        FBData<taSize> mData;
        uint mIteration;

    public:

        RTFACT_PIXEL_SAMPLER_INLINE Iterator(
            const ImageClipRegion& aClipRegion,
            const float aSampleBaseX,
            const float aSampleBaseY,
            const float aSampleWeight,
            const uint aOverSamplingRate
        ) :
            t_Base(aClipRegion),
            mSampleWeight(aSampleWeight),
            mOverSamplingRate(aOverSamplingRate),
            mSizeSqrt(Sample<taSize>::SIZE_SQRT),
            mDelta(1.f / aOverSamplingRate),
            mIteration(0)
        {
            mTileSize = mSizeSqrt / mOverSamplingRate;

            mData.color = Vec3f<taSize>::C_0_0_0();
            mData.alpha = Packet<taSize, float>::C_0();

            mCurrentPixelX = mStartPixelX;
            mCurrentPixelY = mStartPixelY;

            const float fOverSamplingRate = static_cast<float>(aOverSamplingRate);

            const float increment = 1.f / fOverSamplingRate;
            const float4 xIncrement = float4::replicate(increment * 4.f);
            const float4 yIncrement = float4::replicate(increment);

            const float4 startPixelX4 = float4::getInstance(
                aSampleBaseX,
                aSampleBaseX + increment,
                aSampleBaseX + 2.f * increment,
                aSampleBaseX + 3.f * increment);

            float4 currentPixelY4 = float4::replicate(aSampleBaseY);

            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
            {
                if(i % (t_Packet::SIZE_SQRT / 4) == 0)
                {
                    mSampleTemplate.imageX(i).data = startPixelX4;
                    mSampleTemplate.imageY(i).data = currentPixelY4;

                    currentPixelY4 += yIncrement;
                }
                else
                {
                    mSampleTemplate.imageX(i).data =
                        mSampleTemplate.imageX(i - 1).data + xIncrement;
                    mSampleTemplate.imageY(i).data =
                        mSampleTemplate.imageY(i - 1).data;
                }
            }
        }

        RTFACT_PIXEL_SAMPLER_INLINE bool getNextSample(
            Sample<taSize>& oSample)
        {
            if(mCurrentPixelY >= mEndPixelY)
            {
                return false;
            }

            if(mOverSamplingRate > 1)
            {
                mTileOffsetX = (mIteration % mOverSamplingRate) * mTileSize;
                mTileOffsetY = (mIteration / mOverSamplingRate) * mTileSize;

                const float4 offsetX = float4::replicate(mCurrentPixelX + mTileOffsetX);
                const float4 offsetY = float4::replicate(mCurrentPixelY + mTileOffsetY);
                const float4 delta = float4::replicate(mDelta);

                RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                {
                    oSample.imageX(i).data = mSampleTemplate.imageX(i).data + offsetX;
                    oSample.imageY(i).data = mSampleTemplate.imageY(i).data + offsetY;
                    oSample.deltaX(i).data = delta;
                    oSample.deltaY(i).data = delta;
                }

                if(mIteration == mOverSamplingRate * mOverSamplingRate - 1)
                {
                    mCurrentPixelX += mSizeSqrt;

                    if(mCurrentPixelX >= mEndPixelX)
                    {
                        mCurrentPixelX  = mStartPixelX;
                        mCurrentPixelY += mSizeSqrt;
                    }
                }
            }
            else
            {
                const float4 offsetX = float4::replicate(mCurrentPixelX);
                const float4 offsetY = float4::replicate(mCurrentPixelY);
                const float4 delta = float4::replicate(mDelta);

                RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                {
                    oSample.imageX(i).data = mSampleTemplate.imageX(i).data + offsetX;
                    oSample.imageY(i).data = mSampleTemplate.imageY(i).data + offsetY;
                    oSample.deltaX(i).data = delta;
                    oSample.deltaY(i).data = delta;
                }

                mCurrentPixelX += mSizeSqrt;

                if(mCurrentPixelX >= mEndPixelX)
                {
                    mCurrentPixelX  = mStartPixelX;
                    mCurrentPixelY += mSizeSqrt;
                }
            }

            return true;
        }

        template<
            class tFrameBuffer>
        RTFACT_PIXEL_SAMPLER_INLINE void write(
            const Sample<taSize>& aSample,
            const FBData<taSize>& aData,
            tFrameBuffer& aFrameBuffer)
        {
            if(mOverSamplingRate > 1)
            {
                const float weight = 1.f / (mOverSamplingRate * mOverSamplingRate);
                uint i = 0;

                for(uint y = 0, dataY = uint(mTileOffsetY); y < Sample<taSize>::SIZE_SQRT;
                    ++y, dataY += ((y % mOverSamplingRate) == 0))
                {
                    for(uint x = 0, dataX = uint(mTileOffsetX); x < Sample<taSize>::SIZE_SQRT;
                        ++x, ++i, dataX += ((x % mOverSamplingRate) == 0))
                    {
                        const uint dataI = dataY * Sample<taSize>::SIZE_SQRT + dataX;

                        mData.color.xAt(dataI) += weight * aData.color.xAt(i);
                        mData.color.yAt(dataI) += weight * aData.color.yAt(i);
                        mData.color.zAt(dataI) += weight * aData.color.zAt(i);
                    }
                }

                const bool writeAlpha = RTFACT_CHECK_UMASK(aData.supportedData, FBSupportedData::ALPHA);
                if (
                    writeAlpha)
                {
                    uint i = 0;
                    for(uint y = 0, dataY = uint(mTileOffsetY); y < Sample<taSize>::SIZE_SQRT;
                        ++y, dataY += ((y % mOverSamplingRate) == 0))
                    {
                        for(uint x = 0, dataX = uint(mTileOffsetX); x < Sample<taSize>::SIZE_SQRT;
                            ++x, ++i, dataX += ((x % mOverSamplingRate) == 0))
                        {
                            const uint dataI = dataY * Sample<taSize>::SIZE_SQRT + dataX;

                            mData.alpha[dataI] += weight * aData.alpha[i];
                        }
                    }
                }


                if(++mIteration == mOverSamplingRate * mOverSamplingRate)
                {
                    const uint xBegin = static_cast<uint>(
                        aSample.imageX[taSize - 1] - mSizeSqrt);

                    const uint yBegin = static_cast<uint>(
                        aSample.imageY[taSize - 1] - mSizeSqrt);

                    mData.supportedData = aData.supportedData;

                    aFrameBuffer.write(
                        xBegin,
                        yBegin,
                        std::min(xBegin + Sample<taSize>::SIZE_SQRT, mIEndPixelX),
                        std::min(yBegin + Sample<taSize>::SIZE_SQRT, mIEndPixelY),
                        mData,
                        mSampleWeight);

                    mIteration = 0;

                    mData.color = Vec3f<taSize>::C_0_0_0();
                    if (
                        writeAlpha)
                    {
                        mData.alpha = Packet<taSize, float>::C_0();
                    }
                }
            }
            else
            {
                const uint xBegin = static_cast<uint>(aSample.imageX[0]);
                const uint yBegin = static_cast<uint>(aSample.imageY[0]);
                aFrameBuffer.write(
                    xBegin,
                    yBegin,
                    std::min(xBegin + Sample<taSize>::SIZE_SQRT, mIEndPixelX),
                    std::min(yBegin + Sample<taSize>::SIZE_SQRT, mIEndPixelY),
                    aData,
                    mSampleWeight);
            }
        }
    };

    BasicPixelSampler() :
        mSampleSeedX(0.5f),
        mSampleSeedY(0.5f),
        mSampleWeight(1.f),
        mOverSamplingRate(1)
    {}

    void setSampleSeed(
        const float aSampleSeedX,
        const float aSampleSeedY,
        const float aSampleWeight)
    {
        mSampleSeedX = aSampleSeedX;
        mSampleSeedY = aSampleSeedY;
        mSampleWeight = aSampleWeight;
    }

    uint getOverSamplingRate() const
    {
        return mOverSamplingRate;
    }

    void setOverSamplingRate(
        const uint aRate)
    {
        mOverSamplingRate = aRate;
    }

    template<
        uint taSize>
    RTFACT_PIXEL_SAMPLER_INLINE Iterator<taSize> getIterator(
        const ImageClipRegion& aClipRegion) const
    {
        return Iterator<taSize>(
            aClipRegion, mSampleSeedX, mSampleSeedY,
            mSampleWeight, mOverSamplingRate);
    }
};

template<>
class BasicPixelSampler::Iterator<1> :
    public PixelSampler::Iterator<1>
{
protected:

    typedef PixelSampler::Iterator<1> t_Base;

    using t_Base::mStartPixelX;
    using t_Base::mEndPixelX;
    using t_Base::mStartPixelY;
    using t_Base::mEndPixelY;

    float  mCurrentPixelX;
    float  mCurrentPixelY;

    float mStartSampleX;

    const float mSampleWeight;

public:

    RTFACT_PIXEL_SAMPLER_INLINE Iterator(
        const ImageClipRegion& aClipRegion,
        const float aSampleBaseX,
        const float aSampleBaseY,
        const float aSampleWeight
    ) :
        t_Base(aClipRegion),
        mStartSampleX(mStartPixelX + aSampleBaseX),
        mSampleWeight(aSampleWeight)
    {
        mCurrentPixelX = mStartSampleX;
        mCurrentPixelY = mStartPixelY + aSampleBaseY;
    }

    RTFACT_PIXEL_SAMPLER_INLINE bool getNextSample(
        Sample<1>& oSample)
    {
        if(mCurrentPixelY >= mEndPixelY)
        {
            return false;
        }

        oSample.imageX[0] = mCurrentPixelX;
        oSample.imageY[0] = mCurrentPixelY;
        oSample.deltaX[0] = 1.f;
        oSample.deltaY[0] = 1.f;

        ++mCurrentPixelX;

        if(mCurrentPixelX >= mEndPixelX)
        {
            mCurrentPixelX = mStartSampleX;
            ++mCurrentPixelY;
        }

        return true;
    }

    template<
        class tFrameBuffer>
    RTFACT_PIXEL_SAMPLER_INLINE void write(
        const Sample<1>& aSample,
        const FBData<1>& aData,
        tFrameBuffer& aFrameBuffer) const
    {
        const uint x = Math::floorToInt(aSample.imageX);
        const uint y = Math::floorToInt(aSample.imageY);

        aFrameBuffer.write(x, y, x + 1, y + 1, aData, mSampleWeight);
    }
};

template<>
class BasicPixelSampler::Iterator<4> :
    public PixelSampler::Iterator<4>
{
protected:

    typedef PixelSampler::Iterator<4> t_Base;

    using t_Base::mStartPixelX;
    using t_Base::mEndPixelX;
    using t_Base::mStartPixelY;
    using t_Base::mEndPixelY;

    float  mCurrentPixelX;
    float  mCurrentPixelY;
    float4 mStartPixelX4;
    float4 mCurrentPixelX4;
    float4 mCurrentPixelY4;

    float mStartSampleX;

    const float mSampleWeight;

public:

    RTFACT_PIXEL_SAMPLER_INLINE Iterator(
        const ImageClipRegion& aClipRegion,
        const float aSampleBaseX,
        const float aSampleBaseY,
        const float aSampleWeight
    ) :
        PixelSampler::Iterator<4>(aClipRegion),
        mStartSampleX(mStartPixelX + aSampleBaseX),
        mSampleWeight(aSampleWeight)
    {
        mCurrentPixelX = mStartSampleX;
        mCurrentPixelY = mStartPixelY + aSampleBaseY;

        mCurrentPixelX4 =
        mStartPixelX4 =
            float4::replicate(mCurrentPixelX) + float4::C_0_1_0_1;
        mCurrentPixelY4 =
            float4::replicate(mCurrentPixelY) + float4::C_0_0_1_1;
    }

    RTFACT_PIXEL_SAMPLER_INLINE bool getNextSample(
        Sample<4>& oSample)
    {
        if(mCurrentPixelY >= mEndPixelY)
        {
            return false;
        }

        oSample.imageX.data = mCurrentPixelX4;
        oSample.imageY.data = mCurrentPixelY4;
        oSample.deltaX.data = float4::replicate(1.f);
        oSample.deltaY.data = oSample.deltaX.data;

        mCurrentPixelX += 2;
        mCurrentPixelX4 += float4::C_2_2_2_2;

        if(mCurrentPixelX >= mEndPixelX)
        {
            mCurrentPixelX = mStartSampleX;
            mCurrentPixelX4 = mStartPixelX4;
            mCurrentPixelY += 2;
            mCurrentPixelY4 = float4::replicate(mCurrentPixelY) +
                float4::C_0_0_1_1;
        }

        return true;
    }

    template<
        class tFrameBuffer>
    RTFACT_PIXEL_SAMPLER_INLINE void write(
        const Sample<4>& aSample,
        const FBData<4>& aData,
        tFrameBuffer& aFrameBuffer) const
    {
        const uint x = Math::floorToInt(aSample.imageX[0]);
        const uint y = Math::floorToInt(aSample.imageY[0]);

        aFrameBuffer.write(
            x,
            y,
            std::min(x + 2, mIEndPixelX),
            std::min(y + 2, mIEndPixelY),
            aData,
            mSampleWeight);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__PIXELCENTERSAMPLER_HPP
