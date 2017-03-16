/* 
 *  ActiveMask.hpp
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
 *  Created on: 2008-11-09 23:28:58 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__PACKETACTIVEMASK_HPP
#define RTFACT__PACKETACTIVEMASK_HPP

RTFACT_NAMESPACE_BEGIN

template<
    uint taSize>
class ActiveMask :
    public Mask<taSize>
{
    typedef Mask<taSize> t_Base;

public:

    typedef typename t_Base::Data      Data;
    typedef typename t_Base::Container Container;
    typedef typename t_Base::Scalar    Scalar;

    using t_Base::CONTAINER_COUNT;
    using t_Base::SIZE;
    using t_Base::SIZE_SQRT;
    using t_Base::containers;

    uint activeContainers[CONTAINER_COUNT];
    uint activeContainerEndIndex;

    RTFACT_PACKET_INLINE ActiveMask() :
        activeContainerEndIndex(0)
    {}

    RTFACT_PACKET_INLINE ActiveMask(
        const t_Base& aOther
    ) :
        t_Base(aOther)
    {}

    RTFACT_PACKET_INLINE void updateActiveContainers()
    {
        activeContainerEndIndex = 0;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            if(containers[i].getIntMask())
            {
                activeContainers[activeContainerEndIndex++] = i;
            }
        }
    }

    RTFACT_PACKET_INLINE uint& getActiveContainerAt(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex < CONTAINER_COUNT);

        return activeContainers[aIndex];
    }

    RTFACT_PACKET_INLINE const uint& getActiveContainerAt(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex < CONTAINER_COUNT);

        return activeContainers[aIndex];
    }

    RTFACT_PACKET_INLINE uint getActiveContainerAtPrivate(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex <= CONTAINER_COUNT);

        return activeContainers[aIndex];
    }

    RTFACT_PACKET_INLINE uint getActiveContainerEndIndex() const
    {
        return activeContainerEndIndex;
    }

    RTFACT_PACKET_INLINE void setFalseAndUpdateActiveContainers()
    {
        Mask<SIZE>::setFalse();

        activeContainerEndIndex = 0;
    }

    RTFACT_PACKET_INLINE void setTrueAndUpdateActiveContainers()
    {
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, SIZE)
        {
            containers[i].data = Data::C_T_T_T_T;
            activeContainers[i] = i;
        }

        activeContainerEndIndex = CONTAINER_COUNT;
    }

    RTFACT_PACKET_INLINE void clearActiveContainerEndIndex()
    {
        activeContainerEndIndex = 0;
    }

    RTFACT_PACKET_INLINE void updateActiveContainerEndIndex(
        const uint aContainerIndex)
    {
        RTFACT_PACKET_ASSERT(aContainerIndex < CONTAINER_COUNT);

        if(containers[aContainerIndex].getIntMask())
        {
            activeContainers[activeContainerEndIndex++] = aContainerIndex;
        }
    }

    RTFACT_PACKET_INLINE bool isFalse() const
    {
        return activeContainerEndIndex == 0;
    }
};

template<>
class ActiveMask<4> :
    public Mask<4>
{
    typedef Mask<4> t_Base;

public:

    typedef t_Base::Data      Data;
    typedef t_Base::Container Container;
    typedef t_Base::Scalar    Scalar;

    using t_Base::CONTAINER_COUNT;
    using t_Base::SIZE;
    using t_Base::SIZE_SQRT;
    using t_Base::data;

    uint activeContainers[1];
    uint activeContainerEndIndex;

    RTFACT_PACKET_INLINE ActiveMask() :
        activeContainerEndIndex(0)
    {}

    RTFACT_PACKET_INLINE ActiveMask(
        const t_Base& aOther
    ) :
        t_Base(aOther)
    {}

    RTFACT_PACKET_INLINE void updateActiveContainers()
    {
        activeContainers[0] = 0;
        activeContainerEndIndex = (data.getI32Mask() != 0);
    }

    RTFACT_PACKET_INLINE uint& getActiveContainerAt(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex < CONTAINER_COUNT);

        return activeContainers[0];
    }

    RTFACT_PACKET_INLINE const uint& getActiveContainerAt(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex < CONTAINER_COUNT);

        return activeContainers[0];
    }

    RTFACT_PACKET_INLINE uint getActiveContainerAtPrivate(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex <= CONTAINER_COUNT);

        return 0;
    }

    RTFACT_PACKET_INLINE uint getActiveContainerEndIndex() const
    {
        return activeContainerEndIndex;
    }

    RTFACT_PACKET_INLINE void setFalseAndUpdateActiveContainers()
    {
        t_Base::setFalse();
        activeContainerEndIndex = 0;
    }

    RTFACT_PACKET_INLINE void setTrueAndUpdateActiveContainers()
    {
        t_Base::setTrue();
        activeContainers[0] = 0;
        activeContainerEndIndex = 1;
    }

    RTFACT_PACKET_INLINE void clearActiveContainerEndIndex()
    {
        activeContainerEndIndex = 0;
    }

    RTFACT_PACKET_INLINE void updateActiveContainerEndIndex(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        if(getIntMask())
        {
            activeContainers[activeContainerEndIndex++] = 0;
        }
    }

    RTFACT_PACKET_INLINE bool isFalse() const
    {
        return activeContainerEndIndex == 0;
    }
};

template<>
class ActiveMask<1> :
    public Mask<1>
{
    typedef Mask<1> t_Base;

public:

    typedef t_Base::Data      Data;
    typedef t_Base::Container Container;
    typedef t_Base::Scalar    Scalar;

    using t_Base::CONTAINER_COUNT;
    using t_Base::SIZE;
    using t_Base::SIZE_SQRT;
    using t_Base::data;

    uint activeContainers[1];
    uint activeContainerEndIndex;

    RTFACT_PACKET_INLINE ActiveMask() :
        activeContainerEndIndex(0)
    {}

    RTFACT_PACKET_INLINE ActiveMask(
        const t_Base& aOther
    ) :
        t_Base(aOther)
    {}

    RTFACT_PACKET_INLINE ActiveMask<1>& operator=(
        const Scalar aValue)
    {
        data = aValue;

        return *this;
    }

    RTFACT_PACKET_INLINE ActiveMask<1>& operator=(
        const t_Base& aOther)
    {
        data = aOther.data;

        return *this;
    }

    RTFACT_PACKET_INLINE void updateActiveContainers()
    {
        activeContainers[0] = 0;
        activeContainerEndIndex = (data != 0);
    }

    RTFACT_PACKET_INLINE uint& getActiveContainerAt(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        return activeContainers[0];
    }

    RTFACT_PACKET_INLINE const uint& getActiveContainerAt(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        return activeContainers[0];
    }

    RTFACT_PACKET_INLINE uint getActiveContainerAtPrivate(
        const uint aIndex) const
    {
        RTFACT_PACKET_ASSERT(aIndex <= 1);

        return 0;
    }

    RTFACT_PACKET_INLINE uint getActiveContainerEndIndex() const
    {
        return activeContainerEndIndex;
    }

    RTFACT_PACKET_INLINE void setFalseAndUpdateActiveContainers()
    {
        t_Base::setFalse();
        activeContainerEndIndex = 0;
    }

    RTFACT_PACKET_INLINE void setTrueAndUpdateActiveContainers()
    {
        t_Base::setTrue();
        activeContainers[0] = 0;
        activeContainerEndIndex = 1;
    }

    RTFACT_PACKET_INLINE void clearActiveContainerEndIndex()
    {
        activeContainerEndIndex = 0;
    }

    RTFACT_PACKET_INLINE void updateActiveContainerEndIndex(
        const uint aIndex)
    {
        RTFACT_PACKET_ASSERT(aIndex == 0);

        if(data)
        {
            activeContainers[activeContainerEndIndex++] = 0;
        }
    }
};

//template<
//    uint taSize>
//RTFACT_PACKET_INLINE Mask<taSize>::Mask(
//    const ActiveMask<taSize>& aMask)
//{
//    *this = *((Mask<taSize>*) &aMask);
//}

RTFACT_NAMESPACE_END

#endif // RTFACT__PACKETACTIVEMASK_HPP
