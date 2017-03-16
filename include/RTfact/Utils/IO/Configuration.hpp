/*
 *  Configuration.hpp
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
 *  Created on: 2008-11-07 18:41:48 +0100
 *  Author(s): Iliyan Georgiev
 */

 #ifndef RTFACT__CONFIGURATION_HPP
#define RTFACT__CONFIGURATION_HPP

#include <RTfact/Config/Common.hpp>

#include <fstream>
#include <sstream>
#include <map>
#include <set>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#define RTFACT__GLOBAL_GROUP_NAME "_Global_"

RTFACT_NAMESPACE_BEGIN

namespace IO {

class Configuration
{
    struct QuoteTrimPredicate
    {
        bool operator()(const char aChar)
        {
            return (aChar == '\'') || (aChar == '"');
        }
    };

    static void trimQuotes(
        std::string& aString)
    {
        static QuoteTrimPredicate trimPredicate;

        boost::algorithm::trim_if(aString, trimPredicate);
    }

public:

    class Entry :
        public std::string
    {
    public:

        template<
            class tType>
        Entry(
            const tType& aValue
        ) :
            std::string(boost::lexical_cast<std::string>(aValue))
        {}

        template<
            class tType>
        tType as() const
        {
            return boost::lexical_cast<tType>(*this);
        }
    };

    class EntryGroup
    {
        typedef std::map<std::string, Entry> t_EntryMap;

        t_EntryMap mEntries;
        std::string mName;

    public:

        EntryGroup()
        {}

        EntryGroup(
            const std::string& aName
        ) :
            mName(aName)
        {}

        RTFACT_DEFINE_PROPERTY(std::string&, Name);

        bool exists(
            const std::string& aKey) const
        {
            return mEntries.find(aKey) != mEntries.end();
        }

        template<
            class tType>
        void add(
            const std::string& aKey,
            const tType& aValue)
        {
            RTFACT_USER_ERROR_IF_F(exists(aKey),
                "Entry already exists: %1%/%2%=%3%",
                mName % aKey % (*this)[aKey]);

            mEntries.insert(t_EntryMap::value_type(aKey, Entry(aValue)));
        }

        const Entry& operator[](
            const std::string& aKey) const
        {
            RTFACT_USER_ERROR_IF_F(!exists(aKey),
                "Entry does not exist: %1%/%2%", mName % aKey);

            return mEntries.find(aKey)->second;
        }

        Entry& operator[](
            const std::string& aKey)
        {
            RTFACT_USER_ERROR_IF_F(!exists(aKey),
                "Entry does not exist: %1%/%2%", mName % aKey);

            return mEntries.find(aKey)->second;
        }
    };

protected:

    typedef std::map<std::string, EntryGroup> t_GroupMap;

    t_GroupMap mGroups;

public:

    Configuration()
    {
        mGroups.insert(t_GroupMap::value_type(
            RTFACT__GLOBAL_GROUP_NAME, EntryGroup(RTFACT__GLOBAL_GROUP_NAME)));
    }

    Configuration(
        const int argc,
        const char** argv)
    {
        //TODO: implement
    }

    Configuration(
        const std::string& aPath)
    {
        mGroups.insert(t_GroupMap::value_type(
            RTFACT__GLOBAL_GROUP_NAME, EntryGroup(RTFACT__GLOBAL_GROUP_NAME)));

        RTFACT_USER_ERROR_IF_F(!loadFromFile(aPath),
                    "Could not load configuration file: %1%", aPath);
    }

    Configuration(
        std::istream& aStream)
    {
        mGroups.insert(t_GroupMap::value_type(
            RTFACT__GLOBAL_GROUP_NAME, EntryGroup(RTFACT__GLOBAL_GROUP_NAME)));

        loadFromStream(aStream);
    }

    bool loadFromFile(
        const std::string& aPath)
    {
        std::ifstream inStream(aPath.c_str());
        if(!inStream.good())
        {
            return false;
        }

        return loadFromStream(inStream);
    }

    bool loadFromStream(
        std::istream& aStream)
    {
        uint lineNum = 0;

        std::string curGroup(RTFACT__GLOBAL_GROUP_NAME);
        bool curGroupIsUsed = false;

        typedef std::set<std::string> t_UsedGroupSet;
        t_UsedGroupSet usedGroups;

        while(aStream.good() && !aStream.eof())
        {
            ++lineNum;

            std::string curLine;
            std::getline(aStream, curLine);
            boost::algorithm::trim(curLine);

            bool singleQuoteOpened = false;
            bool doubleQuoteOpened = false;
            size_t firstSemiColonIndex = size_t(-1);

            // remove comments, except between quotes
            for(uint i = 0; i < curLine.length(); ++i)
            {
                if(curLine[i] == ';' && !singleQuoteOpened && !doubleQuoteOpened)
                {
                    firstSemiColonIndex = i;

                    break;
                }

                if(curLine[i] == '\'')
                {
                    singleQuoteOpened = !singleQuoteOpened;
                }
                else if(curLine[i] == '"')
                {
                    doubleQuoteOpened = !doubleQuoteOpened;
                }
            }

            if(firstSemiColonIndex != size_t(-1))
            {
                curLine = curLine.substr(0, firstSemiColonIndex);
                boost::algorithm::trim(curLine);
            }

            std::istringstream issLine(curLine);
            std::string firstWord;
            issLine >> firstWord >> std::ws;

            if(firstWord.length() == 0)
            {
                continue;
            }

            if(firstWord[0] == '[') // group
            {
                curGroup = curLine.substr(1, curLine.length() - 2);

                mGroups.insert(t_GroupMap::value_type(
                    curGroup, EntryGroup(curGroup)));

                curGroupIsUsed = (usedGroups.find(curGroup) != usedGroups.end());
            }
            else if(firstWord[0] != '\r' && firstWord[0] != '\n' &&
                    firstWord[0] != '=') // entry
            {
                size_t delimIndex = curLine.find('=');

                std::string key = curLine.substr(0, delimIndex);
                std::string value = curLine.substr(delimIndex + 1, curLine.length() - delimIndex);

                trimQuotes(key);
                trimQuotes(value);

                RTFACT_USER_ERROR_IF_F(key.length() == 0,
                    "Invalid key on line: %1%", lineNum);

                RTFACT_USER_ERROR_IF_F(value.length() == 0,
                    "Empty value on line: %1%", lineNum);

                if(curGroup == RTFACT__GLOBAL_GROUP_NAME && key == "use")
                {
                    usedGroups.insert(value);
                }
                else if(curGroupIsUsed)
                {
                    add(key, value);
                }

                mGroups[curGroup].add(key, value);
            }
        }
        return true;
    }

    bool exists(
        const std::string& aKey) const
    {
        return mGroups.find(RTFACT__GLOBAL_GROUP_NAME)->second.exists(aKey);
    }

    bool existsGroup(
        const std::string& aName) const
    {
        return mGroups.find(aName) != mGroups.end();
    }

    EntryGroup& operator()(
        const std::string& aName)
    {
        t_GroupMap::iterator it = mGroups.find(aName);

        RTFACT_USER_ERROR_IF_F(it == mGroups.end(),
            "Group does not exist: %1%", aName);

        return it->second;
    }

    const EntryGroup& operator()(
        const std::string& aName) const
    {
        t_GroupMap::const_iterator it = mGroups.find(aName);

        RTFACT_USER_ERROR_IF_F(it == mGroups.end(),
            "Group does not exist: %1%", aName);

        return it->second;
    }

    Entry& operator[](
        const std::string& aKey)
    {
        RTFACT_USER_ERROR_IF_F(!exists(aKey),
            "Entry does not exist: %1%", aKey);

        return mGroups.find(RTFACT__GLOBAL_GROUP_NAME)->second[aKey];
    }

    const Entry& operator[](
        const std::string& aKey) const
    {
        RTFACT_USER_ERROR_IF_F(!exists(aKey),
            "Entry does not exist: %1%", aKey);

        return mGroups.find(RTFACT__GLOBAL_GROUP_NAME)->second[aKey];
    }

    template<
        class tType>
    void add(
        const std::string& aKey,
        const tType& aValue)
    {
        RTFACT_USER_ERROR_IF_F(exists(aKey),
            "Entry already exists: %1%", aKey);

        mGroups[RTFACT__GLOBAL_GROUP_NAME].add(aKey, aValue);
    }

    void addGroup(
        const std::string& aName)
    {
        RTFACT_USER_ERROR_IF_F(existsGroup(aName),
            "Group already exists: %1%", aName);

        mGroups.insert(
            t_GroupMap::value_type(aName, EntryGroup(aName))).first->second;
    }
};

template<>
RTFACT_INLINE Vec3f1 Configuration::Entry::as<Vec3f1>() const
{
    std::istringstream issLine(*this);
    char comma;
    Vec3f1 res;

    if(this->find(',') != std::string::npos)
    {
        issLine >> res.x >> comma >> res.y >> comma >> res.z;
    }
    else
    {
        issLine >> res.x >> res.y >> res.z;
    }

    return res;
}

template<>
RTFACT_INLINE bool Configuration::Entry::as<bool>() const
{
    return (*this == "1" || *this == "true");
}

} // namespace IO

RTFACT_NAMESPACE_END

#undef RTFACT__GLOBAL_GROUP_NAME

#endif // RTFACT__CONFIGURATION_HPP
