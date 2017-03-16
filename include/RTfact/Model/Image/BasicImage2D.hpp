/* 
 *  BasicImage2D.hpp
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
 *  Created on: 2008-07-16 17:43:42 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */
 
 #ifndef RTFACT__BASICIMAGE2D_HPP
#define RTFACT__BASICIMAGE2D_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Image/Image2D.hpp>
#include <RTfact/Utils/IO/SerializationUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tComponent>
class BasicImage2D :
    public Image2D
{
public:

    typedef tComponent Component;

protected:

    Component* mData;
    bool mOwnBuffer;
    uint mComponentCount;
    uint mResX;
    uint mResY;
    uint mLineWidth;

public: 

    BasicImage2D() :
        mData(RTFACT_NULL),
        mOwnBuffer(true),
        mComponentCount(0),
        mResX(0),
        mResY(0),
        mLineWidth(0)
    {}

    BasicImage2D(
        const uint aComponentCount,
        const uint aResX,
        const uint aResY
    ) :
        mData(RTFACT_NULL)
    {
        init(aComponentCount, aResX, aResY);
    }

    BasicImage2D(
        const Component* aBuffer,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
        const bool aCopyData = true
    ) :
        mData(RTFACT_NULL)
    {
        init(aBuffer, aComponentCount, aResX, aResY, aCopyData);
    }

    BasicImage2D(
        const Component* aBuffer,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
				const uint aLineWidth,
        const bool aCopyData = true
    ) :
        mData(RTFACT_NULL)
    {
        init(aBuffer, aComponentCount, aResX, aResY, aLineWidth, aCopyData);
    }

    BasicImage2D(
        const BasicImage2D<tComponent>& aOther
    ) :
        mData(RTFACT_NULL)
    {
        init(
            aOther.mData, aOther.mComponentCount,
            aOther.mResX, aOther.mResY, aOther.mLineWidth, aOther.mOwnBuffer);
    }

    ~BasicImage2D()
    {
        clear();
    }

    uint getComponentCount() const
    {
        return mComponentCount;
    }

    uint getPitch() const
    {
        return mLineWidth * sizeof(Component);
    }

    void init(
        const uint aComponentCount,
        const uint aResX,
        const uint aResY)
    {
        clear();

        mOwnBuffer = true;
        mComponentCount = aComponentCount;
        mResX = aResX;
        mResY = aResY;
        mLineWidth = mResX * mComponentCount;
        mData = new Component[mLineWidth * mResY];
    }

    void init(
        const Component* aBuffer,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
        const bool aCopyData = true)
    {
        clear();

        mOwnBuffer = aCopyData;
        mComponentCount = aComponentCount;
        mResX = aResX;
        mResY = aResY;
        mLineWidth = mResX * mComponentCount;

        if(mOwnBuffer)
        {
            mData = new Component[mLineWidth * mResY];
            memcpy(mData, aBuffer, mResY * getPitch());
        }
        else
        {
            mData = const_cast<Component*>(aBuffer);
        }
    }

    void init(
        const Component* aBuffer,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
				const uint aLineWidth,
        const bool aCopyData = true)
    {
        clear();

        mOwnBuffer = aCopyData;
        mComponentCount = aComponentCount;
        mResX = aResX;
        mResY = aResY;
        mLineWidth = aLineWidth * mComponentCount;

        if(mOwnBuffer)
        {
            mData = new Component[mLineWidth * mResY];
            memcpy(mData, aBuffer, mResY * getPitch());
        }
        else
        {
            mData = const_cast<Component*>(aBuffer);
        }
    }

    void clear()
    {
        if(mData != RTFACT_NULL && mOwnBuffer)
        {
            delete [] mData;
        }

        mData = RTFACT_NULL;
    }

    RTFACT_IMAGE_INLINE uint getResX() const
    {
        return mResX;
    }

    RTFACT_IMAGE_INLINE uint getResY() const
    {
        return mResY;
    }

    RTFACT_IMAGE_INLINE const Component* operator()(
        const uint aX,
        const uint aY) const
    {
        RTFACT_IMAGE_ASSERT(mData != RTFACT_NULL);

        return mData + aY * mLineWidth + aX * mComponentCount;
    }

    RTFACT_IMAGE_INLINE Component* operator()(
        const uint aX,
        const uint aY)
    {
        RTFACT_IMAGE_ASSERT(mData != RTFACT_NULL);

        return mData + aY * mLineWidth + aX * mComponentCount;
    }

    RTFACT_IMAGE_INLINE const Component* getFirstComponent(
        const uint aX,
        const uint aY) const
    {
        RTFACT_IMAGE_ASSERT(mData != RTFACT_NULL);

        return mData + aY * mLineWidth + aX * mComponentCount;
    }

    RTFACT_IMAGE_INLINE Component* getFirstComponent(
        const uint aX,
        const uint aY)
    {
        RTFACT_IMAGE_ASSERT(mData != RTFACT_NULL);

        return mData + aY * mLineWidth + aX * mComponentCount;
    }

    void save(
        IO::Serializer& aSerializer)
    {
        aSerializer << mComponentCount << mResX << mResY;

        const uint bufferSize = mLineWidth * mResY;

        for(uint i = 0; i < bufferSize; ++i)
        {
            aSerializer << mData[i];
        }
    }

    void load(
        IO::Serializer& aSerializer)
    {
        clear();

        aSerializer >> mComponentCount >> mResX >> mResY;

        mOwnBuffer = true;
        mLineWidth = mResX * mComponentCount;
        mData = new Component[mResY * mLineWidth];

        const uint bufferSize = mLineWidth * mResY;

        for(uint i = 0; i < bufferSize; ++i)
        {
            aSerializer >> mData[i];
        }
    }
};

RTFACT_NAMESPACE_END

RTFACT_SERIALIZATION_REGISTER_LOAD_SAVE_METHODS_TEMPLATE_CLASS(
    RTfact::BasicImage2D, (class))

#endif // RTFACT__BASICIMAGE2D_HPP
