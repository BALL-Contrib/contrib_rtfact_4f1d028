#ifndef RTFACT__MIPMAP_HPP
#define RTFACT__MIPMAP_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>

#include <RTfact/Utils/Math/MathUtils.hpp>
#include <RTfact/Utils/TextureUtils.hpp>
//#include <RTfact/Utils/Image.hpp>
//#include <RTfact/Utils/ImageUtils.hpp>
//#include <RTfact/Utils/Image2DSampler.hpp>
#include <RTfact/Utils/Containers/RefBlockedArray2D.hpp>
#include <RTfact/Utils/Containers/Vector.hpp>

#define RTFACT__FILTER_WIDTH 2

typedef unsigned char   ubyte;

RTFACT_NAMESPACE_BEGIN

// -------------------------
enum WrapMode
{
    WRAP_REPEAT = 0,
    WRAP_CLAMP  = 1
};

RTFACT_TEXTURE_INLINE uint wrapCoord(
    const uint aCoord,
    const WrapMode aWrap,
    const uint aResolution)
{
    switch(aWrap)
    {
    case WRAP_REPEAT:
        return Math::mod(aCoord, aResolution);
    default: // WRAP_CLAMP
        return Math::min(aCoord, aResolution - 1);
    }
}

RTFACT_TEXTURE_INLINE float wrapCoord(
    const float aCoord,
    const WrapMode aWrap)
{
    float temp;

    switch(aWrap)
    {
    case WRAP_REPEAT:
        temp = Math::fraction(aCoord);
        temp = (temp < 0.f) ? 1.f + temp : temp;
        break;
    case WRAP_CLAMP:
        temp = Math::max(aCoord, 0.f);
        break;
    }

    return Math::min(temp, 1.f - RTFACT_EPSILON);
}

RTFACT_TEXTURE_INLINE void getCoords(
    const Vec2f1& aCoords,
    const WrapMode aWrapX,
    const WrapMode aWrapY,
    const uint aResX,
    const uint aResY,
    uint& oX,
    uint& oY,
    float& oXFrac,
    float& oYFrac)
{
    const float xf = wrapCoord(aCoords.x, aWrapX);
    const float yf = wrapCoord(aCoords.y, aWrapY);

    const float coordXf = xf * static_cast<float>(aResX);
    const float coordYf = yf * static_cast<float>(aResY);

    float x, y;

    oXFrac = Math::fraction(coordXf, x);
    oYFrac = Math::fraction(coordYf, y);

    oX = static_cast<uint>(x);
    oY = static_cast<uint>(y);
}


// -------------------------

class MipMap
{
protected:

    struct ResampleWeight
    {
        int firstTexel;
        float weight[RTFACT__FILTER_WIDTH * 2];
    };

    void computeWeights(
        const uint aOldRes,
        const uint aNewRes,
        Vector<ResampleWeight>& oWeights)
    {
        const float invFilterWidth = 1.f / RTFACT__FILTER_WIDTH;

        for(uint i = 0; i < aNewRes; ++i)
        {
            const float center = (i + 0.5f) * aOldRes / aNewRes;

            oWeights[i].firstTexel =
                Math::floorToInt(center - RTFACT__FILTER_WIDTH + 0.5f);

            float weightSum = 0.f;

            for(uint j = 0; j < RTFACT__FILTER_WIDTH * 2; ++j)
            {
                const float pos = oWeights[i].firstTexel + j + .5f;

                weightSum += oWeights[i].weight[j] =
                    lanczos((pos - center) * invFilterWidth);
            }

            weightSum = 1.f / weightSum;

            for(uint j = 0; j < RTFACT__FILTER_WIDTH * 2; ++j)
            {
                oWeights[i].weight[j] *= weightSum;
            }
        }
    }

    template<
        bool taGetColor,
        bool taGetAlpha>
    RTFACT_TEXTURE_INLINE void lookupPrivate(
        const Vec2f1& aCoords,
        const uint aLevel,
        Vec3f1* oColor,
        float* oAlpha) const
    {
        uint x, y;
        float xFrac, yFrac;

        getCoords(
            aCoords, mWrap, mWrap, mLevels[aLevel].getResX(),
            mLevels[aLevel].getResY(), x, y, xFrac, yFrac);

        const uint x1 = wrapCoord(x + 1, mWrap, mLevels[aLevel].getResX());
        const uint y1 = wrapCoord(y + 1, mWrap, mLevels[aLevel].getResY());

        const float xOpp = 1.f - xFrac;
        const float yOpp = 1.f - yFrac;

        const float xFracYFrac = xFrac * yFrac;
        const float xFracYOpp = xFrac * yOpp;
        const float xOppYOpp = xOpp * yOpp;
        const float xOppYFrac = xOpp * yFrac;

        const float* ptr[4] = {
            mLevels[aLevel].get<float>(x,  y),
            mLevels[aLevel].get<float>(x1, y),
            mLevels[aLevel].get<float>(x,  y1),
            mLevels[aLevel].get<float>(x1, y1)
        };

        if(taGetColor)
        {
            oColor->x = ptr[0][0] * xOppYOpp  + ptr[1][0] * xFracYOpp +
                        ptr[2][0] * xOppYFrac + ptr[3][0] * xFracYFrac;

            oColor->y = ptr[0][1] * xOppYOpp  + ptr[1][1] * xFracYOpp +
                        ptr[2][1] * xOppYFrac + ptr[3][1] * xFracYFrac;

            oColor->z = ptr[0][2] * xOppYOpp  + ptr[1][2] * xFracYOpp +
                        ptr[2][2] * xOppYFrac + ptr[3][2] * xFracYFrac;
        }

        if(taGetAlpha)
        {
            *oAlpha = ptr[0][3] * xOppYOpp  + ptr[1][3] * xFracYOpp +
                      ptr[2][3] * xOppYFrac + ptr[3][3] * xFracYFrac;
        }
    }

    Vector<RefBlockedArray2D<ubyte> > mLevels;
    uint mResX, mResY, mComponentCount;
    uint mOrigResX, mOrigResY;
    uint mLevelCount;
    bool mPrecise;
    float mMaxAnisotropy;
    WrapMode mWrap;

public:

    MipMap() :
        mResX(0),
        mResY(0),
        mComponentCount(0),
        mLevelCount(0),
        mPrecise(false),
        mMaxAnisotropy(8.f),
        mWrap(WRAP_REPEAT)
    {}

    MipMap(
        const BasicImage2D<float>& aImage,
        const WrapMode aWrap = WRAP_REPEAT,
        const bool aPrecise = false,
        const float aMaxAnisotropy = 8.f)
    {
        init(aImage, aWrap, aPrecise, aMaxAnisotropy);
    }

    RTFACT_IMAGE_INLINE uint getResX() const
    {
        return mResX;
    }

    RTFACT_IMAGE_INLINE uint getResY() const
    {
        return mResY;
    }

    RTFACT_IMAGE_INLINE uint getOrigResX() const
    {
        return mOrigResX;
    }

    RTFACT_IMAGE_INLINE uint getOrigResY() const
    {
        return mOrigResY;
    }

    RTFACT_IMAGE_INLINE uint getComponentCount() const
    {
        return mComponentCount;
    }

    RTFACT_IMAGE_INLINE uint getLevelCount() const
    {
        return static_cast<uint>(mLevels.size());
    }

    void init(
        const BasicImage2D<float>& aImage,
        const WrapMode aWrap = WRAP_REPEAT,
        const bool aPrecise = false,
        const float aMaxAnisotropy = 8.f)
    {
        RTFACT_IMAGE_ASSERT(aImage.getComponentCount() >= 3);

        mWrap = aWrap;
        mComponentCount = aImage.getComponentCount();
        mPrecise = aPrecise;
        mMaxAnisotropy = aMaxAnisotropy;
        mOrigResX = aImage.getResX();
        mOrigResY = aImage.getResY();
        mResX = Math::roundUpToPowerOf2(mOrigResX);
        mResY = Math::roundUpToPowerOf2(mOrigResY);

        const uint lineWidth = mResX * mComponentCount;

        Vector<float> tempBuffers[2];
        tempBuffers[0].resize(lineWidth * mResY);
        tempBuffers[1].resize(lineWidth * mResY);

        if(mResX == mOrigResX && mResY == mOrigResY)
        {
            for(uint y = 0; y < mResY; ++y)
            {
                for(uint x = 0; x < mResX; ++x)
                {
                    const uint xc = x * mComponentCount;

                    for(uint c = 0; c < mComponentCount; ++c)
                    {
                        tempBuffers[0][y * lineWidth + xc + c] = *(aImage(x, y) + c);
                    }
                }
            }
        }
        else
        {
            Vector<ResampleWeight> xWeights(mResX), yWeights(mResY);

            computeWeights(mOrigResX, mResX, xWeights);
            computeWeights(mOrigResY, mResY, yWeights);

            // resize in X dimension
            for(uint y = 0; y < mOrigResY; ++y)
            {
                for(uint x = 0; x < mResX; ++x)
                {
                    const uint xc = x * mComponentCount;

                    for(uint c = 0; c < mComponentCount; ++c)
                    {
                        tempBuffers[0][y * lineWidth + xc + c] = 0.f;
                    }

                    for(uint p = 0; p < RTFACT__FILTER_WIDTH * 2; ++p)
                    {
                        int origX = xWeights[x].firstTexel + p;

                        switch(mWrap)
                        {
                        case WRAP_REPEAT:
                            origX = Math::mod(origX, mOrigResX);
                            break;
                        case WRAP_CLAMP:
                            origX = Math::clamp(origX, 0, (int)mOrigResX - 1);
                            break;
                        }

                        if(origX >= 0)
                        {
                            for(uint c = 0; c < mComponentCount; ++c)
                            {
                                float data = *(aImage(origX, y) + c);

                                tempBuffers[0][y * lineWidth + xc + c] +=
                                    xWeights[x].weight[p] * data;
                            }
                        }
                    }
                }
            }

            Vector<float> temp(mResY * mComponentCount);

            // resize in Y dimension
            for(uint x = 0; x < mResX; ++x)
            {
                const uint xc = x * mComponentCount;

                for(uint y = 0; y < mResY; ++y)
                {
                    const uint yc = y * mComponentCount;

                    for(uint c = 0; c < mComponentCount; ++c)
                    {
                        temp[yc + c] = 0.f;
                    }

                    for(int p = 0; p < RTFACT__FILTER_WIDTH * 2; ++p)
                    {
                        int offset = yWeights[y].firstTexel + p;

                        switch(mWrap)
                        {
                        case WRAP_REPEAT:
                            offset = Math::mod(offset, mOrigResY);
                            break;
                        case WRAP_CLAMP:
                            offset = Math::clamp(offset, 0, (int)mOrigResY - 1);
                            break;
                        }

                        if(offset >= 0)
                        {
                            for(uint c = 0; c < mComponentCount; ++c)
                            {
                                temp[yc + c] +=
                                    yWeights[y].weight[p] *
                                    tempBuffers[0][offset * lineWidth + xc + c];
                            }
                        }
                    }
                }

                for(uint y = 0; y < mResY; ++y)
                {
                    const uint yc = y * mComponentCount;

                    for(uint c = 0; c < mComponentCount; ++c)
                    {
                        tempBuffers[0][y * lineWidth + xc + c] =
                            Math::clamp(temp[yc + c], 0.f, RTFACT_INFINITY);
                    }
                }
            }
        }

        mLevelCount = 1 + static_cast<uint>(
            Math::log2Int(static_cast<float>(Math::max(mResX, mResY))));

        mLevels.resize(mLevelCount);

        const uint componentSize = sizeof(float);

        mLevels[0].init(mResX, mResY, mComponentCount * componentSize);

        for(uint y = 0; y < mResY; ++y)
        {
            for(uint x = 0; x < mResX; ++x)
            {
                const uint xc = x * mComponentCount;

                float* ptr = mLevels[0].get<float>(x, y);

                for(uint c = 0; c < mComponentCount; ++c)
                {
                    *(ptr + c) = tempBuffers[0][y * lineWidth + xc + c];
                }
            }
        }

        uint r = 0, w = 1;

        for(uint l = 1; l < mLevelCount; ++l)
        {
            const uint resX = Math::max(1u, mLevels[l - 1].getResX() / 2);
            const uint resY = Math::max(1u, mLevels[l - 1].getResY() / 2);
            const uint lineWidth = resX * mComponentCount;

            mLevels[l].init(resX, resY, mComponentCount * componentSize);

            const uint rResX = resX * 2;
            const uint rResY = resY * 2;
            const uint rLineWidth = rResX * mComponentCount;

            for(uint y = 0; y < resY; ++y)
            {
                for(uint x = 0; x < resX; ++x)
                {
                    const uint xw = x * mComponentCount;
                    const uint yw = y;

                    uint xr = 2 * x, yr = 2 * y;
                    uint x1r, y1r;

                    switch(mWrap)
                    {
                    case WRAP_REPEAT:
                        x1r = Math::mod(xr + 1, rResX);
                        y1r = Math::mod(yr + 1, rResY);
                        break;
                    case WRAP_CLAMP:
                        x1r = Math::clamp(xr + 1, 0u, rResX - 1);
                        y1r = Math::clamp(yr + 1, 0u, rResY - 1);
                        break;
                    }

                    xr *= mComponentCount;
                    x1r *= mComponentCount;

                    for(uint c = 0; c < mComponentCount; ++c)
                    {
                        tempBuffers[w][yw * lineWidth + xw + c] = 0.25f * (
                            tempBuffers[r][yr  * rLineWidth + xr  + c] +
                            tempBuffers[r][yr  * rLineWidth + x1r + c] +
                            tempBuffers[r][y1r * rLineWidth + xr  + c] +
                            tempBuffers[r][y1r * rLineWidth + x1r + c]);

                        *(mLevels[l].get<float>(x, y) + c) =
                                tempBuffers[w][yw * lineWidth + xw + c];
                    }
                }
            }

            std::swap(r, w);
        }
    }

    /*const BasicImage2D<float> getImage(
        const uint aLevel = 0) const
    {
        RTFACT_ASSERT(aLevel < mLevelCount);

        Image2D ret(
            mComponentType, mComponentCount,
            mLevels[aLevel].getResX(), mLevels[aLevel].getResY());

        for(uint y = 0; y < mLevels[aLevel].getResY(); ++y)
        {
            for(uint x = 0; x < mLevels[aLevel].getResX(); ++x)
            {
                for(uint c = 0; c < mComponentCount; ++c)
                {
                    float temp = *(mLevels[aLevel].get<float>(x, y) + c);

                    switch(mComponentType)
                    {
                    case IMAGE_UBYTE:
                        *(ret.get<ubyte>(x, y) + c) = *(mLevels[aLevel].get<ubyte>(x, y) + c);
                        break;
                    case IMAGE_FLOAT:
                        *(ret.get<float>(x, y) + c) = *(mLevels[aLevel].get<float>(x, y) + c);
                        break;
                    }
                }
            }
        }

        return ret;
    }*/

    template<
        bool taGetColor,
        bool taGetAlpha>
    void lookup(
        const Vec2f1& aCoords,
        Vec3f1* oColor,
        float* oAlpha,
        const float aWidth = 0.f) const
    {
        RTFACT_IMAGE_ASSERT(!taGetAlpha || mComponentCount >= 4);

        const float level = mLevelCount - 1 + Math::log2(Math::max(aWidth, 1e-8f));

        if(level <= 0)
        {
            lookup<taGetColor,taGetAlpha>(aCoords, 0, oColor, oAlpha);
        }
        else if(level >= mLevelCount - 1)
        {
            const float* ptr = mLevels[mLevelCount - 1].get<float>(0, 0);

            if(taGetColor)
            {
                oColor->x = ptr[0];
                oColor->y = ptr[1];
                oColor->z = ptr[2];
            }

            if(taGetAlpha)
            {
                *oAlpha = ptr[3];
            }
        }
        else
        {
            const int iLevel = Math::floorToInt(level);
            const float delta = level - iLevel;
            const float deltaOpp = 1.f - delta;

            Vec3f1 color0, color1;
            float alpha0, alpha1;

            lookupPrivate<taGetColor, taGetAlpha>(aCoords, iLevel, &color0, &alpha0);
            lookupPrivate<taGetColor, taGetAlpha>(aCoords, iLevel + 1, &color1, &alpha1);

            if(taGetColor)
            {
                *oColor = deltaOpp * color0 + delta * color1;
            }

            if(taGetAlpha)
            {
                *oAlpha = deltaOpp * alpha0 + delta * alpha1;
            }
        }
    }

    template<
        bool taGetColor,
        bool taGetAlpha>
    void lookup(
        const Vec2f1& aCoords,
        const uint aLevel,
        Vec3f1* oColor,
        float* oAlpha) const
    {
        RTFACT_IMAGE_ASSERT(!taGetAlpha || mComponentCount >= 4);

        lookupPrivate<taGetColor, taGetAlpha>(
                aCoords, aLevel, oColor, oAlpha);
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__FILTER_WIDTH

#endif // RTFACT__MIPMAP_HPP
