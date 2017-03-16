/*
 * ScalarWrapper.cpp
 *
 *  Created on: Apr 2, 2009
 *      Author: moll
 *
 * These functions are only executed if packetization is not enabled.
 *
 * If this module is linked into the shader glue module, the calls to masked_shade_4
 * and masked_shade_4_commonOrigin are resolved with the functions implemented
 * here instead of the automatically generated (packetized) ones.
 */
#include <xmmintrin.h>

// helper functions that extract the i'th component of a SIMD vector
float & extract(const __m128 & data, unsigned i) {
	return ((float*) (&data))[i];
}
unsigned & extract(const __m128i & data, unsigned i) {
	return ((unsigned*) (&data))[i];
}

// This is the scalar function which is called inside the inner loop of
// masked_shade_4 / masked_shade_4_commonOrigin.
// Its implementation is in ShaderGlue.cpp.
extern "C" void masked_shade
(
        void * __restrict  context,
        const unsigned     rayType,
        const bool         inside,
        const int          primeBase,
        const float        cumulativeFactorX,
        const float        cumulativeFactorY,
        const float        cumulativeFactorZ,
        const int          randomSeed,
        const unsigned     maxDepth,
        const unsigned     depth,
        const unsigned     activeMask,
        const float        originX,
        const float        originY,
        const float        originZ,
        const float        dirX,
        const float        dirY,
        const float        dirZ,
        const unsigned     appearanceID,
        const float        hitDistance,
        const float        geomNormalX,
        const float        geomNormalY,
        const float        geomNormalZ,
        const float        normalX,
        const float        normalY,
        const float        normalZ,
        const float        vertColorX,
        const float        vertColorY,
        const float        vertColorZ,
        const float        u,
        const float        v,
        const float        texCoord1,
        const float        texCoord2,
        float * __restrict oResultX,
        float * __restrict oResultY,
        float * __restrict oResultZ,
        const float        tangentX,
        const float        tangentY,
        const float        tangentZ,
        const float        binormalX,
        const float        binormalY,
        const float        binormalZ
);


// wrapper functions that loop over the packets of size 4 and call the scalar function

extern "C" void masked_shade_4
(
        void * __restrict   context,
        const unsigned      rayType,
        const bool          inside,
        const __m128i       primeBase,
        const __m128        cumulativeFactorX,
        const __m128        cumulativeFactorY,
        const __m128        cumulativeFactorZ,
        const __m128i       randomSeed,
        const unsigned      maxDepth,
        const unsigned      depth,
        const __m128i	    activeMask,
        const __m128        originX,
        const __m128        originY,
        const __m128        originZ,
        const __m128        dirX,
        const __m128        dirY,
        const __m128        dirZ,
        const __m128i       appearanceID,
        const __m128        hitDistance,
        const __m128        geomNormalX,
        const __m128        geomNormalY,
        const __m128        geomNormalZ,
        const __m128 	    normalX,
        const __m128 	    normalY,
        const __m128  	    normalZ,
        const __m128	    vertColorX,
        const __m128 	    vertColorY,
        const __m128        vertColorZ,
        const __m128        u,
        const __m128        v,
        const __m128        texCoord1,
        const __m128        texCoord2,
        __m128 * __restrict oResultX,
        __m128 * __restrict oResultY,
        __m128 * __restrict oResultZ,
        const __m128        tangentX,
        const __m128        tangentY,
        const __m128        tangentZ,
        const __m128        binormalX,
        const __m128        binormalY,
        const __m128        binormalZ
)
{
	for(int i = 0; i < 4 ; ++i)
	{
		masked_shade(
            context,
            rayType,
            inside,
            extract(primeBase, i),
            extract(cumulativeFactorX, i),
            extract(cumulativeFactorY, i),
            extract(cumulativeFactorZ, i),
            extract(randomSeed, i),
            maxDepth,
            depth,
            extract(activeMask, i),
            extract(originX, i),
            extract(originY, i),
            extract(originZ, i),
            extract(dirX, i),
            extract(dirY, i),
            extract(dirZ, i),
            extract(appearanceID, i),
            extract(hitDistance, i),
            extract(geomNormalX, i),
            extract(geomNormalY, i),
            extract(geomNormalZ, i),
            extract(normalX, i),
            extract(normalY, i),
            extract(normalZ, i),
            extract(vertColorX, i),
            extract(vertColorY, i),
            extract(vertColorZ, i),
            extract(u, i),
            extract(v, i),
            extract(texCoord1, i),
            extract(texCoord2, i),
            &(extract(*oResultX, i)),
            &(extract(*oResultY, i)),
            &(extract(*oResultZ, i)),
            extract(tangentX, i),
            extract(tangentY, i),
            extract(tangentZ, i),
            extract(binormalX, i),
            extract(binormalY, i),
            extract(binormalZ, i)
		);
	}
}

extern "C" void masked_shade_4_commonOrigin
(
        void * __restrict   context,
        const unsigned      rayType,
        const bool          inside,
        const __m128i       primeBase,
        const __m128        cumulativeFactorX,
        const __m128        cumulativeFactorY,
        const __m128        cumulativeFactorZ,
        const __m128i       randomSeed,
        const unsigned      maxDepth,
        const unsigned      depth,
        const __m128i	    activeMask,
        const float         originX,
        const float         originY,
        const float         originZ,
        const __m128        dirX,
        const __m128        dirY,
        const __m128        dirZ,
        const __m128i       appearanceID,
        const __m128        hitDistance,
        const __m128        geomNormalX,
        const __m128        geomNormalY,
        const __m128        geomNormalZ,
        const __m128 	    normalX,
        const __m128 	    normalY,
        const __m128  	    normalZ,
        const __m128	    vertColorX,
        const __m128 	    vertColorY,
        const __m128        vertColorZ,
        const __m128        u,
        const __m128        v,
        const __m128        texCoord1,
        const __m128        texCoord2,
        __m128 * __restrict oResultX,
        __m128 * __restrict oResultY,
        __m128 * __restrict oResultZ,
        const __m128        tangentX,
        const __m128        tangentY,
        const __m128        tangentZ,
        const __m128        binormalX,
        const __m128        binormalY,
        const __m128        binormalZ
)
{
	for(int i = 0; i < 4 ; ++i)
	{
		masked_shade(
            context,
            rayType,
            inside,
            extract(primeBase, i),
            extract(cumulativeFactorX, i),
            extract(cumulativeFactorY, i),
            extract(cumulativeFactorZ, i),
            extract(randomSeed, i),
            maxDepth,
            depth,
            extract(activeMask, i),
            originX,
            originY,
            originZ,
            extract(dirX, i),
            extract(dirY, i),
            extract(dirZ, i),
            extract(appearanceID, i),
            extract(hitDistance, i),
            extract(geomNormalX, i),
            extract(geomNormalY, i),
            extract(geomNormalZ, i),
            extract(normalX, i),
            extract(normalY, i),
            extract(normalZ, i),
            extract(vertColorX, i),
            extract(vertColorY, i),
            extract(vertColorZ, i),
            extract(u, i),
            extract(v, i),
            extract(texCoord1, i),
            extract(texCoord2, i),
            &(extract(*oResultX, i)),
            &(extract(*oResultY, i)),
            &(extract(*oResultZ, i)),
            extract(tangentX, i),
            extract(tangentY, i),
            extract(tangentZ, i),
            extract(binormalX, i),
            extract(binormalY, i),
            extract(binormalZ, i)
		);
	}
}
