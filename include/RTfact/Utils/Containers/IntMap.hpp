/* 
 *  IntMap.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein, Petr Kmoch
 */
 
 #ifndef RTFACT__INTMAP_HPP
#define RTFACT__INTMAP_HPP

#include <RTfact/Config/Common.hpp>

#include <list>
#include <utility>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/IO/SerializationUtils.hpp>

#define RTFACT__MAP_INLINE RTFACT_CONTAINER_INLINE
#define RTFACT__MAP_ASSERT RTFACT_ASSERT

#ifdef RTFACT_ASSERTIONS_ENABLED
#   define RTFACT__MAP_ASSERTIONS_ENABLED
#endif

RTFACT_NAMESPACE_BEGIN

/*

    NOTES:

    The goal of this class is to provide safe and more importantly fast
    management of objects, which can be created/erased often. The requirements
    are:

        1) Each object has a unique 32-bit (integer) key (SIMD-friendly);
        2) An object's key doesn't change over time;
        3) Quiering the object using the key is as fast as possible;
        4) Adding and deleting objects is fast

    We will internally use a vector and whenever an element is deleted, we will
    mark its space as free for later use for another object. This way, all
    requirements are met, with the note that memory reallocation and copying
    will be done from time to time when when adding new items and the capacity
    of the buffer is exceeded.

*/

#define RTFACT__DEFAULT_MIN_USED_KEY 0
#define RTFACT__DEFAULT_MAX_USED_KEY 0

template<
    class tElement>
class IntMap
{
    typedef IntMap<tElement>                    t_ThisMap;

public:

    typedef size_t                              key_type;
    typedef tElement                            value_type;
    typedef value_type*                         pointer;
    typedef Vector<key_type>                    key_vector;
    //typedef const boost::shared_ptr<key_vector> key_vector_ptr;
    typedef key_vector*                   key_vector_ptr;
    typedef size_t                              size_type;

protected:

    enum
    {
        ELEM_HAS_TRIVIAL_CONSTR = boost::has_trivial_constructor<value_type>::value
    };

    struct key_interval
    {
        key_type keyBegin, keyEnd;

        key_interval(
            const key_type aKeyBegin,
            const key_type aKeyEnd
        ) :
            keyBegin(aKeyBegin),
            keyEnd(aKeyEnd)
        {}

        size_type size() const
        {
            return keyEnd - keyBegin;
        }
    };

    typedef Vector<value_type>              t_Vector;
    typedef std::list<key_interval>         t_List;
    typedef typename t_List::iterator       t_ListIterator;
    typedef typename t_List::const_iterator t_ListConstIterator;

    t_Vector  mElements;
    t_List    mFreeKeys;
    key_type  mMinUsedKey, mMaxUsedKey;
    size_type mSize;

    bool isKeyFree(
        const key_type aKey) const
    {
        for(t_ListConstIterator it = mFreeKeys.begin();
            it != mFreeKeys.end(); ++it)
        {
            if(aKey >= it->keyBegin && aKey < it->keyEnd)
            {
                return true;
            }
        }

        return false;
    }

    #define RTFACT__ASSERT_ELEMENT_EXISTS(aKey)                                \
        RTFACT__MAP_ASSERT(aKey >= mMinUsedKey && aKey < mMaxUsedKey &&        \
                          !isKeyFree(aKey))

	#define RTFACT__ASSERT_ELEMENT_NOT_EXISTS(aKey)                                \
    RTFACT__MAP_ASSERT(aKey < mMinUsedKey || aKey >= mMaxUsedKey ||       \
                      isKeyFree(aKey))

    friend class key_iterator;

public:

    class key_iterator
    {
    protected:

        friend class IntMap<tElement>;

        const t_ThisMap&    mMap;
        t_ListConstIterator mFreeKeyIt;
        key_type            mKey;

        key_iterator(const key_type aKey, const t_ThisMap& aMap)
            : mMap(aMap), mFreeKeyIt(aMap.mFreeKeys.begin()), mKey(aKey)
        {

            if(mFreeKeyIt != mMap.mFreeKeys.end() &&
                mKey > mFreeKeyIt->keyBegin)
            {
                ++mFreeKeyIt;
            }
        }

    public:

        RTFACT__MAP_INLINE key_type operator*() const
        {
            RTFACT__MAP_ASSERT(mKey < mMap.mMaxUsedKey);

            return mKey;
        }

        RTFACT__MAP_INLINE key_iterator& operator++()
        {
            RTFACT__MAP_ASSERT(mKey < mMap.mMaxUsedKey);

            if(mFreeKeyIt == mMap.mFreeKeys.end() ||
               mFreeKeyIt->keyBegin == (mMap.mMaxUsedKey) ||
               mKey < mFreeKeyIt->keyBegin - 1)
            {
                ++mKey;
            }
            else
            {
                mKey = mFreeKeyIt->keyEnd;

                ++mFreeKeyIt;
            }

            return *this;
        }

        RTFACT__MAP_INLINE key_iterator operator++(int)
        {
            RTFACT__MAP_ASSERT(mKey < mMap.mMaxUsedKey);

            key_iterator prevThis = *this;
            
            if(mFreeKeyIt == mMap.mFreeKeys.end() ||
               mFreeKeyIt->keyBegin == (mMap.mMaxUsedKey) ||
               mKey < mFreeKeyIt->keyBegin - 1)
            {
                ++mKey;
            }
            else
            {
                mKey = mFreeKeyIt->keyEnd;

                ++mFreeKeyIt;
            }

            return prevThis;
        }

        RTFACT__MAP_INLINE key_iterator& operator+=(size_t aCount)
        {
            RTFACT__MAP_ASSERT(mKey < mMap.mMaxUsedKey);

            while(mFreeKeyIt != mMap.mFreeKeys.end() &&
                mFreeKeyIt->keyBegin - mKey <= aCount)
            {
                aCount -= mFreeKeyIt->keyBegin - mKey;
                if(mFreeKeyIt->keyBegin == (mMap.mMaxUsedKey))
                    mKey = mKey = mFreeKeyIt->keyBegin;
                else
                    mKey = mKey = mFreeKeyIt->keyEnd;
                ++mFreeKeyIt;
            }

            mKey+= aCount;

            RTFACT__MAP_ASSERT(mKey <= mMap.mMaxUsedKey);

            return *this;
        }

        RTFACT__MAP_INLINE key_iterator operator+(size_t aCount)
        {
            key_iterator newIt = *this;

            newIt += aCount;

            return newIt;
        }

        #define RTFACT__KEY_ITERATOR_BINARY_OP(aOperator)                      \
            RTFACT__MAP_INLINE bool operator aOperator(                        \
                const key_iterator& aOther) const                              \
            {                                                                  \
                return (mKey aOperator aOther.mKey);                           \
            }

        RTFACT__KEY_ITERATOR_BINARY_OP(==)
        RTFACT__KEY_ITERATOR_BINARY_OP(!=)

        #undef RTFACT__KEY_ITERATOR_BINARY_OP
    };

protected:

    template<class tChild>
    class iterator_base
    {
    protected:

        key_iterator mKeyIterator;

        iterator_base(
            const key_iterator& aKeyIterator
        ) :
            mKeyIterator(aKeyIterator)
        {}

    public:

        RTFACT__MAP_INLINE tChild& operator++()
        {
            ++mKeyIterator;

            return *static_cast<tChild*>(this);
        }

        RTFACT__MAP_INLINE tChild operator++(int)
        {
            tChild prevThis = *static_cast<tChild*>(this);

            ++mKeyIterator;

            return prevThis;
        }


        RTFACT__MAP_INLINE tChild& operator+=(size_t aCount)
        {
            mKeyIterator += aCount;

            return *static_cast<tChild*>(this);
        }

        RTFACT__MAP_INLINE tChild operator+(size_t aCount)
        {
            tChild newIt = *static_cast<tChild*>(this);

            newIt += aCount;

            return newIt;
        }

        #define RTFACT_UTILS__ITERATOR_BINARY_OP(aOperator)                    \
            RTFACT__MAP_INLINE bool operator aOperator(                        \
                const iterator_base& aOther) const                             \
            {                                                                  \
                return (mKeyIterator aOperator aOther.mKeyIterator);           \
            }

        RTFACT_UTILS__ITERATOR_BINARY_OP(==)
        RTFACT_UTILS__ITERATOR_BINARY_OP(!=)
    };

public:

    class const_iterator;

    class iterator :
        public iterator_base<iterator>
    {
    protected:

        using iterator_base<iterator>::mKeyIterator;

        friend class IntMap<tElement>;
        friend class const_iterator;

        t_ThisMap& mMap;

        iterator(
            const key_iterator& aKeyIterator,
            t_ThisMap& aMap
        ) :
            iterator_base<iterator>(aKeyIterator),
            mMap(aMap)
        {}

    public:

        const value_type& operator*() const
        {
            return mMap.mElements[*mKeyIterator];
        }

        value_type& operator*()
        {
            return mMap.mElements[*mKeyIterator];
        }

        const value_type* operator->() const
        {
            return &mMap.mElements[*mKeyIterator];
        }

        value_type* operator->()
        {
            return &mMap.mElements[*mKeyIterator];
        }
    };

    class const_iterator :
        public iterator_base<const_iterator>
    {
    protected:

        using iterator_base<const_iterator>::mKeyIterator;

        friend class IntMap<tElement>;

        const t_ThisMap& mMap;

        const_iterator(
            key_iterator& aKeyIterator,
            const t_ThisMap& aMap
        ) :
            iterator_base<const_iterator>(aKeyIterator),
            mMap(aMap)
        {}

    public:

        const_iterator(
            const iterator& aIt
        ) :
            iterator_base<const_iterator>(aIt.mKeyIterator),
            mMap(aIt.mMap)
        {}

        const value_type& operator*() const
        {
            return mMap.mElements[*mKeyIterator];
        }

        const value_type* operator->() const
        {
            return &mMap.mElements[*mKeyIterator];
        }
    };

    IntMap() :
        mMinUsedKey(RTFACT__DEFAULT_MIN_USED_KEY),
        mMaxUsedKey(RTFACT__DEFAULT_MAX_USED_KEY),
        mSize(0)
    {}

    IntMap(
        const t_ThisMap& aOther)
    {
        RTFACT_PROGRAM_ERROR("Not implemented");
    }

    ~IntMap()
    {
        clear();
    }

    void printFreeKeys() const
    {
        for(t_ListConstIterator it = mFreeKeys.begin(); it != mFreeKeys.end(); ++it)
        {
            std::cout << "[" << it->keyBegin << ", " << it->keyEnd << "] ";
        }

        std::cout << std::endl;
    }

    key_type create()
    {
        key_type key;

        if(mFreeKeys.size() > 0)
        {
            t_ListIterator firstFreeKeyIt = mFreeKeys.begin();

            key = firstFreeKeyIt->keyBegin;

            if(!ELEM_HAS_TRIVIAL_CONSTR)
            {
                new (&mElements[key]) value_type;
            }

            if(firstFreeKeyIt->size() == 1)
            {
                mFreeKeys.erase(firstFreeKeyIt);
            }
            else
            {
                ++firstFreeKeyIt->keyBegin;
            }
        }
        else
        {
            key = mElements.size();

            mElements.resize(key + 1);
        }

        mMinUsedKey = RTFACT_MIN(mMinUsedKey, key);
        mMaxUsedKey = RTFACT_MAX(mMaxUsedKey, key + 1);

        ++mSize;

        return key;
    }

    key_vector_ptr create(
        const size_type aCount)
    {
        RTFACT__MAP_ASSERT(aCount > 0);

        key_vector& keys = *(new key_vector(aCount));

        if(mFreeKeys.size() > 0)
        {
            t_ListIterator freeKeyIt = mFreeKeys.begin();

            size_type resIndex = 0;

            while(freeKeyIt != mFreeKeys.end())
            {
                int i = freeKeyIt->keyBegin;

                while(i < freeKeyIt->keyEnd && resIndex < aCount)
                {
                    keys[resIndex] = i++;

                    if(!ELEM_HAS_TRIVIAL_CONSTR)
                    {
                        new (&mElements[resIndex]) value_type;
                    }

                    ++resIndex;
                }

                if(resIndex == aCount)
                {
                    if(i < freeKeyIt->keyEnd)
                    {
                        freeKeyIt->keyBegin = i;
                    }
                    else
                    {
                        freeKeyIt = mFreeKeys.erase(freeKeyIt);
                    }

                    break;
                }

                freeKeyIt = mFreeKeys.erase(freeKeyIt);
            }

            if(freeKeyIt == mFreeKeys.end() && resIndex < aCount)
            {
                const size_type oldSize = mElements.size();
                
                mElements.resize(oldSize + aCount);

                int i = oldSize;

                while(resIndex < aCount)
                {
                    keys[resIndex++] = i++;
                }
            }

            mMinUsedKey = RTFACT_MIN(mMinUsedKey, keys[0]);
            mMaxUsedKey = RTFACT_MAX(mMaxUsedKey, keys[aCount - 1] + 1);
        }
        else
        {
            key_type firstKey = mElements.size();

            const size_type newSize = firstKey + aCount;

            mElements.resize(newSize);

            for(size_type i = 0; i < aCount; ++i)
            {
                keys[i] = firstKey + i;
            }

            mMinUsedKey = 0;
            mMaxUsedKey = newSize;
        }

        mSize += aCount;

        return key_vector_ptr(&keys);
    }

    key_type insert(
        const value_type& aValue)
    {
        key_type key;

        if(mFreeKeys.size() > 0)
        {
            t_ListIterator firstFreeKeyIt = mFreeKeys.begin();

            key = firstFreeKeyIt->keyBegin;

            mElements[key] = aValue;

            if(firstFreeKeyIt->size() == 1)
            {
                mFreeKeys.erase(firstFreeKeyIt);
            }
            else
            {
                ++firstFreeKeyIt->keyBegin;
            }
        }
        else
        {
            key = mElements.size();

            mElements.push_back(aValue);
        }

        mMinUsedKey = RTFACT_MIN(mMinUsedKey, key);
        mMaxUsedKey = RTFACT_MAX(mMaxUsedKey, key + 1);

        ++mSize;

        return key;
    }

    template<
        class tIterator>
    key_vector_ptr insert(
        tIterator aBegin,
        tIterator aEnd)
    {
        key_vector_ptr pKeys = create(aEnd - aBegin);

        for(key_vector::iterator it = pKeys->begin(); it != pKeys->end(); ++it)
        {
            mElements[*it] = *aBegin;

            ++aBegin;
        }

        return pKeys;
    }

    RTFACT__MAP_INLINE const value_type& operator[](
        const key_type aKey) const
    {
        RTFACT__ASSERT_ELEMENT_EXISTS(aKey);

        return mElements[aKey];
    }

    RTFACT__MAP_INLINE value_type& operator[](
        const key_type aKey)
    {
        RTFACT__ASSERT_ELEMENT_EXISTS(aKey);

        return mElements[aKey];
    }


    // Note: DO NOT USE this function while iterating IntMap
    // with key_iterator. Otherwise, assertions will fail
    // and unexpected behaviour occur including infinite loops
    void erase(
        const key_type aKey)
    {
        RTFACT__ASSERT_ELEMENT_EXISTS(aKey);

        mElements.get_allocator().destroy(&mElements[aKey]);

        t_ListIterator freeKeyIt = mFreeKeys.begin();

        if(mMinUsedKey + 1 == mMaxUsedKey ) // all elements have been erased
        {
            mMinUsedKey = RTFACT__DEFAULT_MIN_USED_KEY;
            mMaxUsedKey = RTFACT__DEFAULT_MAX_USED_KEY;

            mFreeKeys.clear();
            mFreeKeys.push_back(key_interval(0, key_type(mElements.size())));
        }
        else if(mFreeKeys.size() == 0)
        {
            mFreeKeys.push_back(key_interval(aKey, aKey + 1));

            if(aKey == mMinUsedKey)
            {
                ++mMinUsedKey;
            }
            else if(aKey + 1 == mMaxUsedKey)
            {
                --mMaxUsedKey;
            }
        }
        else
        {
            if(aKey < freeKeyIt->keyBegin)
            {
                if(aKey == freeKeyIt->keyBegin - 1)
                {
                    --freeKeyIt->keyBegin;
                }
                else
                {
                    mFreeKeys.push_front(key_interval(aKey, aKey + 1));
                }
            }
            else
            {
                t_ListIterator nextFreeKeyIt = freeKeyIt;
                ++nextFreeKeyIt;

                while(nextFreeKeyIt != mFreeKeys.end() &&
                      aKey > nextFreeKeyIt->keyBegin)
                {
                    ++nextFreeKeyIt;
                }

                freeKeyIt = nextFreeKeyIt;
                --freeKeyIt;

                if(nextFreeKeyIt == mFreeKeys.end())
                {
                    if(aKey == freeKeyIt->keyEnd)
                    {
                        ++freeKeyIt->keyEnd;
                    }
                    else
                    {
                        mFreeKeys.insert(nextFreeKeyIt,
                                          key_interval(aKey, aKey + 1));
                    }
                }
                else if(aKey == freeKeyIt->keyEnd)
                {
                    if(aKey == nextFreeKeyIt->keyBegin - 1)
                    {
                        freeKeyIt->keyEnd = nextFreeKeyIt->keyEnd;

                        mFreeKeys.erase(nextFreeKeyIt);
                    }
                    else
                    {
                        ++freeKeyIt->keyEnd;
                    }
                }
                else if(aKey == nextFreeKeyIt->keyBegin - 1)
                {
                    --nextFreeKeyIt->keyBegin;
                }
                else
                {
                    mFreeKeys.insert(nextFreeKeyIt,
                                      key_interval(aKey, aKey + 1));
                }
            }

            if(aKey == mMinUsedKey)
            {
                mMinUsedKey = (mFreeKeys.front().keyBegin > 0) ?
                    0 : mFreeKeys.front().keyEnd;
            }
            else if(aKey + 1 == mMaxUsedKey)
            {
                mMaxUsedKey =
                   (static_cast<typename t_Vector::size_type>(mFreeKeys.back().keyEnd) <
                    mElements.size()) ?
                        mElements.size() :
                        mFreeKeys.back().keyBegin;
            }
        }



        --mSize;

        RTFACT__ASSERT_ELEMENT_NOT_EXISTS(aKey);

    }

    // Optimize this!
    template<
        class tIterator>
    void erase(
        tIterator aBegin,
        tIterator aEnd)
    {
        while(aBegin != aEnd)
        {
            erase(*aBegin);

            ++aBegin;
        }
    }

    size_type size() const
    {
        return mSize;
    }

    void clear()
    {
        mElements.clear();
        mFreeKeys.clear();

        mMinUsedKey = RTFACT__DEFAULT_MIN_USED_KEY;
        mMaxUsedKey = RTFACT__DEFAULT_MAX_USED_KEY;

        mSize = 0;
    }

    RTFACT__MAP_INLINE key_iterator keysBegin() const
    {
        return key_iterator(mMinUsedKey, *this);
    }

    RTFACT__MAP_INLINE key_iterator keysEnd() const
    {
        return key_iterator(mMaxUsedKey, *this);
    }

    RTFACT__MAP_INLINE const_iterator begin() const
    {
        return const_iterator(keysBegin(), *this);
    }

    RTFACT__MAP_INLINE iterator begin()
    {
        return iterator(keysBegin(), *this);
    }

    RTFACT__MAP_INLINE const_iterator end() const
    {
        return const_iterator(keysEnd(), *this);
    }

    RTFACT__MAP_INLINE iterator end()
    {
        return iterator(keysEnd(), *this);
    }

    RTFACT__MAP_INLINE const value_type& getElementBase() const
    {
        return mElements[0];
    }

    RTFACT__MAP_INLINE key_type getMaxKey() const
    {
        return mMaxUsedKey - 1;
    }


    void save(
        IO::Serializer& aSerializer)
    {
        typename t_List::size_type freeKeyCount = mFreeKeys.size();

        aSerializer << mSize
                    << mMinUsedKey
                    << mMaxUsedKey
                    << mElements
                    << freeKeyCount;

        for(typename t_List::iterator it = mFreeKeys.begin();
            it != mFreeKeys.end(); ++it)
        {
            aSerializer << it->keyBegin << it->keyEnd;
        }
    }

    void load(
        IO::Serializer& aSerializer)
    {
        typename t_List::size_type freeKeyCount;
        int keyBegin, keyEnd;

        aSerializer >> mSize
                    >> mMinUsedKey
                    >> mMaxUsedKey
                    >> mElements
                    >> freeKeyCount;

        for(typename t_List::size_type i = 0; i < freeKeyCount; ++i)
        {
            aSerializer >> keyBegin >> keyEnd;

            mFreeKeys.push_back(key_interval(keyBegin, keyEnd));
        }
    }
};

RTFACT_NAMESPACE_END

RTFACT_SERIALIZATION_REGISTER_LOAD_SAVE_METHODS_TEMPLATE_CLASS(RTfact::IntMap, (class))

#pragma region Undefine helper macros

#undef RTFACT__ASSERT_ELEMENT_EXISTS

#undef RTFACT__DEFAULT_MIN_USED_KEY
#undef RTFACT__DEFAULT_MAX_USED_KEY

#undef RTFACT__MAP_ASSERT
#undef RTFACT__MAP_INLINE

#ifdef RTFACT_ASSERTIONS_ENABLED
#   undef RTFACT__MAP_ASSERTIONS_ENABLED
#endif

#pragma endregion // Undefine helper macros

#endif // RTFACT__INTMAP_HPP
