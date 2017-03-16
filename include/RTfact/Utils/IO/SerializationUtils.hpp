/* 
 *  SerializationUtils.hpp
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
 
 #ifndef RTFACT__SERIALIZATIONUTILS_HPP
#define RTFACT__SERIALIZATIONUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <iostream>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/size.hpp>

#include <boost/type_traits/remove_const.hpp>

RTFACT_NAMESPACE_BEGIN

namespace IO {

#pragma region Serialization traits

template<
    class tType>
struct HasSerializeMethod
{
    RTFACT_STATIC_CONSTANT(value, 0);
};

template<
    class tType>
struct HasLoadSaveSerializationMethods
{
    RTFACT_STATIC_CONSTANT(value, 0);
};

template<
    class tType>
struct HasVersioning
{
    RTFACT_STATIC_CONSTANT(value, 0);
};

template<
    class tType>
struct Version
{
    RTFACT_STATIC_CONSTANT(value, 0);
};

#define RTFACT_SERIALIZATION_DEFINE_TRAIT(aTraitName, aClassName, aValue)      \
    RTFACT_NAMESPACE_BEGIN                                                     \
    namespace IO {                                                             \
    template<>                                                                 \
    struct aTraitName<aClassName>                                              \
    {                                                                          \
        RTFACT_STATIC_CONSTANT(value, aValue);                                 \
    };                                                                         \
    } /* namespace IO */                                                       \
    RTFACT_NAMESPACE_END                                                       \

#define RTFACT__SERIALIZATION_TRAITS_HELPER(_, aName, aIndex, aType)           \
    (aType BOOST_PP_CAT(aName, aIndex))

#define RTFACT_SERIALIZATION_DEFINE_TEMPLATE_TRAIT(                            \
        aTraitName, aClassName, aParams, aValue)                               \
    RTFACT_NAMESPACE_BEGIN                                                     \
    namespace IO {                                                             \
    template<BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FOR_EACH_I(                        \
        RTFACT__SERIALIZATION_TRAITS_HELPER, T, aParams))>                     \
    struct aTraitName<aClassName<                                              \
        BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(aParams), T)> >                 \
    {                                                                          \
        RTFACT_STATIC_CONSTANT(value, aValue);                                 \
    };                                                                         \
    } /* namespace IO */                                                       \
    RTFACT_NAMESPACE_END                                                       \

#define RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_CLASS(aClassName)       \
    RTFACT_SERIALIZATION_DEFINE_TRAIT(HasSerializeMethod, aClassName, 1)

#define RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_TEMPLATE_CLASS(         \
        aClassName, aParams)                                                   \
    RTFACT_SERIALIZATION_DEFINE_TEMPLATE_TRAIT(                                \
        HasSerializeMethod, aClassName, aParams, 1)

#define RTFACT_SERIALIZATION_REGISTER_LOAD_SAVE_METHODS_CLASS(aClassName)      \
    RTFACT_SERIALIZATION_DEFINE_TRAIT(                                         \
        HasLoadSaveSerializationMethods, aClassName, 1)

#define RTFACT_SERIALIZATION_REGISTER_LOAD_SAVE_METHODS_TEMPLATE_CLASS(        \
        aClassName, aParams)                                                   \
    RTFACT_SERIALIZATION_DEFINE_TEMPLATE_TRAIT(                                \
        HasLoadSaveSerializationMethods, aClassName, aParams, 1)

#define RTFACT_SERIALIZATION_REGISTER_CLASS_VERSION(aClassName, aVersionNumber)\
    RTFACT_SERIALIZATION_DEFINE_TRAIT(HasVersioning, aClassName, 1)            \
    RTFACT_SERIALIZATION_DEFINE_TRAIT(Version, aClassName, aVersionNumber, 1)

#define RTFACT_SERIALIZATION_REGISTER_TEMPLATE_CLASS_VERSION(                  \
        aClassName, aParams, aVersionNumber)                                   \
    RTFACT_SERIALIZATION_DEFINE_TEMPLATE_TRAIT(                                \
        HasVersioning, aClassName, aParams, 1)                                 \
    RTFACT_SERIALIZATION_DEFINE_TEMPLATE_TRAIT(                                \
        Version, aClassName, aParams, aVersionNumber)

#pragma endregion // Serialization traits

#pragma region Serializer class definition

class Serializer
{
    enum Mode
    {
        MODE_SERIALIZATION,
        MODE_DESERIALIZATION
    };

    #define RTFACT__ASSERT_SERIALIZATION_MODE                                  \
        RTFACT_PROGRAM_ERROR_IF(                                               \
            mMode != MODE_SERIALIZATION,                                       \
            "Trying to deserialize in serialization mode")

    #define RTFACT__ASSERT_DESERIALIZATION_MODE                                \
        RTFACT_PROGRAM_ERROR_IF(                                               \
            mMode != MODE_DESERIALIZATION,                                     \
            "Trying to serialize in deserialization mode")

    std::ostream* mOstream;
    std::istream* mIstream;
    Mode          mMode;

    #ifdef RTFACT_ARCH_64_BIT

        bool m32BitCompatibility;
    
    #endif

    template<
        class tType>
    void writeObject(
        tType& aObject)
    {
        mOstream->write(
            reinterpret_cast<const std::ostream::char_type*>(&aObject),
            sizeof(tType));
    }

    template<
        class tType>
    void readObject(
        tType& aObject)
    {
        mIstream->read(
            reinterpret_cast<std::istream::char_type*>(
                const_cast<typename boost::remove_const<tType>::type*>(&aObject)),
            sizeof(tType));
    }

public:

    #ifdef RTFACT_ARCH_64_BIT
        
        Serializer(
            std::ostream& aOutputStream,
            const bool a32BitCompatibility = true
        ) :
            mOstream(&aOutputStream),
            mMode(MODE_SERIALIZATION),
            m32BitCompatibility(a32BitCompatibility)
        {}

        Serializer(
            std::istream& aInputStream,
            const bool a32BitCompatibility = true
        ) :
            mIstream(&aInputStream),
            mMode(MODE_DESERIALIZATION),
            m32BitCompatibility(a32BitCompatibility)
        {}

    #else
        
        Serializer(
            std::ostream& aOutputStream
        ) :
            mOstream(&aOutputStream),
            mMode(MODE_SERIALIZATION)
        {}

        Serializer(
            std::istream& aInputStream
        ) :
            mIstream(&aInputStream),
            mMode(MODE_DESERIALIZATION)
        {}

    #endif

    template<
        class tType>
    Serializer& operator <<(
        tType& aObject)
    {
        RTFACT__ASSERT_SERIALIZATION_MODE;

        return handle(aObject);
    }

    template<
        class tType>
    Serializer& operator >>(
        tType& aObject)
    {
        RTFACT__ASSERT_DESERIALIZATION_MODE;

        return handle(aObject);
    }

    template<
        class tType>
    Serializer& operator &(
        tType& aObject)
    {
        return handle(aObject);
    }

    template<
        class tType>
    Serializer& handle(
        tType& aObject);

    template<
        class tType>
    void handleSerializeMethodType(
        tType& aObject)
    {
        aObject.serialize(*this);
    }

    template<
        class tType>
    void handleSerializeVersioningMethodType(
        tType& aObject)
    {
        uint version;

        if(mMode == MODE_SERIALIZATION)
        {
            version = Version<tType>::value;

            writeObject<const uint>(version);
        }
        else
        {
            readObject<uint>(version);
        }

        aObject.serialize(*this, version);
    }

    template<
        class tType>
    void handleLoadSaveMethodsType(
        tType& aObject)
    {
        if(mMode == MODE_SERIALIZATION)
        {
            aObject.save(*this);
        }
        else
        {
            aObject.load(*this);
        }
    }

    template<
        class tType>
    void handleLoadSaveVersioningMethodsType(
        tType& aObject)
    {
        if(mMode == MODE_SERIALIZATION)
        {
            writeObject<const uint>(Version<tType>::value);

            aObject.save(*this, Version<tType>::value);
        }
        else
        {
            uint version;
            readObject<uint>(version);

            aObject.load(*this, version);
        }
    }

    template<
        class tType>
    void handleGenericType(
        tType& aObject)
    {
        if(mMode == MODE_SERIALIZATION)
        {
            writeObject(aObject);
        }
        else
        {
            readObject(aObject);
        }
    }
};

#ifdef RTFACT_ARCH_64_BIT

    template<>
    RTFACT_INLINE void Serializer::writeObject<size_t>(
        size_t& aObject)
    {
        if(m32BitCompatibility)
        {
            uint tempObject = static_cast<uint>(aObject);

            mOstream->write(
                reinterpret_cast<const std::ostream::char_type*>(&tempObject),
                sizeof(uint));
        }
        else
        {
            mOstream->write(
                reinterpret_cast<const std::ostream::char_type*>(&aObject),
                sizeof(size_t));
        }
    }

    template<>
    RTFACT_INLINE void Serializer::readObject<size_t>(
        size_t& oObject)
    {
        if(m32BitCompatibility)
        {
            uint tempObject;

            mIstream->read(
                reinterpret_cast<std::ostream::char_type*>(&tempObject),
                sizeof(uint));

            oObject = static_cast<size_t>(tempObject);
        }
        else
        {
            mIstream->read(
                reinterpret_cast<std::ostream::char_type*>(&oObject),
                sizeof(size_t));
        }
    }

#endif

#pragma endregion // Serializer class definition

#pragma region Serialization helpers

class Serializer;

namespace Detail {

template<
    class tType,
    bool taHasVersioning>
struct SerializerSerializeVersioningMethodHelper
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        aSerializer.handleSerializeVersioningMethodType(aObject);
    }
};

template<
    class tType>
struct SerializerSerializeVersioningMethodHelper<tType, false>
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        aSerializer.handleSerializeMethodType(aObject);
    }
};

template<
    class tType,
        bool taHasSerializeMethod>
struct SerializerSerializeMethodHelper
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        SerializerSerializeVersioningMethodHelper<
            tType, HasVersioning<tType>::value>
                ::handle(aSerializer, aObject);
    }
};

template<
    class tType>
struct SerializerSerializeMethodHelper<tType, false>
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        aSerializer.handleGenericType(aObject);
    }
};

template<
    class tType,
        bool taHasVersioning>
struct SerializerLoadSaveVersioningMethodHelper
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        aSerializer.handleLoadSaveVersioningMethodsType(aObject);
    }
};

template<
    class tType>
struct SerializerLoadSaveVersioningMethodHelper<tType, false>
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        aSerializer.handleLoadSaveMethodsType(aObject);
    }
};

template<
    class tType,
        bool taHasLoadSaveMethods>
struct SerializerLoadSaveMethodsHelper
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        SerializerLoadSaveVersioningMethodHelper<
            tType, HasVersioning<tType>::value>
                ::handle(aSerializer, aObject);
    }
};

template<
    class tType>
struct SerializerLoadSaveMethodsHelper<tType, false>
{
    static void handle(
        Serializer& aSerializer,
        tType& aObject)
    {
        SerializerSerializeMethodHelper<
            tType, HasSerializeMethod<tType>::value>
                ::handle(aSerializer, aObject);
    }
};

} // namespace Detail

#pragma endregion // Serialization helpers

template<
    class tType>
Serializer& Serializer::handle(
    tType& aObject)
{
    Detail::SerializerLoadSaveMethodsHelper<
        tType, HasLoadSaveSerializationMethods<tType>::value>
            ::handle(*this, aObject);

    return *this;
}

} // namespace IO

RTFACT_NAMESPACE_END

#undef RTFACT__ASSERT_SERIALIZATION_MODE
#undef RTFACT__ASSERT_DESERIALIZATION_MODE

#endif // RTFACT__SERIALIZATIONUTILS_HPP
