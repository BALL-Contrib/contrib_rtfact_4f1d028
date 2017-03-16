/*
 *  StringConverter.hpp
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
 *  Created on: 2010-12-12 00:36:06 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */

#ifndef RTFACT__STRINGCONVERTER_HPP
#define RTFACT__STRINGCONVERTER_HPP

#include <RTfact/Config/Common.hpp>

#include <string>

#include <boost/type_traits/is_fundamental.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_same.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Detail
{
    //-----------------------------------
    //Default string converter
    //-----------------------------------
    #pragma region DefaultStringConverter class

    template<
        class tType,
        bool taIsIntegralType>
    struct DefaultStringConverterHelper
    {
        static const tType fromString(
            const std::string& aData)
        {
            try
            {
                return boost::lexical_cast<tType>(aData);
            }
            catch(...)
            {
                RTFACT_USER_ERROR_F(
                    "Cannot convert \"%1%\" to the specified type", aData);
            }
        }

        static const std::string toString(
            const tType& aData)
        {
            try
            {
                return boost::lexical_cast<std::string>(aData);
            }
            catch(...)
            {
                RTFACT_USER_ERROR_F(
                    "Cannot convert the specified value to string", aData);
            }
        }
    };

    template<
        class tType>
    struct DefaultStringConverterHelper<tType, false>
    {
        static const tType fromString(
            const std::string& aData)
        {
            RTFACT_USER_ERROR_F(
                "Cannot convert \"%1%\" to the specified type", aData);
        }

        static const std::string toString(
            const tType& aData)
        {
            RTFACT_USER_ERROR_F(
                "Cannot convert the specified value to string", aData);
        }
    };

    template<
        class tType>
    struct DefaultStringConverter
    {
        static const tType fromString(
            const std::string& aData)
        {
            return DefaultStringConverterHelper<
                tType, boost::is_fundamental<tType>::value>::fromString(aData);
        }

        static const std::string toString(
            const tType& aData)
        {
            return DefaultStringConverterHelper<
                tType, boost::is_fundamental<tType>::value>::toString(aData);
        }
    };

    #pragma endregion // StringConverterHelper class

} // namespace Detail

//-----------------------------------
//String convertion traits class
//-----------------------------------
#pragma region String conversion traits

template<
    class tType>
struct StringConversionTraits
{
    typedef Detail::DefaultStringConverter<tType> Converter;
};

#define RTFACT_STRING_CONVERTION_TRAITS(aType, aConverter)                     \
    struct StringConversionTraits<aType >                                      \
    {                                                                          \
        typedef aConverter Converter;                                          \
    };                                                                         \

#pragma endregion // String conversion traits

//-----------------------------------
//Generic string converter class
//-----------------------------------
#pragma region String conversion utility classes

template<
    class tType>
struct StringConverter
{
    static const tType fromString(
        const std::string& aData)
    {
        return StringConversionTraits<tType>::Converter::fromString(aData);
    }

    static const std::string toString(
        const tType& aData)
    {
        return StringConversionTraits<tType>::Converter::toString(aData);
    }
};

#pragma endregion // String conversion utility classes

//-----------------------------------
//String converter specializations
//for basic types
//-----------------------------------
#pragma region String converters for basic types

template<>
struct StringConverter<std::string>
{
    static const std::string fromString(
        const std::string& aData)
    {
        return aData;
    }

    static const std::string toString(
        const std::string& aData)
    {
        return aData;
    }
};

template<>
struct StringConverter<bool>
{
    static bool fromString(
        const std::string& aData)
    {
        const bool isTrue =
            aData == "1"     || aData == "true" ||
            aData == "True"  || aData == "TRUE";

        const bool isFalse =
            aData == "0"     || aData == "false" ||
            aData == "False" || aData == "FALSE";

        if(isTrue || isFalse)
        {
            return isTrue;
        }
        else
        {
            RTFACT_USER_ERROR_F("Cannot convert \"%1%\" to bool", aData);
        }
        return false;
    }

    static const std::string toString(
        const bool aData)
    {
        return aData ? "TRUE" : "FALSE";
    }
};

#pragma endregion //String converters for basic types

//-----------------------------------
//Convenience string converting functions
//-----------------------------------
#pragma region String conversion functions

template<
    class tType>
RTFACT_INLINE const tType fromString(
    const std::string& aData)
{
    return StringConverter<tType>::fromString(aData);
}

template<
    class tType>
RTFACT_INLINE const std::string toString(
    const tType& aData)
{
    return StringConverter<tType>::toString(aData);
}

template<
    class tType,
    class tInputType>
RTFACT_INLINE const std::string toString(
    const tInputType& aData)
{
    return StringConverter<tType>::toString(aData);
}

#pragma endregion // String conversion functions

RTFACT_NAMESPACE_END

#endif // RTFACT__STRINGCONVERTER_HPP
