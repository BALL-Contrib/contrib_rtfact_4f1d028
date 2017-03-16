/* 
 *  BitMask.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__BITMASK_HPP
#define RTFACT__BITMASK_HPP

#include <RTfact/Config/Common.hpp>

#define RTFACT__BIT_MASK_ASSERT(x)        RTFACT_ASSERT(x)
#define RTFACT__BIT_MASK_STATIC_ASSERT(x) RTFACT_STATIC_ASSERT(x)

RTFACT_NAMESPACE_BEGIN

namespace Detail {

// forward declaration
template<
    uint taSize,
    uint taDataSize>
class BitMaskBase;

#pragma region BitMaskBase<1, 32> class definition

#pragma region Operator definition macros

#define RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_OP(aOperator)                     \
    RTFACT_BIT_MASK_INLINE const t_This operator aOperator(                    \
        const t_This& aBitMask) const                                          \
    {                                                                          \
        return t_This(mMask aOperator aBitMask.mMask);                         \
    }

#define RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_ASSIGN_OP(aOperator)              \
    RTFACT_BIT_MASK_INLINE t_This& operator aOperator##=(                      \
        const t_This& aBitMask)                                                \
    {                                                                          \
        mMask aOperator##= aBitMask.mMask;                                     \
        return *this;                                                          \
    }

#pragma endregion

#pragma region Class definition

template<>
class BitMaskBase<1, 32>
{
public:

    enum
    {
        SIZE = 1,
        SIZE_SQRT = 1,
        CONTAINER_COUNT = 1
    };

    typedef uint Data;

private:

    Data mMask;

    #define RTFACT__SINGLE_32_BIT_MASK_BASE_C_TRUE  static_cast<Data>(1)
    #define RTFACT__SINGLE_32_BIT_MASK_BASE_C_FALSE static_cast<Data>(0)

    typedef BitMaskBase<1, 32> t_This;

protected:

    RTFACT_BIT_MASK_INLINE BitMaskBase()
    {}

    RTFACT_BIT_MASK_INLINE explicit BitMaskBase(
        const Data aMask
    ) :
        mMask(aMask)
    {
        RTFACT__BIT_MASK_ASSERT(aMask >> 1 == 0);
    }

public:

    RTFACT_BIT_MASK_INLINE void setFalse()
    {
        mMask = RTFACT__SINGLE_32_BIT_MASK_BASE_C_FALSE;
    }

    RTFACT_BIT_MASK_INLINE const t_This operator~() const
    {
        return t_This(!mMask);
    }

    RTFACT_BIT_MASK_INLINE uint get(
        const uint aStartIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aStartIndex == 0);

        return mMask;
    }

    RTFACT_BIT_MASK_INLINE uint getContainer(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex == 0);

        return mMask;
    }

    RTFACT_BIT_MASK_INLINE void set(
        const uint aStartIndex,
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aStartIndex == 0 && aBit >> 1 == 0 &&
                              mMask == RTFACT__SINGLE_32_BIT_MASK_BASE_C_FALSE);

        mMask |= aBit;
    }

    RTFACT_BIT_MASK_INLINE void setContainer(
        const uint aIndex,
        const uint aValue)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex == 0 && aValue >> 1 == 0 &&
                              mMask == RTFACT__SINGLE_32_BIT_MASK_BASE_C_FALSE);

        mMask |= aValue;
    }

    RTFACT_BIT_MASK_INLINE bool isTrue() const
    {
        return (mMask == RTFACT__SINGLE_32_BIT_MASK_BASE_C_TRUE);
    }

    RTFACT_BIT_MASK_INLINE bool isFalse() const
    {
        return (mMask == RTFACT__SINGLE_32_BIT_MASK_BASE_C_FALSE);
    }

    static RTFACT_BIT_MASK_INLINE const t_This replicate(
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aBit >> 1 == 0);

        return t_This(aBit);
    }

    RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_OP       (|)
    RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_ASSIGN_OP(|)
    RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_OP       (&)
    RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_ASSIGN_OP(&)
    RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_OP       (^)
    RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_ASSIGN_OP(^)
};

#pragma endregion

#pragma region Undefine helper macros

#undef RTFACT__SINGLE_32_BIT_MASK_BASE_C_TRUE
#undef RTFACT__SINGLE_32_BIT_MASK_BASE_C_FALSE

#undef RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_OP
#undef RTFACT__SINGLE_32_BIT_MASK_BASE_BOOL_ASSIGN_OP

#pragma endregion

#pragma endregion

#pragma region BitMaskBase<taSize, 32> class definition

#pragma region Operator definition macros

#define RTFACT__32_BIT_MASK_BASE_BOOL_OP(aOperator)                            \
    RTFACT_BIT_MASK_INLINE t_This operator aOperator(                          \
        const t_This& aBitMask) const                                          \
    {                                                                          \
        return t_This(mMask aOperator aBitMask.mMask);                         \
    }

#define RTFACT__32_BIT_MASK_BASE_BOOL_ASSIGN_OP(aOperator)                     \
    RTFACT_BIT_MASK_INLINE t_This& operator aOperator##=(                      \
        const t_This& aBitMask)                                                \
    {                                                                          \
        mMask aOperator##= aBitMask.mMask;                                     \
        return *this;                                                          \
    }

#pragma endregion

#pragma region Class definition

template<
    uint taSize>
class BitMaskBase<taSize, 32>
{
    RTFACT__BIT_MASK_STATIC_ASSERT(taSize > 1 && taSize <= 32);

public:

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef uint Data;

private:

    Data mMask;

    #define RTFACT__32_BIT_MASK_BASE_C_TRUE  ((static_cast<Data>(1) << SIZE) - 1)
    #define RTFACT__32_BIT_MASK_BASE_C_FALSE static_cast<Data>(0)

    typedef BitMaskBase<taSize, 32> t_This;

protected:

    RTFACT_BIT_MASK_INLINE BitMaskBase()
    {}

    RTFACT_BIT_MASK_INLINE explicit BitMaskBase(
        const Data aMask
    ) :
        mMask(aMask)
    {
        RTFACT__BIT_MASK_ASSERT(aMask >> SIZE == 0);
    }

public:

    RTFACT_BIT_MASK_INLINE void setFalse()
    {
        mMask = RTFACT__32_BIT_MASK_BASE_C_FALSE;
    }

    RTFACT_BIT_MASK_INLINE const t_This operator~() const
    {
        return t_This(mMask ^ RTFACT__32_BIT_MASK_BASE_C_TRUE);
    }

    RTFACT_BIT_MASK_INLINE uint get(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < SIZE);

        return (mMask >> aIndex) & 1;
    }

    RTFACT_BIT_MASK_INLINE uint getContainer(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < CONTAINER_COUNT);

        return (mMask >> (aIndex << 2)) & 0xF;
    }

    RTFACT_BIT_MASK_INLINE void set(
        const uint aIndex,
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < SIZE && aBit >> 1 == 0 && get(aIndex));

        mMask |= (aBit << aIndex);
    }

    RTFACT_BIT_MASK_INLINE void setContainer(
        const uint aIndex,
        const uint aValue)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < CONTAINER_COUNT &&
                              aValue >> 4 == 0 && getContainer(aIndex) == 0);

        mMask |= (aValue << (aIndex << 2));
    }

    RTFACT_BIT_MASK_INLINE bool isTrue() const
    {
        return (mMask == RTFACT__32_BIT_MASK_BASE_C_TRUE);
    }

    RTFACT_BIT_MASK_INLINE bool isFalse() const
    {
        return (mMask == RTFACT__32_BIT_MASK_BASE_C_FALSE);
    }

    static RTFACT_BIT_MASK_INLINE const t_This replicate(
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aBit >> 1 == 0);

        return t_This(aBit ? RTFACT__32_BIT_MASK_BASE_C_TRUE :
                             RTFACT__32_BIT_MASK_BASE_C_FALSE);
    }

    RTFACT__32_BIT_MASK_BASE_BOOL_OP       (|)
    RTFACT__32_BIT_MASK_BASE_BOOL_ASSIGN_OP(|)
    RTFACT__32_BIT_MASK_BASE_BOOL_OP       (&)
    RTFACT__32_BIT_MASK_BASE_BOOL_ASSIGN_OP(&)
    RTFACT__32_BIT_MASK_BASE_BOOL_OP       (^)
    RTFACT__32_BIT_MASK_BASE_BOOL_ASSIGN_OP(^)
};

#pragma endregion

#pragma region Undefine helper macros

#undef RTFACT__32_BIT_MASK_BASE_C_TRUE
#undef RTFACT__32_BIT_MASK_BASE_C_FALSE

#undef RTFACT__32_BIT_MASK_BASE_BOOL_OP
#undef RTFACT__32_BIT_MASK_BASE_BOOL_ASSIGN_OP

#pragma endregion

#pragma endregion

#pragma region BitMaskBase<taSize, 64> class definition

#pragma region Operator definition macros

#define RTFACT__64_BIT_MASK_BASE_BOOL_OP(aOperator)                            \
    RTFACT_BIT_MASK_INLINE const t_This operator aOperator(                    \
        const t_This& aBitMask) const                                          \
    {                                                                          \
        return t_This(mMask aOperator aBitMask.mMask);                         \
    }

#define RTFACT__64_BIT_MASK_BASE_BOOL_ASSIGN_OP(aOperator)                     \
    RTFACT_BIT_MASK_INLINE t_This& operator aOperator##=(                      \
        const t_This& aBitMask)                                                \
    {                                                                          \
        mMask aOperator##= aBitMask.mMask;                                     \
        return *this;                                                          \
    }

#pragma endregion

#pragma region Class definition

template<
    uint taSize>
class BitMaskBase<taSize, 64>
{
    RTFACT__BIT_MASK_STATIC_ASSERT(taSize > 32 && taSize <= 64);

public:

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef uint64 Data;

private:

    Data mMask;

    #define RTFACT__64_BIT_MASK_BASE_C_TRUE  ((static_cast<Data>(1) << SIZE) - 1)
    #define RTFACT__64_BIT_MASK_BASE_C_FALSE static_cast<Data>(0)

    typedef BitMaskBase<taSize, 64> t_This;

protected:

    RTFACT_BIT_MASK_INLINE BitMaskBase()
    {}

    RTFACT_BIT_MASK_INLINE explicit BitMaskBase(
        const Data aMask
    ) :
        mMask(aMask)
    {
        RTFACT__BIT_MASK_ASSERT(aMask >> SIZE == 0);
    }

public:

    RTFACT_BIT_MASK_INLINE void setFalse()
    {
        mMask = RTFACT__64_BIT_MASK_BASE_C_FALSE;
    }

    RTFACT_BIT_MASK_INLINE const t_This operator~() const
    {
        return t_This(mMask ^ RTFACT__64_BIT_MASK_BASE_C_TRUE);
    }

    RTFACT_BIT_MASK_INLINE uint get(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < SIZE);

        return static_cast<uint>(mMask >> aIndex) & 1;
    }

    RTFACT_BIT_MASK_INLINE uint getContainer(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < CONTAINER_COUNT);

        return static_cast<uint>(mMask >> (aIndex << 2)) & 0xF;
    }

    RTFACT_BIT_MASK_INLINE void set(
        const uint aIndex,
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < SIZE &&
                              aBit >> 1 == 0 && get(aIndex) == 0);

        mMask |= (static_cast<Data>(aBit) << aIndex);
    }

    RTFACT_BIT_MASK_INLINE void setContainer(
        const uint aIndex,
        const uint aValue)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < CONTAINER_COUNT &&
                              aValue >> 4 == 0 && getContainer(aIndex) == 0);

        mMask |= (static_cast<Data>(aValue) << (aIndex << 2));
    }

    RTFACT_BIT_MASK_INLINE bool isTrue() const
    {
        return (mMask == RTFACT__64_BIT_MASK_BASE_C_TRUE);
    }

    RTFACT_BIT_MASK_INLINE bool isFalse() const
    {
        return (mMask == RTFACT__64_BIT_MASK_BASE_C_FALSE);
    }

    static RTFACT_BIT_MASK_INLINE const t_This replicate(
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aBit >> 1 == 0);

        return t_This(aBit ? RTFACT__64_BIT_MASK_BASE_C_TRUE :
                             RTFACT__64_BIT_MASK_BASE_C_FALSE);
    }

    RTFACT__64_BIT_MASK_BASE_BOOL_OP       (|)
    RTFACT__64_BIT_MASK_BASE_BOOL_ASSIGN_OP(|)
    RTFACT__64_BIT_MASK_BASE_BOOL_OP       (&)
    RTFACT__64_BIT_MASK_BASE_BOOL_ASSIGN_OP(&)
    RTFACT__64_BIT_MASK_BASE_BOOL_OP       (^)
    RTFACT__64_BIT_MASK_BASE_BOOL_ASSIGN_OP(^)
};

#pragma endregion

#pragma region Undefine helper macros

#undef RTFACT__64_BIT_MASK_BASE_C_TRUE
#undef RTFACT__64_BIT_MASK_BASE_C_FALSE

#undef RTFACT__64_BIT_MASK_BASE_BOOL_OP
#undef RTFACT__64_BIT_MASK_BASE_BOOL_ASSIGN_OP

#pragma endregion

#pragma endregion

#pragma region General BitMaskBase class definition

#pragma region Operator definition macros

#define RTFACT__GENERAL_BIT_MASK_BASE_BOOL_OP(aOperator)                       \
    RTFACT_BIT_MASK_INLINE const t_This operator aOperator(                    \
        const t_This& aBitMask) const                                          \
    {                                                                          \
        t_This ret;                                                            \
        for(uint i = 0; i < CONTAINER_COUNT; ++i)                              \
        {                                                                      \
            ret.mMasks[i] = mMasks[i] aOperator aBitMask.mMasks[i];            \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__GENERAL_BIT_MASK_BASE_BOOL_ASSIGN_OP(aOperator)                \
    RTFACT_BIT_MASK_INLINE t_This& operator aOperator##=(                      \
        const t_This& aBitMask)                                                \
    {                                                                          \
        for(uint i = 0; i < CONTAINER_COUNT; ++i)                              \
        {                                                                      \
            mMasks[i] aOperator##= aBitMask.mMasks[i];                         \
        }                                                                      \
        return *this;                                                          \
    }

#pragma endregion

#pragma region Class definition

template<
    uint taSize,
    uint taDataSize>
class BitMaskBase
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value,
        CONTAINER_COUNT = SIZE / 4
    };

    typedef uint64 Data;

private:

    enum
    {
        DATA_CONTAINER_SIZE = sizeof(Data) * 8,
        DATA_CONTAINER_COUNT = SIZE / DATA_CONTAINER_SIZE,
    };

    RTFACT__BIT_MASK_STATIC_ASSERT(SIZE > 64 && SIZE % DATA_CONTAINER_SIZE == 0);

    Data mMasks[DATA_CONTAINER_COUNT];

    #define RTFACT__GENERAL_BIT_MASK_BASE_C_TRUE  static_cast<Data>(-1)
    #define RTFACT__GENERAL_BIT_MASK_BASE_C_FALSE static_cast<Data>(0)

    typedef BitMaskBase<taSize, taDataSize> t_This;

protected:

    RTFACT_BIT_MASK_INLINE BitMaskBase()
    {}

    RTFACT_BIT_MASK_INLINE explicit BitMaskBase(
        const Data aMask)
    {
        RTFACT_PROGRAM_ERROR_F(
            "The constructor BitMask<%1%>::BitMask<%1%>(const Data) should never be called.",
            SIZE);
    }

public:

    RTFACT_BIT_MASK_INLINE void setFalse()
    {
        for(uint i = 0; i < DATA_CONTAINER_COUNT; ++i)
        {
            mMasks[i] = RTFACT__GENERAL_BIT_MASK_BASE_C_FALSE;
        }
    }

    RTFACT_BIT_MASK_INLINE const t_This operator~() const
    {
        t_This ret;

        for(uint i = 0; i < DATA_CONTAINER_COUNT; ++i)
        {
            ret.mMasks[i] = mMasks[i] ^ RTFACT__GENERAL_BIT_MASK_BASE_C_TRUE;
        }

        return ret;
    }

    RTFACT_BIT_MASK_INLINE uint get(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < SIZE);

        return static_cast<uint>(
            mMasks[aIndex / DATA_CONTAINER_SIZE] >>
                (aIndex % DATA_CONTAINER_SIZE)) & 1;
    }

    RTFACT_BIT_MASK_INLINE uint getContainer(
        const uint aIndex) const
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < CONTAINER_COUNT);

        const uint maskIndex = aIndex << 2;

        return static_cast<uint>(
            mMasks[maskIndex / DATA_CONTAINER_SIZE] >>
                (maskIndex % DATA_CONTAINER_SIZE)) & 0xF;
    }

    RTFACT_BIT_MASK_INLINE void set(
        const uint aIndex,
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex < SIZE && aBit >> 1 == 0 &&
                              get(aIndex) == 0);

        mMasks[aIndex / DATA_CONTAINER_SIZE] |=
            (static_cast<Data>(aBit) << (aIndex % DATA_CONTAINER_SIZE));
    }

    RTFACT_BIT_MASK_INLINE void setContainer(
        const uint aIndex,
        const uint aValue)
    {
        RTFACT__BIT_MASK_ASSERT(aIndex <= CONTAINER_COUNT &&
                              aValue >> 4 == 0 && getContainer(aIndex) == 0);

        const uint maskIndex = aIndex << 2;

        mMasks[maskIndex / DATA_CONTAINER_SIZE] |=
            (static_cast<Data>(aValue) <<
                (maskIndex % DATA_CONTAINER_SIZE));
    }

    RTFACT_BIT_MASK_INLINE bool isTrue() const
    {
        Data mask = mMasks[0];

        for(uint i = 1; i < DATA_CONTAINER_COUNT; ++i)
        {
            mask &= mMasks[i];
        }

        return mask == RTFACT__GENERAL_BIT_MASK_BASE_C_TRUE;
    }

    RTFACT_BIT_MASK_INLINE bool isFalse() const
    {
        Data mask = mMasks[0];

        for(uint i = 1; i < DATA_CONTAINER_COUNT; ++i)
        {
            mask |= mMasks[i];
        }

        return mask == RTFACT__GENERAL_BIT_MASK_BASE_C_FALSE;
    }

    static RTFACT_BIT_MASK_INLINE const t_This replicate(
        const uint aBit)
    {
        RTFACT__BIT_MASK_ASSERT(aBit >> 1 == 0);

        const Data mask = aBit ? RTFACT__GENERAL_BIT_MASK_BASE_C_TRUE :
                                 RTFACT__GENERAL_BIT_MASK_BASE_C_FALSE;

        t_This ret;

        for(uint i = 0; i < DATA_CONTAINER_COUNT; ++i)
        {
            ret.mMasks[i] = mask;
        }

        return ret;
    }

    RTFACT__GENERAL_BIT_MASK_BASE_BOOL_OP       (|)
    RTFACT__GENERAL_BIT_MASK_BASE_BOOL_ASSIGN_OP(|)
    RTFACT__GENERAL_BIT_MASK_BASE_BOOL_OP       (&)
    RTFACT__GENERAL_BIT_MASK_BASE_BOOL_ASSIGN_OP(&)
    RTFACT__GENERAL_BIT_MASK_BASE_BOOL_OP       (^)
    RTFACT__GENERAL_BIT_MASK_BASE_BOOL_ASSIGN_OP(^)
};

#pragma endregion

#pragma region Undefine helper macros

#undef RTFACT__GENERAL_BIT_MASK_BASE_C_TRUE
#undef RTFACT__GENERAL_BIT_MASK_BASE_C_FALSE

#undef RTFACT__GENERAL_BIT_MASK_BASE_BOOL_OP
#undef RTFACT__GENERAL_BIT_MASK_BASE_BOOL_ASSIGN_OP

#pragma endregion

#pragma endregion

} // namespace Detail

#pragma region BitMask class definition

#define RTFACT__BIT_MASK_CALC_DATA_SIZE(aSize)                                 \
    ((aSize <= 32) ? 32 : ((aSize <= 64) ? 64 : 128 /* the 128 is dummy */))

template<
    uint taSize>
class BitMask :
    public Detail::BitMaskBase<taSize,
                                 RTFACT__BIT_MASK_CALC_DATA_SIZE(taSize)>
{
    typedef Detail::BitMaskBase<taSize,
                                RTFACT__BIT_MASK_CALC_DATA_SIZE(taSize)> t_Base;

public:

    typedef typename t_Base::Data Data;

    RTFACT_BIT_MASK_INLINE BitMask() :
        t_Base()
    {}

    RTFACT_BIT_MASK_INLINE BitMask(
        const t_Base& aMask
    ) :
        t_Base(aMask)
    {}

    RTFACT_BIT_MASK_INLINE explicit BitMask(
        const Data aMask
    ) :
        t_Base(aMask)
    {}
};

#pragma region Undefine helper macros

#undef RTFACT_BIT_MASK_INLINE
#undef RTFACT__BIT_MASK_CALC_DATA_SIZE

#undef RTFACT__BIT_MASK_ASSERT
#undef RTFACT__BIT_MASK_STATIC_ASSERT

#pragma endregion

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__BITMASK_HPP
