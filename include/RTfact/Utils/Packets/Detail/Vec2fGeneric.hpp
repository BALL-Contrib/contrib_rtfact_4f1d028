#ifndef RTFACT__VEC2FGENERIC_HPP
#define RTFACT__VEC2FGENERIC_HPP

RTFACT_NAMESPACE_BEGIN

template<
    uint taSize>
class Vec2f
{
    // Size can only be greater or equal than 16 and a multiple of 4
    RTFACT_VEC2F_STATIC_ASSERT(( (taSize >= 16) && (taSize % 4 == 0) ));

    // Uncomment to restrict the SIZE to a power of 4 and greater than or equal to 16
    //RTFACT_VEC2F_STATIC_ASSERT(( taSize >= 16 && ((taSize & (taSize - 1)) == 0) && (taSize & 0x55555555) ));

    typedef Vec2f<taSize> t_This;

public:

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef Packet<taSize, float> FloatPacket;
    typedef Packet<taSize, int>   IntPacket;
    typedef float4                Data;
    typedef Vec2f<4>              Container;

    Container containers[CONTAINER_COUNT];

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    #pragma region Constants

    //RTFACT_VEC2F_DECLARE_CONSTS

    static RTFACT_VEC2F_INLINE const Vec2f<taSize> C_0_0()
    {
        Vec2f<taSize> ret;

        ret.containers[0] = Container::C_0_0();

        for(uint i = 1; i < CONTAINER_COUNT; ++i)
        {
            ret.containers[i] = ret.containers[0];
        }

        return ret;
    }

    #pragma endregion

    #pragma region Accessors

    RTFACT_VEC2F_INLINE float xAt(
        uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return containers[aIndex / 4].xAt(aIndex % 4);
    }

    RTFACT_VEC2F_INLINE float& xAt(
        uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return containers[aIndex / 4].xAt(aIndex % 4);
    }

    RTFACT_VEC2F_INLINE float yAt(
        uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return containers[aIndex / 4].yAt(aIndex % 4);
    }

    RTFACT_VEC2F_INLINE float& yAt(
        uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return containers[aIndex / 4].yAt(aIndex % 4);
    }

    RTFACT_VEC2F_INLINE void set(
        const uint aIndex,
        const Vec2f<1>& aVec)
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        containers[aIndex / 4].set(aIndex % 4, aVec);
    }

    RTFACT_VEC2F_INLINE const Vec2f<1> operator[](
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < SIZE);

        return Vec2f<1>(xAt(aIndex), yAt(aIndex));
    }

    RTFACT_VEC2F_INLINE const Container& operator()(
        const uint aIndex) const
    {
        RTFACT_VEC2F_ASSERT(aIndex < CONTAINER_COUNT);

        return containers[aIndex];
    }

    RTFACT_VEC2F_INLINE Container& operator()(
        const uint aIndex)
    {
        RTFACT_VEC2F_ASSERT(aIndex < CONTAINER_COUNT);

        return containers[aIndex];
    }

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_VEC2F_INLINE const Vec2f<taSize> replicate(
        const float aX,
        const float aY)
    {
        const Container temp = Container::replicate(aX, aY);

        Vec2f<taSize> ret;

        for(uint i = 0; i < CONTAINER_COUNT; ++i)
        {
            ret.containers[i] = temp;
        }

        return ret;
    }

    static RTFACT_VEC2F_INLINE const Vec2f<taSize> replicate(
        const Vec2f<1>& aVec)
    {
        const Container temp = Container::replicate(aVec);

        Vec2f<taSize> ret;

        for(uint i = 0; i < CONTAINER_COUNT; ++i)
        {
            ret.containers[i] = temp;
        }

        return ret;
    }

    #pragma endregion
};

RTFACT_NAMESPACE_END

#endif // RTFACT__VEC2FGENERIC_HPP
