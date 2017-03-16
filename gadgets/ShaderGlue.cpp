#include <RTfact/Utils/Packets/Vec3f.hpp>
#include <RTfact/Utils/SurfaceIntersection.hpp>
#include <RTfact/Utils/RayPacket.hpp>

#define ANYSL_USE_COMMON_ORG

//################################################# RTfact to shader wrappers ##################################
// - masked_shade comes from shader bitcode file
// - masked_shade_4 & masked_shade_4_commonOrigin come from packetizer
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
);

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
);



//
// Wrapper class for arbitrarily sized containers
//
template<bool taCommonOrg, RTfact::uint taSize>
struct JitCall
{
	RTFACT_APPEARANCE_INLINE static void generic_shade(
			void * context,
			const RTfact::RayPacket<taSize>& aRayPacket,
			const RTfact::ActiveMask<taSize>& aRayMask,
			const RTfact::SurfaceIntersection<taSize>& aIntersection,
			RTfact::Vec3f<taSize>& oResult)
	{
    //printf("generic_shade_taSize \n");

        using RTfact::uint;
        const unsigned depth = aRayPacket.depth;
        const unsigned maxDepth = aRayPacket.maxDepth;
		RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
		{

#ifdef ANYSL_USE_COMMON_ORG
			// use common org optimization
			if (taCommonOrg) {
            	masked_shade_4_commonOrigin
					(
						context,
            aRayPacket.rayType,
            aRayPacket.inside,
            (const __m128i&)aRayPacket.primeBase(i),
            (const __m128&)aRayPacket.cumulativeFactor(i).x,
            (const __m128&)aRayPacket.cumulativeFactor(i).y,
            (const __m128&)aRayPacket.cumulativeFactor(i).z,
            (const __m128i&)aRayPacket.randomSeed(i),
            maxDepth,
						depth,
						(const __m128i&)aRayMask(i),
						aRayPacket.org(0).xAt(0),
						aRayPacket.org(0).yAt(0),
						aRayPacket.org(0).zAt(0),
						(const __m128&)aRayPacket.dir(i).x,
						(const __m128&)aRayPacket.dir(i).y,
						(const __m128&)aRayPacket.dir(i).z,
						(const __m128i&)aIntersection.appearanceID(i),
						(const __m128&)aIntersection.dist(i),
						(const __m128&)aIntersection.geomNormal(i).x,
						(const __m128&)aIntersection.geomNormal(i).y,
						(const __m128&)aIntersection.geomNormal(i).z,
						(const __m128&)aIntersection.normal(i).x,
						(const __m128&)aIntersection.normal(i).y,
						(const __m128&)aIntersection.normal(i).z,
						(const __m128&)aIntersection.vertexColor(i).x,
						(const __m128&)aIntersection.vertexColor(i).y,
						(const __m128&)aIntersection.vertexColor(i).z,
						(const __m128&)aIntersection.u(i),
						(const __m128&)aIntersection.v(i),
						(const __m128&)aIntersection.texCoord1(i),
						(const __m128&)aIntersection.texCoord2(i),
						(__m128*)&oResult(i).x,
						(__m128*)&oResult(i).y,
						(__m128*)&oResult(i).z,
						(const __m128&)aIntersection.tangent(i).x,
						(const __m128&)aIntersection.tangent(i).y,
						(const __m128&)aIntersection.tangent(i).z,
						(const __m128&)aIntersection.binormal(i).x,
						(const __m128&)aIntersection.binormal(i).y,
						(const __m128&)aIntersection.binormal(i).z
					);
			} else {
#endif
            // either we don't use common origin optimization at all,
			// or the packet actually does not have a common origin
            masked_shade_4
                (
            context,
            aRayPacket.rayType,
            aRayPacket.inside,
            (const __m128i&)aRayPacket.primeBase(i),
            (const __m128&)aRayPacket.cumulativeFactor(i).x,
            (const __m128&)aRayPacket.cumulativeFactor(i).y,
            (const __m128&)aRayPacket.cumulativeFactor(i).z,
            (const __m128i&)aRayPacket.randomSeed(i),
            maxDepth,
            depth,
            (const __m128i&)aRayMask(i),
						(const __m128&)aRayPacket.org(i).x,
						(const __m128&)aRayPacket.org(i).y,
						(const __m128&)aRayPacket.org(i).z,
						(const __m128&)aRayPacket.dir(i).x,
						(const __m128&)aRayPacket.dir(i).y,
						(const __m128&)aRayPacket.dir(i).z,
						(const __m128i&)aIntersection.appearanceID(i),
						(const __m128&)aIntersection.dist(i),
						(const __m128&)aIntersection.geomNormal(i).x,
						(const __m128&)aIntersection.geomNormal(i).y,
						(const __m128&)aIntersection.geomNormal(i).z,
						(const __m128&)aIntersection.normal(i).x,
						(const __m128&)aIntersection.normal(i).y,
						(const __m128&)aIntersection.normal(i).z,
						(const __m128&)aIntersection.vertexColor(i).x,
						(const __m128&)aIntersection.vertexColor(i).y,
						(const __m128&)aIntersection.vertexColor(i).z,
						(const __m128&)aIntersection.u(i),
						(const __m128&)aIntersection.v(i),
						(const __m128&)aIntersection.texCoord1(i),
						(const __m128&)aIntersection.texCoord2(i),
						(__m128*)&oResult(i).x,
						(__m128*)&oResult(i).y,
						(__m128*)&oResult(i).z,
						(const __m128&)aIntersection.tangent(i).x,
						(const __m128&)aIntersection.tangent(i).y,
						(const __m128&)aIntersection.tangent(i).z,
						(const __m128&)aIntersection.binormal(i).x,
						(const __m128&)aIntersection.binormal(i).y,
						(const __m128&)aIntersection.binormal(i).z
                );

#ifdef ANYSL_USE_COMMON_ORG
			}
#endif

		}
	}
};

//
// Scalar shader for packet of size 1 (no looping necessary)
//
template<bool taCommonOrg>
struct JitCall<taCommonOrg, 1>
{
	RTFACT_APPEARANCE_INLINE static void generic_shade(
			void * context,
			const RTfact::RayPacket<1>& aRayPacket,
			const RTfact::ActiveMask<1>& aRayMask,
			const RTfact::SurfaceIntersection<1>& aIntersection,
			RTfact::Vec3f<1>& oResult)
	{
    //printf("generic_shade_1 \n");
    //printf("binormalZ = %f\n", aIntersection.binormal(0).zAt(0));

		masked_shade
		(
			context,
      aRayPacket.rayType,
      aRayPacket.inside,
      aRayPacket.primeBase,
      aRayPacket.cumulativeFactor(0).xAt(0),
      aRayPacket.cumulativeFactor(0).yAt(0),
      aRayPacket.cumulativeFactor(0).zAt(0),
      aRayPacket.randomSeed(0),
      aRayPacket.maxDepth,
			aRayPacket.depth,
			aRayMask(0)[0],
			aRayPacket.org(0).xAt(0),
			aRayPacket.org(0).yAt(0),
			aRayPacket.org(0).zAt(0),
			aRayPacket.dir(0).xAt(0),
			aRayPacket.dir(0).yAt(0),
			aRayPacket.dir(0).zAt(0),
			aIntersection.appearanceID(0),
			aIntersection.dist(0),
			aIntersection.geomNormal(0).xAt(0),
			aIntersection.geomNormal(0).yAt(0),
			aIntersection.geomNormal(0).zAt(0),
			aIntersection.normal(0).xAt(0),
			aIntersection.normal(0).yAt(0),
			aIntersection.normal(0).zAt(0),
			aIntersection.vertexColor(0).xAt(0),
			aIntersection.vertexColor(0).yAt(0),
			aIntersection.vertexColor(0).zAt(0),
			aIntersection.u(0),
			aIntersection.v(0),
			aIntersection.texCoord1(0),
			aIntersection.texCoord2(0),
			&oResult(0).xAt(0),
			&oResult(0).yAt(0),
			&oResult(0).zAt(0),
			aIntersection.tangent(0).xAt(0),
			aIntersection.tangent(0).yAt(0),
			aIntersection.tangent(0).zAt(0),
			aIntersection.binormal(0).xAt(0),
			aIntersection.binormal(0).yAt(0),
			aIntersection.binormal(0).zAt(0)
		);
	}
};

//
// Generic wrapper
//
// This function is the one that is actually called by RTfact
#define INTERFACE_SHADE_X(taCommonOrg, taSize)                    \
extern "C" void generic_masked_shade_##taCommonOrg##taSize        \
(                                                                 \
		void * aContext,                                          \
		const RTfact::RayPacket<taSize>& aRayPacket,              \
		const RTfact::ActiveMask<taSize>& aRayMask,               \
		const RTfact::SurfaceIntersection<taSize>& aIntersection, \
		RTfact::Vec3f<taSize>& oResult                            \
)                                                                 \
{                                                                 \
	JitCall<taCommonOrg, taSize>::generic_shade(                  \
		aContext, aRayPacket, aRayMask, aIntersection, oResult);  \
}

INTERFACE_SHADE_X(true,1)
INTERFACE_SHADE_X(true,4)
INTERFACE_SHADE_X(true,16)
INTERFACE_SHADE_X(true,64)
INTERFACE_SHADE_X(true,256)

INTERFACE_SHADE_X(false,1)
INTERFACE_SHADE_X(false,4)
INTERFACE_SHADE_X(false,16)
INTERFACE_SHADE_X(false,64)
INTERFACE_SHADE_X(false,256)

#undef INTERFACE_SHADE_X





