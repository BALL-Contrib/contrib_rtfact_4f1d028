/* 
 *  CompileTimeMathUtils.hpp
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
 
 #ifndef RTFACT__COMPILE_TIME_MATH_UTILS_HPP
#define RTFACT__COMPILE_TIME_MATH_UTILS_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN
namespace Math {

#pragma region Square root

// Definition:

template<
    uint taSize,
    uint taLow = 1,
    uint taHigh = taSize>
struct SquareRoot;

template<
    uint taSize,
    uint taMid>
struct SquareRoot<taSize, taMid, taMid>
{
    enum
    {
        value = taMid
    };
};

template<
    uint taSize,
    uint taLow,
    uint taHigh>
struct SquareRoot
{
private:

    enum
    {
        mMean = (taLow + taHigh) / 2,
        mDown = (mMean * mMean >= taSize)
    };

public:

    enum
    {
        value =
            SquareRoot<
                taSize,
                (mDown ? taLow : mMean + 1),
                (mDown ? mMean : taHigh)>::value
    };
};

/* Example use:

    const uint root = Root<16>::value;

*/

#pragma endregion // Square root

} // namespace Math
RTFACT_NAMESPACE_END

#endif // RTFACT__COMPILE_TIME_MATH_UTILS_HPP
