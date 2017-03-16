/*
 *  StringAlgorithms.hpp
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
 *  Created on: 2010-12-12 15:51:00 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */

#ifndef RTFACT__STRINGALGORITHMS_HPP
#define RTFACT__STRINGALGORITHMS_HPP

#include <RTfact/Config/Common.hpp>

#include <string>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/type_traits/is_same.hpp>

RTFACT_NAMESPACE_BEGIN

//-----------------------------------
//Misc string algorithms
//-----------------------------------
#pragma region Misc string algorithms

RTFACT_INLINE const std::string toLower(
    const std::string& aString)
{
    return boost::to_lower_copy(aString);
}

RTFACT_INLINE const std::string trim(
    const std::string& aString)
{
    return boost::algorithm::trim_copy(aString);
}

struct trimQuotes_QuoteTrimPredicate
{
    bool operator()(const char aChar)
    {
        return (aChar == '\'') || (aChar == '"');
    }
};

RTFACT_INLINE void trimQuotes(
    std::string& aString)
{
    static trimQuotes_QuoteTrimPredicate trimPredicate;

    boost::algorithm::trim_if(aString, trimPredicate);
}

RTFACT_INLINE void replaceAll(
    std::string& aWhere,
    const std::string& aWhat,
    const std::string& aWith)
{
    std::string::size_type i = 0;

    while(i < aWhere.size())
    {
        i = aWhere.find(aWhat, i);

        if(i == std::string::npos)
        {
            break;
        }

        aWhere.replace(i, aWhat.size(), aWith);
        i += aWith.size();
    }
}

#pragma endregion //Misc string algorithms

RTFACT_NAMESPACE_END

#endif //RTFACT__STRINGALGORITHMS_HPP
