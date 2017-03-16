/* 
 *  Metafunctions.hpp
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
 *  Created on: 2010-06-11 12:01:36 +0200
 *  Author(s): Lukas Marsalek
 */
 
 #ifndef RTFACT__METAFUNCTIONS_HPP
#define RTFACT__METAFUNCTIONS_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

template <
    typename tEffect,
    typename tSoFar>
struct orPreRender
{
    typedef boost::mpl::bool_<tSoFar::value || tEffect::mHasPreRender> type;
};

template <
    typename tEffect,
    typename tSoFar>
struct orPrePass
{
    typedef boost::mpl::bool_<tSoFar::value || tEffect::mHasPrePass!=0> type;
};

template <
    typename tEffect,
    typename tSoFar>
struct orPostPass
{
    typedef boost::mpl::bool_<tSoFar::value || tEffect::mHasPostPass!=0> type;
};

template <
    typename tEffect,
    typename tSoFar>
struct orPostRender
{
    typedef boost::mpl::bool_<tSoFar::value || tEffect::mHasPostRender> type;
};



template <
    typename tSequence1,
    typename tSequence2>
boost::fusion::zip_view<boost::fusion::vector<tSequence1&,tSequence2&> > zipPair(
    tSequence1 &aSequence1,
    tSequence2 &aSequence2)
{
    typedef boost::fusion::vector<tSequence1&,tSequence2&> Zipper;
    return boost::fusion::zip_view<Zipper> (Zipper(aSequence1,aSequence2));
};

RTFACT_NAMESPACE_END

#endif // RTFACT__METAFUNCTIONS_HPP
