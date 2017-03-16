/* 
 *  TransformTools.hpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_RTPIE__TRANSFORM_HPP
#define RTFACT_RTPIE__TRANSFORM_HPP

#include <cstring>

RTFACT_RTPIE_NAMESPACE_BEGIN

struct TransformTools
{
    static void GetIdentity(float *aMatrix, float *aInvMatrix){
        static const float matID[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
        if(aMatrix)
            memcpy(aMatrix, matID, sizeof(float) * 16);
        if(aInvMatrix)
            memcpy(aInvMatrix, matID, sizeof(float) * 16);
    }

    static void GetTranslation(float aX, float aY, float aZ,
            float *aMatrix, float *aInvMatrix){
        const float mat[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            aX, aY, aZ, 1};
        const float matInv[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -aX, -aY, -aZ, 1};
        if(aMatrix)
            memcpy(aMatrix, mat, sizeof(float) * 16);
        if(aInvMatrix)
            memcpy(aInvMatrix, matInv, sizeof(float) * 16);
    }
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGE_HPP

