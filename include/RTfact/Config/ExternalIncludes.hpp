/* 
 *  ExternalIncludes.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */
 
 #ifndef RTFACT__INCLUDES_HPP
#define RTFACT__INCLUDES_HPP

#include <RTfact/Config/Common.hpp>

#pragma region Standard library

#include <algorithm>
#include <cassert>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <float.h>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <stack>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>

#pragma endregion // Standard library

#pragma region Intrinsics

#if defined(_WIN32) && (defined(_MSC_VER) || defined(__INTEL_COMPILER))
#   include <intrin.h>
#endif

#include <xmmintrin.h>
#include <emmintrin.h>

#pragma endregion // Intrinsics

#pragma region Boost

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/lambda/bind.hpp>

#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/identity.hpp>
#include <boost/preprocessor/iteration/local.hpp>

#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/filter.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/push_back.hpp>
#include <boost/preprocessor/seq/size.hpp>

#include <boost/fusion/container.hpp>
#include <boost/foreach.hpp>

#pragma endregion // Boost

#endif // RTFACT__INCLUDES_HPP
