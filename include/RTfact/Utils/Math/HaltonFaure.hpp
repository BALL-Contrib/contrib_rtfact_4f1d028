/* 
 *  MathUtils.hpp
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
 *  Created on: Fri Mar 18 11:21:27 CET 2011 @473 /Internet Time/
 *  Author(s): Vaclav Krajicek
 */

#ifndef RTFACT__HALTONFAURE_HPP
#define RTFACT__HALTONFAURE_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN
namespace Math {

namespace Detail {
	extern const int PRECOMPUTED_PRIME_COUNT;
	extern const int PRECOMPUTED_FAURE_PERM_COUNT;
    extern const uint FAURE_PERMUTATION_OFFSETS[];
    extern const uint FAURE_PERMUTATIONS[];
    extern const int PRECOMPUTED_PRIMES[];
}

RTFACT_MATH_INLINE int getPrime(
    const uint aID)
{
	RTFACT_ASSERT(aID < (uint)Detail::PRECOMPUTED_PRIME_COUNT);

    return Detail::PRECOMPUTED_PRIMES[aID];
}

RTFACT_MATH_INLINE const uint* getFaurePermutation(
    const uint aID)
{
    RTFACT_ASSERT(aID < (uint)Detail::PRECOMPUTED_FAURE_PERM_COUNT);

    return Detail::FAURE_PERMUTATIONS + Detail::FAURE_PERMUTATION_OFFSETS[aID];
}

} // namespace Math
RTFACT_NAMESPACE_END

#endif // RTFACT__HALTONFAURE_HPP