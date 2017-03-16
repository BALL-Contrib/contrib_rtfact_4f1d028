#ifndef RTFACT__REFBLOCKEDARRAY2D_HPP
#define RTFACT__REFBLOCKEDARRAY2D_HPP

#include <RTfact/Config/Common.hpp>
#include <boost/shared_array.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tElement,
    uint taLogBlockSize = 2>
struct RefBlockedArray2D
{
public:

    typedef tElement          value_type;
    typedef const value_type& const_reference;
    typedef uint              size_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef ptrdiff_t         difference_type;

protected:

    enum
    {
        LOG_BLOCK_SIZE = taLogBlockSize,
        BLOCK_SIZE = 1 << LOG_BLOCK_SIZE,
        BLOCK_SIZE2 = BLOCK_SIZE * BLOCK_SIZE,
        BLOCK_ID_SHIFT = LOG_BLOCK_SIZE,
        BLOCK_OFFSET_MASK = BLOCK_SIZE - 1,
    };

    typedef boost::shared_array<tElement> t_Buffer;
    typedef Detail::MemoryDeallocator<tElement, true> t_Deallocator;

    uint roundUp(
        const uint aSize) const
    {
        return (aSize + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);
    }

    t_Buffer mBuffer;
    size_type mResX, mResY;
    size_type mBlockResX, mBlockResY;
    size_type mComponentCount;

public:

    RefBlockedArray2D() :
        mBuffer(RTFACT_NULL),
        mResX(0),
        mResY(0),
        mBlockResX(0),
        mBlockResY(0),
        mComponentCount(0)
    {}

    RefBlockedArray2D(
        const size_type aResX,
        const size_type aResY,
        const size_type aComponentCount,
        const_pointer aData = RTFACT_NULL)
    {
        init(aResX, aResY, aComponentCount, aData);
    }

    RefBlockedArray2D(
        const RefBlockedArray2D<tElement, taLogBlockSize>& aOther
    ) :
        mBuffer(aOther.mBuffer),
        mResX(aOther.mResX),
        mResY(aOther.mResY),
        mBlockResX(aOther.mBlockResX),
        mBlockResY(aOther.mBlockResY),
        mComponentCount(aOther.mComponentCount)
    {}

    RefBlockedArray2D(
        const RefBlockedArray2D<tElement, taLogBlockSize>& aOther,
        const bool aCopy)
    {
        mResX = aOther.mResX;
        mResY = aOther.mResY;
        mBlockResX = aOther.mBlockResX;
        mBlockResY = aOther.mBlockResY;
        mComponentCount = aOther.mComponentCount;

        if(aCopy)
        {
            const uint size = mBlockResX * mBlockResY * mComponentCount * BLOCK_SIZE2;
            pointer data = new value_type[size];

            Detail::fastCopy(aOther.mBuffer.get(), aOther.mBuffer.get() + size, data);

            mBuffer = t_Buffer(data, t_Deallocator());
        }
        else
        {
            mBuffer = aOther.mBuffer;
        }
    }

    void init(
        const size_type aResX,
        const size_type aResY,
        const size_type aComponentCount,
        const_pointer aData = RTFACT_NULL)
    {
        mResX = aResX;
        mResY = aResY;
        mBlockResX = roundUp(mResX) >> LOG_BLOCK_SIZE;
        mBlockResY = roundUp(mResY) >> LOG_BLOCK_SIZE;
        mComponentCount = aComponentCount;

        const uint size = mBlockResX * mBlockResY * mComponentCount * BLOCK_SIZE2;

        mBuffer = t_Buffer(new value_type[size], t_Deallocator());

        if(aData)
        {
            for(uint y = 0, i = 0; y < mResY; ++y)
            {
                for(uint x = 0; x < mResX; ++x)
                {
                    pointer p = (*this)(x, y);

                    for(uint c = 0; c < mComponentCount; ++c, ++i)
                    {
                        *(p + c) = aData[i];
                    }
                }
            }
        }
    }

    void clear()
    {
        mBuffer = t_Buffer(RTFACT_NULL);
    }

    RTFACT_CONTAINER_INLINE operator bool() const
    {
        return mBuffer;
    }

    RTFACT_CONTAINER_INLINE size_type getResX() const
    {
        return mResX;
    }

    RTFACT_CONTAINER_INLINE size_type getResY() const
    {
        return mResY;
    }

    RTFACT_CONTAINER_INLINE size_type getComponentCount() const
    {
        return mComponentCount;
    }

    template<
        class tComponentType>
    RTFACT_CONTAINER_INLINE const tComponentType* get(
        const size_type aX,
        const size_type aY) const
    {
        RTFACT_CONTAINER_ASSERT(aX < mResX && aY < mResY);

        const uint blockX = aX >> BLOCK_ID_SHIFT;
        const uint blockY = aY >> BLOCK_ID_SHIFT;
        const uint offsetX = aX & BLOCK_OFFSET_MASK;
        const uint offsetY = aY & BLOCK_OFFSET_MASK;

        const uint blockIndex = BLOCK_SIZE2 * (mBlockResX * blockY + blockX);
        const uint elementIndex = BLOCK_SIZE * offsetY + offsetX;

        return reinterpret_cast<const tComponentType*>(
            mBuffer.get() + (blockIndex + elementIndex) * mComponentCount);
    }

    template<
        class tComponentType>
    RTFACT_CONTAINER_INLINE tComponentType* get(
        const size_type aX,
        const size_type aY)
    {
        RTFACT_CONTAINER_ASSERT(aX < mResX && aY < mResY);

        const uint blockX = aX >> BLOCK_ID_SHIFT;
        const uint blockY = aY >> BLOCK_ID_SHIFT;
        const uint offsetX = aX & BLOCK_OFFSET_MASK;
        const uint offsetY = aY & BLOCK_OFFSET_MASK;

        const uint blockIndex = BLOCK_SIZE2 * (mBlockResX * blockY + blockX);
        const uint elementIndex = BLOCK_SIZE * offsetY + offsetX;

        return reinterpret_cast<tComponentType*>(
            mBuffer.get() + (blockIndex + elementIndex) * mComponentCount);
    }

    RTFACT_FORCE_INLINE const_pointer operator()(
        const size_type aX,
        const size_type aY) const
    {
        return get<value_type>(aX, aY);
    }

    RTFACT_FORCE_INLINE pointer operator()(
        const size_type aX,
        const size_type aY)
    {
        return get<value_type>(aX, aY);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__REFBLOCKEDARRAY2D_HPP
