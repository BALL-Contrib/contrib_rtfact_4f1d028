#ifndef RTFACT__RANGE_HPP
#define RTFACT__RANGE_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tType>
struct RangeBase
{
    tType begin;
    tType end;

    RangeBase()
    {}

    RangeBase(
        const tType aBegin,
        const tType aEnd
    ) :
        begin(aBegin),
        end(aEnd)
    {}

    const tType operator[](
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 2);

        return *(&begin + aIndex);
    }

    tType& operator[](
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 2);

        return *(&begin + aIndex);
    }
};

template<
    class tType>
struct RangeBase2D
{
    RangeBase<tType> x;
    RangeBase<tType> y;

    RangeBase2D()
    {}

    RangeBase2D(
        const tType aBeginX,
        const tType aEndX,
        const tType aBeginY,
        const tType aEndY
    ) :
        x(aBeginX, aEndX),
        y(aBeginY, aEndY)
    {}

    RangeBase2D(
        RangeBase<tType> aX,
        RangeBase<tType> aY
    ) :
        x(aX),
        y(aY)
    {}

    const tType operator[](
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 4);

        return *(&x.begin + aIndex);
    }

    tType& operator[](
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 4);

        return *(&x.begin + aIndex);
    }
};

template<
    class tType>
struct RangeBase3D
{
    RangeBase<tType> x;
    RangeBase<tType> y;
    RangeBase<tType> z;

    RangeBase3D()
    {}

    RangeBase3D(
        const tType aBeginX,
        const tType aEndX,
        const tType aBeginY,
        const tType aEndY,
        const tType aBeginZ,
        const tType aEndZ
    ) :
        x(aBeginX, aEndX),
        y(aBeginY, aEndY),
        z(aBeginZ, aEndZ)
    {}

    RangeBase3D(
        RangeBase<tType> aX,
        RangeBase<tType> aY,
        RangeBase<tType> aZ
    ) :
        x(aX),
        y(aY),
        z(aZ)
    {}

    const tType operator[](
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 6);

        return *(&x.begin + aIndex);
    }

    tType& operator[](
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 6);

        return *(&x.begin + aIndex);
    }
};

typedef RangeBase<uint> Range;
typedef RangeBase<float> Rangef;

typedef RangeBase2D<uint> Range2D;
typedef RangeBase2D<float> Range2Df;

typedef RangeBase3D<uint> Range3D;
typedef RangeBase3D<float> Range3Df;

RTFACT_NAMESPACE_END

#endif // RTFACT__RANGE_HPP
