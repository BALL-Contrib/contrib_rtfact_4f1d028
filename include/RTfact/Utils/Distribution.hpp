#ifndef RTFACT__CDF_HPP
#define RTFACT__CDF_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/Packets/Vec2f.hpp>
#include <RTfact/Utils/Range.hpp>

RTFACT_NAMESPACE_BEGIN

class Distribution
{
protected:

    RTFACT_FORCE_INLINE const Range traverse(
        const float aScaledSample) const
    {
        RTFACT_ASSERT(mData.size() > 0);
        RTFACT_ASSERT(mInvTotal > 0.f);

        Range bounds(0, static_cast<uint>(mData.size()));

        while(bounds.end - bounds.begin > 1)
        {
            const uint middle = (bounds.begin + bounds.end) >> 1;

            bounds[aScaledSample < mCumulativeData[middle]] = middle;
        }

        return bounds;
    }

    Vector<float> mData;
    Vector<float> mCumulativeData;
    float mTotal, mInvTotal, mSizeF;

public:

    Distribution() :
        mTotal(0.f),
        mInvTotal(0.f),
        mSizeF(0.f)
    {}

    Distribution(
        const uint aSize
    ) :
        mTotal(0.f),
        mInvTotal(0.f),
        mSizeF(0.f)
    {
        mData.resize(aSize);
        mCumulativeData.resize(mData.size() + 1);
    }

    RTFACT_FORCE_INLINE bool isZero() const
    {
        return mTotal == 0;
    }

    RTFACT_FORCE_INLINE float operator[](
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < mData.size());

        return mData[aIndex];
    }

    RTFACT_FORCE_INLINE float& operator[](
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < mData.size());

        return mData[aIndex];
    }

    RTFACT_FORCE_INLINE float operator()(
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex <= mData.size());

        return mCumulativeData[aIndex];
    }

    void init(
        const uint aSize)
    {
        mData.resize(aSize);
        mCumulativeData.resize(aSize + 1);
        mSizeF = static_cast<float>(aSize);
    }

    void setUniform()
    {
        for(uint i = 0; i < mData.size(); ++i)
        {
            mData[i] = 1.f;
        }

        build();
    }

    RTFACT_FORCE_INLINE uint getSize() const
    {
        return static_cast<uint>(mData.size());
    }

    RTFACT_FORCE_INLINE float getTotal() const
    {
        return mTotal;
    }

    RTFACT_FORCE_INLINE float getInvTotal() const
    {
        return mInvTotal;
    }

    void build(
        const float aUniformFraction = 0.f,
        const float aBinMaxFraction = 1.f)
    {
        RTFACT_ASSERT(mData.size() > 0);

        if(aBinMaxFraction < 1.f)
        {
            float total = 0.f;

            for(uint i = 0; i < mData.size(); ++i)
            {
                RTFACT_ASSERT(!Math::isNaN(mData[i]));
                RTFACT_ASSERT(!Math::isInfinity(mData[i]));
                RTFACT_ASSERT(mData[i] >= 0.f);

                total += mData[i];
            }

            const float maxValue = aBinMaxFraction * total;
            float totalExtraEnergy = 0.f;
            uint cappedBins = 0;

            for(uint i = 0; i < mData.size(); ++i)
            {
                const float extraEnergy = Math::max(0.f, mData[i] - maxValue);

                totalExtraEnergy += extraEnergy;
                cappedBins += (extraEnergy > 0.f);
            }

            const float compensation = totalExtraEnergy / (mData.size() - cappedBins);

            for(uint i = 0; i < mData.size(); ++i)
            {
                mData[i] = Math::min(mData[i] + compensation, maxValue);
            }
        }

        mCumulativeData[0] = 0.f;

        for(uint i = 0; i < mData.size(); ++i)
        {
            RTFACT_ASSERT(!Math::isNaN(mData[i]));
            RTFACT_ASSERT(!Math::isInfinity(mData[i]));
            RTFACT_ASSERT(mData[i] >= 0.f);

            mCumulativeData[i + 1] = mCumulativeData[i] + mData[i];
        }

        mTotal = mCumulativeData[mData.size()];

        if(aUniformFraction > 0.f)
        {
            const float pdf = aUniformFraction * mTotal / mSizeF;

            for(uint i = 0; i < mData.size(); ++i)
            {
                mCumulativeData[i + 1] = mCumulativeData[i] + pdf;
            }

            mTotal = mCumulativeData[mData.size()];
        }

        mInvTotal = Math::reciprocal(mTotal);
    }

    uint sampleDiscrete(
        const float aSample) const
    {
        RTFACT_ASSERT(!isZero());

        traverse(aSample * mTotal).begin;
    }

    uint sampleDiscrete(
        const float aSample,
        float& oPDF) const
    {
        RTFACT_ASSERT(!isZero());

        const Range bounds = traverse(aSample * mTotal);

        oPDF = mData[bounds.begin] * mInvTotal;

        return bounds.begin;
    }

    float sampleContinuous(
        const float aSample,
        float& oPDF) const
    {
        RTFACT_ASSERT(!isZero());

        const float scaledSample = aSample * mTotal;
        const Range bounds = traverse(scaledSample);

        oPDF = mData[bounds.begin] * mInvTotal * mSizeF;

        return
            (static_cast<float>(bounds.begin) +
            (scaledSample - mCumulativeData[bounds.begin]) / mData[bounds.begin]) / mSizeF;
    }

    float pdfDiscrete(
        const uint aPos) const
    {
        const float importance = mData[aPos];

        return importance * mInvTotal;
    }

    float pdfContinuous(
        const float aPos) const
    {
        const uint pos = static_cast<uint>(aPos * mSizeF);

        return mData[pos] * mInvTotal * mSizeF;
    }
};

class Distribution2D
{
protected:
   
    Vector<float> mData;
    Vector<float> mCumulativeData;
    Vector<float> mCumulativeRowData;
    uint mResX, mResY;
    float mTotal, mInvTotal, mSizeF;

public:

    Distribution2D() :
        mResX(0),
        mResY(0),
        mTotal(0.f),
        mInvTotal(0.f),
        mSizeF(0.f)
    {}

    Distribution2D(
        const uint aResX,
        const uint aResY
    ) :
        mResX(aResX),
        mResY(aResY),
        mTotal(0.f),
        mInvTotal(0.f)
    {
        const uint size = mResX * mResY;
        mSizeF = static_cast<float>(size);

        mData.resize(size);
        mCumulativeData.resize((mResX + 1) * mResY);
        mCumulativeRowData.resize(mResY + 1);
    }

    RTFACT_FORCE_INLINE bool isZero() const
    {
        return mTotal == 0.f;
    }

    void clear()
    {
        for(uint i = 0; i < mData.size(); ++i)
        {
            mData[i] = 0.f;
        }
    }

    void init(
        const uint aResX,
        const uint aResY)
    {
        if(mResX != aResX || mResY != aResY)
        {
            mResX = aResX;
            mResY = aResY;

            const uint size = mResX * mResY;
            mSizeF = static_cast<float>(size);

            mData.resize(size);
            mCumulativeData.resize((mResX + 1) * mResY);
            mCumulativeRowData.resize(mResY + 1);
        }
    }

    void setUniform()
    {
        for(uint i = 0; i < mData.size(); ++i)
        {
            mData[i] = 1.f;
        }

        build();
    }

    RTFACT_FORCE_INLINE float getTotal() const
    {
        return mTotal;
    }

    RTFACT_FORCE_INLINE float getInvTotal() const
    {
        return mInvTotal;
    }

    RTFACT_FORCE_INLINE uint getResX() const
    {
        return mResX;
    }

    RTFACT_FORCE_INLINE uint getResY() const
    {
        return mResY;
    }

    RTFACT_FORCE_INLINE float operator()(
        const uint aX,
        const uint aY) const
    {
        RTFACT_ASSERT(aX < mResX);
        RTFACT_ASSERT(aY < mResY);

        return mData[aY * mResX + aX];
    }

    RTFACT_FORCE_INLINE float& operator()(
        const uint aX,
        const uint aY)
    {
        RTFACT_ASSERT(aX < mResX);
        RTFACT_ASSERT(aY < mResY);

        return mData[aY * mResX + aX];
    }

    void build(
        const float aUniformFraction = 0.f,
        const float aBinMaxFraction = 1.f)
    {
        RTFACT_ASSERT(mResX > 0 && mResY > 0);

        if(aBinMaxFraction < 1.f)
        {
            float total = 0.f;

            for(uint i = 0; i < mData.size(); ++i)
            {
                RTFACT_ASSERT(!Math::isNaN(mData[i]));
                RTFACT_ASSERT(!Math::isInfinity(mData[i]));
                RTFACT_ASSERT(mData[i] >= 0.f);

                total += mData[i];
            }

            const float maxValue = aBinMaxFraction * total;
            float totalExtraEnergy = 0.f;
            uint cappedBins = 0;

            for(uint i = 0; i < mData.size(); ++i)
            {
                const float extraEnergy = Math::max(0.f, mData[i] - maxValue);

                totalExtraEnergy += extraEnergy;
                cappedBins += (extraEnergy > 0.f);
            }

            const float compensation = totalExtraEnergy / (mData.size() - cappedBins);

            for(uint i = 0; i < mData.size(); ++i)
            {
                mData[i] = Math::min(mData[i] + compensation, maxValue);
            }
        }

        mCumulativeRowData[0] = 0.f;

        for(uint y = 0; y < mResY; ++y)
        {
            const uint rowIndex = y * mResX;
            const uint cumRowIndex = rowIndex + y;

            mCumulativeData[cumRowIndex] = 0.f;

            for(uint x = 0; x < mResX; ++x)
            {
                RTFACT_ASSERT(!Math::isNaN(mData[rowIndex + x]));
                RTFACT_ASSERT(!Math::isInfinity(mData[rowIndex + x]));
                RTFACT_ASSERT(mData[rowIndex + x] >= 0.f);

                const uint curCumRowIndex = cumRowIndex + x;

                mCumulativeData[curCumRowIndex + 1] =
                    mCumulativeData[curCumRowIndex] + mData[rowIndex + x];
            }

            mCumulativeRowData[y + 1] =
                mCumulativeRowData[y] + mCumulativeData[cumRowIndex + mResX];
        }

        mTotal = mCumulativeRowData[mResY];

        if(aUniformFraction > 0.f)
        {
            const float pdf = aUniformFraction * mTotal / mSizeF;

            for(uint y = 0; y < mResY; ++y)
            {
                const uint rowIndex = y * mResX;
                const uint cumRowIndex = rowIndex + y;

                mCumulativeData[cumRowIndex] = 0.f;

                for(uint x = 0; x < mResX; ++x)
                {
                    const uint curCumRowIndex = cumRowIndex + x;

                    mCumulativeData[curCumRowIndex + 1] =
                        mCumulativeData[curCumRowIndex] + pdf;
                }

                mCumulativeRowData[y + 1] =
                    mCumulativeRowData[y] + mCumulativeData[cumRowIndex + mResX];
            }

            mTotal = mCumulativeRowData[mResY];
        }

        mInvTotal = Math::reciprocal(mTotal);
    }

    void sampleDiscrete(
        const Vec2f1& aSamples,
        uint& oX,
        uint& oY) const
    {
        RTFACT_ASSERT(!isZero());

        const float scaledSampleY = aSamples.y * mTotal;

        Range2D bounds(0, mResX, 0, mResY);

        while(bounds.y.end - bounds.y.begin > 1)
        {
            const uint middle = (bounds.y.begin + bounds.y.end) >> 1;

            bounds.y[scaledSampleY < mCumulativeRowData[middle]] = middle;
        }

        const float importanceY =
            mCumulativeRowData[bounds.y.end] - mCumulativeRowData[bounds.y.begin];

        const float scaledSampleX = aSamples.x * importanceY;

        const uint row = bounds.y.begin * (mResX + 1);

        while(bounds.x.end - bounds.x.begin > 1)
        {
            const uint middle = (bounds.x.begin + bounds.x.end) >> 1;

            bounds.x[scaledSampleX < mCumulativeData[row + middle]] = middle;
        }

        oX = bounds.x.begin;
        oY = bounds.y.begin;
    }

    void sampleDiscrete(
        const Vec2f1& aSamples,
        uint& oX,
        uint& oY,
        float& oPDF) const
    {
        RTFACT_ASSERT(!isZero());

        const float scaledSampleY = aSamples.y * mTotal;

        Range2D bounds(0, mResX, 0, mResY);

        while(bounds.y.end - bounds.y.begin > 1)
        {
            const uint middle = (bounds.y.begin + bounds.y.end) >> 1;

            bounds.y[scaledSampleY < mCumulativeRowData[middle]] = middle;
        }

        const float importanceY =
            mCumulativeRowData[bounds.y.end] - mCumulativeRowData[bounds.y.begin];

        const float scaledSampleX = aSamples.x * importanceY;

        const uint row = bounds.y.begin * (mResX + 1);

        while(bounds.x.end - bounds.x.begin > 1)
        {
            const uint middle = (bounds.x.begin + bounds.x.end) >> 1;

            bounds.x[scaledSampleX < mCumulativeData[row + middle]] = middle;
        }

        const uint i = row + bounds.x.begin;
        const float importanceX = mCumulativeData[i + 1] - mCumulativeData[i];

        const float xPDF = importanceX / mCumulativeData[row + mResX];
        const float yPDF = importanceY * mInvTotal;

        oX = bounds.x.begin;
        oY = bounds.y.begin;
        oPDF = xPDF * yPDF;
    }

    const Vec2f1 sampleContinuous(
        const Vec2f1& aSamples,
        float& oPDF) const
    {
        RTFACT_ASSERT(!isZero());

        const float scaledSampleY = aSamples.y * mTotal;

        Range2D bounds(0, mResX, 0, mResY);

        while(bounds.y.end - bounds.y.begin > 1)
        {
            const uint middle = (bounds.y.begin + bounds.y.end) >> 1;

            bounds.y[scaledSampleY < mCumulativeRowData[middle]] = middle;
        }

        const float importanceY =
            mCumulativeRowData[bounds.y.end] - mCumulativeRowData[bounds.y.begin];

        const float scaledSampleX = aSamples.x * importanceY;

        const uint row = bounds.y.begin * (mResX + 1);

        while(bounds.x.end - bounds.x.begin > 1)
        {
            const uint middle = (bounds.x.begin + bounds.x.end) >> 1;

            bounds.x[scaledSampleX < mCumulativeData[row + middle]] = middle;
        }

        const uint i = row + bounds.x.begin;
        const float importanceX = mCumulativeData[i + 1] - mCumulativeData[i];

        const float xPDF = importanceX / mCumulativeData[row + mResX];
        const float yPDF = importanceY * mInvTotal;

        oPDF = xPDF * yPDF * mSizeF;

        const float subX = (scaledSampleX - mCumulativeData[i]) / importanceX;
        const float subY = (scaledSampleY - mCumulativeRowData[bounds.y.begin]) / importanceY;

        return Vec2f1(
            (bounds.x.begin + subX) / mResX, (bounds.y.begin + subY) / mResY);
    }

    float pdfDiscrete(
        const uint aX,
        const uint aY) const
    {
        RTFACT_ASSERT(aX >= 0 && aX < mResX);
        RTFACT_ASSERT(aY >= 0 && aY < mResY);

        const float yPDF =
            (mCumulativeRowData[aY + 1] - mCumulativeRowData[aY]) * mInvTotal;

        const uint row = aY * (mResX + 1);
        const uint rowX = row + aX;

        const float xPDF =
            (mCumulativeData[rowX + 1] - mCumulativeData[rowX]) /
            mCumulativeData[row + mResX];

        return xPDF * yPDF;
    }

    float pdfContinuous(
        const Vec2f1& aPos) const
    {
        const uint x = static_cast<uint>(aPos.x * mResX);
        const uint y = static_cast<uint>(aPos.y * mResY);

        RTFACT_ASSERT(x >= 0 && x < mResX);
        RTFACT_ASSERT(y >= 0 && y < mResY);

        const float yPDF =
            (mCumulativeRowData[y + 1] - mCumulativeRowData[y]) * mInvTotal;

        const uint row = y * (mResX + 1);
        const uint rowX = row + x;

        const float xPDF =
            (mCumulativeData[rowX + 1] - mCumulativeData[rowX]) /
            mCumulativeData[row + mResX];

        return xPDF * yPDF * mSizeF;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__CDF_HPP
