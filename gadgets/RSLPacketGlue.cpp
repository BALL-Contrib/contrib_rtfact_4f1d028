/**
 * @file   RSLPacketGlue.cpp
 * @date   13.12.2009
 * @author Ralf Karrenberg
 *
 * Copyright (C) 2009 Saarland University
 * Released under the GPL
 */

#include "RSLGlue.h"

static const __m128 _mm_one = _mm_set_ps1(1.f);

//----------------------------------------------------------------------------//
// empty packet prototype (generated by packetizer at runtime)
//----------------------------------------------------------------------------//
#ifdef USE_DERIVATIVES
extern "C" void __attribute__((noinline)) inner_shade_4(
	void* __restrict aContext,
	void* __restrict aParams,
	void* __restrict aTextureTable,
	const __m128i mask,
	const __m128 posX,
	const __m128 posY,
	const __m128 posZ,
	const __m128 originX,
	const __m128 originY,
	const __m128 originZ,
	const __m128 dirX,
	const __m128 dirY,
	const __m128 dirZ,
	const __m128 normalX,
	const __m128 normalY,
	const __m128 normalZ,
	const __m128 geomNormalX,
	const __m128 geomNormalY,
	const __m128 geomNormalZ,
	const __m128 vertColorX,
	const __m128 vertColorY,
	const __m128 vertColorZ,
	const __m128 dPduX,
	const __m128 dPduY,
	const __m128 dPduZ,
	const __m128 dPdvX,
	const __m128 dPdvY,
	const __m128 dPdvZ,
	const __m128 u,
	const __m128 v,
	const __m128 du,
	const __m128 dv,
	const __m128 texCoord1,
	const __m128 texCoord2,
	const __m128 dT1du,
	const __m128 dT1dv,
	const __m128 dT2du,
	const __m128 dT2dv,
	const unsigned depth,
	const __m128 time,
	__m128* __restrict resX,
	__m128* __restrict resY,
	__m128* __restrict resZ
);
#else
extern "C" void __attribute__((noinline)) inner_shade_4(
	void* __restrict aContext,
	void* __restrict aParams,
	void* __restrict aTextureTable,
	const __m128i mask,
	const __m128 posX,
	const __m128 posY,
	const __m128 posZ,
	const __m128 originX,
	const __m128 originY,
	const __m128 originZ,
	const __m128 dirX,
	const __m128 dirY,
	const __m128 dirZ,
	const __m128 normalX,
	const __m128 normalY,
	const __m128 normalZ,
	const __m128 geomNormalX,
	const __m128 geomNormalY,
	const __m128 geomNormalZ,
	const __m128 vertColorX,
	const __m128 vertColorY,
	const __m128 vertColorZ,
	const __m128 u,
	const __m128 v,
	const __m128 texCoord1,
	const __m128 texCoord2,
	const unsigned depth,
	const __m128 time,
	__m128* __restrict resX,
	__m128* __restrict resY,
	__m128* __restrict resZ
);
#endif

//----------------------------------------------------------------------------//
// external packet API functions (RTfact side, implemented in RTAnySLShaderAPI.hpp)
//----------------------------------------------------------------------------//
extern "C" void RTFACT_trace_4(
	const void     * __restrict aContext,
	const __m128i               mask,
	const __m128                aPositionX,
	const __m128                aPositionY,
	const __m128                aPositionZ,
	const __m128                aDirectionX,
	const __m128                aDirectionY,
	const __m128                aDirectionZ,
	const unsigned              aDepth,
	__m128         * __restrict oResultX,
	__m128         * __restrict oResultY,
	__m128         * __restrict oResultZ
);

extern "C" void RTFACT_getLC_4(
	const void    * __restrict aContext,
	const __m128i              mask,
	const __m128               aPositionX,
	const __m128               aPositionY,
	const __m128               aPositionZ,
	const __m128               aDirectionX, //not negated yet!
	const __m128               aDirectionY, //not negated yet!
	const __m128               aDirectionZ, //not negated yet!
	const __m128               aDistance,
	__m128i       * __restrict oContributes, //not yet set to 0!
	__m128        * __restrict oIntensityX,
	__m128        * __restrict oIntensityY,
	__m128        * __restrict oIntensityZ
);

extern "C" void RTFACT_sampleLight_4(
	const void     * __restrict aContext,
	const __m128i               mask,
	const unsigned              aLightIndex,
	const __m128                aPointX,
	const __m128                aPointY,
	const __m128                aPointZ,
	__m128         * __restrict oDirectionX,
	__m128         * __restrict oDirectionY,
	__m128         * __restrict oDirectionZ,
	__m128         * __restrict oDistance,
	__m128         * __restrict oRadianceX,
	__m128         * __restrict oRadianceY,
	__m128         * __restrict oRadianceZ,
	__m128         * __restrict oPositionX,
	__m128         * __restrict oPositionY,
	__m128         * __restrict oPositionZ
);

extern "C" void RTFACT_sampleDT_4
(
	const void    * __restrict aContext,
	const __m128i              mask,
	const void    * __restrict aTextureHandle,
	const __m128               aTexCoordX,
	const __m128               aTexCoordY,
	const __m128               aTexCoordOffsetX,
	const __m128               aTexCoordOffsetY,
	__m128        * __restrict oResultX,
	__m128        * __restrict oResultY,
	__m128        * __restrict oResultZ
);

extern "C" void RTFACT_sampleDT_id_4
(
	const void     * __restrict aContext,
	const __m128i               mask,
	void           * __restrict aTextureTable,
	const unsigned              aTextureID,
	const __m128                aTexCoordX,
	const __m128                aTexCoordY,
	const __m128                aTexCoordOffsetX,
	const __m128                aTexCoordOffsetY,
	__m128         * __restrict oResultX,
	__m128         * __restrict oResultY,
	__m128         * __restrict oResultZ
);



//----------------------------------------------------------------------------//
// RTfact shade function implementation
//----------------------------------------------------------------------------//
// specialization for single-ray packets (independent of packetization)
// HACK: it is difficult to link different shade-functions (scalar/packetized)
//       together because of context, type replacement etc. that have to be
//       performed before linking (resulting in double definitions of trace,
//       get_params etc.).
//       Thus, in the packetized case, we simply call the packetized version
//       also for single-ray packets and set the mask to 0.
//       TODO: check introduced overhead (value copying to SSE types)
template<bool taCommonOrg>
void JitCall<taCommonOrg, 1>::shade_wrapper(
		void * __restrict aContext,
		void * __restrict aParams,
		void * __restrict aTextureTable,
		const float time,
		const RTfact::RayPacket<1>* __restrict aRayPacket,
		const RTfact::ActiveMask<1>* __restrict aRayMask,
		const RTfact::SurfaceIntersection<1>* __restrict aIntersection,
		RTfact::Vec3f<1>* __restrict oResult)
{
	using RTfact::uint;

	if (aRayMask->isFalse()) return;

	//TODO: use aIntersection->point
	const RTfact::Vec3f<1> position = aRayPacket->org + aIntersection->dist * aRayPacket->dir;

	const __m128 _mm_time = _mm_set_ps1(time);
	const unsigned depth = aRayPacket->depth;

	// default opacity is set in RSLGlue.h to be able to optimize it out
	// in shaders that do not use it. If we want a different behaviour, we have
	// to trade that for some performance.
//	__m128 opacityX = _mm_one; //Incident ray opacity, default=1
//	__m128 opacityY = _mm_one;
//	__m128 opacityZ = _mm_one;

#ifdef USE_DERIVATIVES
	inner_shade_4(
		aContext,
		aParams,
		aTextureTable,
		_mm_set_epi32(-1, 0, 0, 0),
		(const __m128&)position(0).x,
		(const __m128&)position(0).y,
		(const __m128&)position(0).z,
		(const __m128&)aRayPacket->org(0).x,
		(const __m128&)aRayPacket->org(0).y,
		(const __m128&)aRayPacket->org(0).z,
		(const __m128&)aRayPacket->dir(0).x,
		(const __m128&)aRayPacket->dir(0).y,
		(const __m128&)aRayPacket->dir(0).z,
		(const __m128&)aIntersection->normal(0).x,
		(const __m128&)aIntersection->normal(0).y,
		(const __m128&)aIntersection->normal(0).z,
		(const __m128&)aIntersection->geomNormal(0).x,
		(const __m128&)aIntersection->geomNormal(0).y,
		(const __m128&)aIntersection->geomNormal(0).z,
		(const __m128&)aIntersection->vertexColor(0).x,
		(const __m128&)aIntersection->vertexColor(0).y,
		(const __m128&)aIntersection->vertexColor(0).z,
		(const __m128&)aIntersection->dpdu(0).x,
		(const __m128&)aIntersection->dpdu(0).y,
		(const __m128&)aIntersection->dpdu(0).z,
		(const __m128&)aIntersection->dpdv(0).x,
		(const __m128&)aIntersection->dpdv(0).y,
		(const __m128&)aIntersection->dpdv(0).z,
		(const __m128&)aIntersection->u(0),
		(const __m128&)aIntersection->v(0),
		(const __m128&)aIntersection->du(0),
		(const __m128&)aIntersection->dv(0),
		(const __m128&)aIntersection->texCoord1(0),
		(const __m128&)aIntersection->texCoord2(0),
		(const __m128&)aIntersection->dt1du(0),
		(const __m128&)aIntersection->dt1du(0),
		(const __m128&)aIntersection->dt2dv(0),
		(const __m128&)aIntersection->dt2dv(0),
		depth,
		_mm_time,
		(__m128*)&(*oResult)(0).x,
		(__m128*)&(*oResult)(0).y,
		(__m128*)&(*oResult)(0).z
	);
#else
	inner_shade_4(
		aContext,
		aParams,
		aTextureTable,
		_mm_set_epi32(-1, 0, 0, 0),
		(const __m128&)position(0).x,
		(const __m128&)position(0).y,
		(const __m128&)position(0).z,
		(const __m128&)aRayPacket->org(0).x,
		(const __m128&)aRayPacket->org(0).y,
		(const __m128&)aRayPacket->org(0).z,
		(const __m128&)aRayPacket->dir(0).x,
		(const __m128&)aRayPacket->dir(0).y,
		(const __m128&)aRayPacket->dir(0).z,
		(const __m128&)aIntersection->normal(0).x,
		(const __m128&)aIntersection->normal(0).y,
		(const __m128&)aIntersection->normal(0).z,
		(const __m128&)aIntersection->geomNormal(0).x,
		(const __m128&)aIntersection->geomNormal(0).y,
		(const __m128&)aIntersection->geomNormal(0).z,
		(const __m128&)aIntersection->vertexColor(0).x,
		(const __m128&)aIntersection->vertexColor(0).y,
		(const __m128&)aIntersection->vertexColor(0).z,
		(const __m128&)aIntersection->u(0),
		(const __m128&)aIntersection->v(0),
		(const __m128&)aIntersection->texCoord1(0),
		(const __m128&)aIntersection->texCoord2(0),
		depth,
		_mm_time,
		(__m128*)&(*oResult)(0).x,
		(__m128*)&(*oResult)(0).y,
		(__m128*)&(*oResult)(0).z
	);
#endif

}


// packet shading (4 rays in parallel)
template<bool taCommonOrg, RTfact::uint taSize>
void JitCall<taCommonOrg, taSize>::shade_wrapper(
		void * __restrict aContext,
		void * __restrict aParams,
		void * __restrict aTextureTable,
		const float time,
		const RTfact::RayPacket<taSize>* __restrict aRayPacket,
		const RTfact::ActiveMask<taSize>* __restrict aRayMask,
		const RTfact::SurfaceIntersection<taSize>* __restrict aIntersection,
		RTfact::Vec3f<taSize>* __restrict oResult)
{
	using RTfact::uint;

	//TODO: use aIntersection->point
	const RTfact::Vec3f<taSize> position = aRayPacket->org + aIntersection->dist * aRayPacket->dir;

	const __m128 _mm_time = _mm_set_ps1(time);
	const unsigned depth = aRayPacket->depth;

	RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, (*aRayMask))
	{
		// default opacity is set in RSLGlue.h to be able to optimize it out
		// in shaders that do not use it. If we want a different behaviour, we have
		// to trade that for some performance.
//		__m128 opacityX = _mm_one; //Incident ray opacity, default=1
//		__m128 opacityY = _mm_one;
//		__m128 opacityZ = _mm_one;

#ifdef USE_DERIVATIVES
		inner_shade_4(
			aContext,
			aParams,
			aTextureTable,
			(const __m128i&)(*aRayMask)(i),
			(const __m128&)position(i).x,
			(const __m128&)position(i).y,
			(const __m128&)position(i).z,
			(const __m128&)aRayPacket->org(i).x,
			(const __m128&)aRayPacket->org(i).y,
			(const __m128&)aRayPacket->org(i).z,
			(const __m128&)aRayPacket->dir(i).x,
			(const __m128&)aRayPacket->dir(i).y,
			(const __m128&)aRayPacket->dir(i).z,
			(const __m128&)aIntersection->normal(i).x,
			(const __m128&)aIntersection->normal(i).y,
			(const __m128&)aIntersection->normal(i).z,
			(const __m128&)aIntersection->geomNormal(i).x,
			(const __m128&)aIntersection->geomNormal(i).y,
			(const __m128&)aIntersection->geomNormal(i).z,
			(const __m128&)aIntersection->vertexColor(i).x,
			(const __m128&)aIntersection->vertexColor(i).y,
			(const __m128&)aIntersection->vertexColor(i).z,
			(const __m128&)aIntersection->dpdu(i).x,
			(const __m128&)aIntersection->dpdu(i).y,
			(const __m128&)aIntersection->dpdu(i).z,
			(const __m128&)aIntersection->dpdv(i).x,
			(const __m128&)aIntersection->dpdv(i).y,
			(const __m128&)aIntersection->dpdv(i).z,
			(const __m128&)aIntersection->u(i),
			(const __m128&)aIntersection->dv(i),
			(const __m128&)aIntersection->du(i),
			(const __m128&)aIntersection->v(i),
			(const __m128&)aIntersection->texCoord1(i),
			(const __m128&)aIntersection->texCoord2(i),
			(const __m128&)aIntersection->dt1du(i),
			(const __m128&)aIntersection->dt1du(i),
			(const __m128&)aIntersection->dt2dv(i),
			(const __m128&)aIntersection->dt2dv(i),
			depth,
			_mm_time,
			(__m128*)&(*oResult)(i).x,
			(__m128*)&(*oResult)(i).y,
			(__m128*)&(*oResult)(i).z
		);
#else
		inner_shade_4(
			aContext,
			aParams,
			aTextureTable,
			(const __m128i&)(*aRayMask)(i),
			(const __m128&)position(i).x,
			(const __m128&)position(i).y,
			(const __m128&)position(i).z,
			(const __m128&)aRayPacket->org(i).x,
			(const __m128&)aRayPacket->org(i).y,
			(const __m128&)aRayPacket->org(i).z,
			(const __m128&)aRayPacket->dir(i).x,
			(const __m128&)aRayPacket->dir(i).y,
			(const __m128&)aRayPacket->dir(i).z,
			(const __m128&)aIntersection->normal(i).x,
			(const __m128&)aIntersection->normal(i).y,
			(const __m128&)aIntersection->normal(i).z,
			(const __m128&)aIntersection->geomNormal(i).x,
			(const __m128&)aIntersection->geomNormal(i).y,
			(const __m128&)aIntersection->geomNormal(i).z,
			(const __m128&)aIntersection->vertexColor(i).x,
			(const __m128&)aIntersection->vertexColor(i).y,
			(const __m128&)aIntersection->vertexColor(i).z,
			(const __m128&)aIntersection->u(i),
			(const __m128&)aIntersection->v(i),
			(const __m128&)aIntersection->texCoord1(i),
			(const __m128&)aIntersection->texCoord2(i),
			depth,
			_mm_time,
			(__m128*)&(*oResult)(i).x,
			(__m128*)&(*oResult)(i).y,
			(__m128*)&(*oResult)(i).z
		);
#endif

	}
}


#define INTERFACE_SHADE_X(taCommonOrg, taSize)                    \
extern "C" void generic_masked_shade_##taCommonOrg##taSize        \
(                                                                 \
		void * __restrict aContext,                                          \
		void * __restrict aParamPtr,                                         \
		void * __restrict aTextureTable,                                     \
        const float time,                                         \
		const RTfact::RayPacket<taSize>* __restrict aRayPacket,              \
		const RTfact::ActiveMask<taSize>* __restrict aRayMask,               \
		const RTfact::SurfaceIntersection<taSize>* __restrict aIntersection, \
		RTfact::Vec3f<taSize>* __restrict oResult                            \
)                                                                 \
{                                                                 \
	JitCall<taCommonOrg, taSize>::shade_wrapper(                  \
		aContext, aParamPtr, aTextureTable, time, aRayPacket, aRayMask,          \
		aIntersection, oResult);                                  \
}                                                                 \

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





// dummy function for fake calls
// required in order to prevent dead function removal of packetized API declarations
void __fake_call__(void* voidptr, __m128 v, __m128* vp, __m128i vi, __m128i* vip, unsigned u)
{
	RTFACT_trace_4(
		voidptr,
		vi,
		v, v, v, v, v, v,
		u, vp, vp, vp);

	RTFACT_getLC_4(
		voidptr,
		vi,
        v, v, v, v, v, v, v,
        vip, vp, vp, vp);

	RTFACT_sampleLight_4(
        voidptr,
        vi,
        u,
        v, v, v,
        vp, vp, vp, vp, vp, vp, vp, vp, vp, vp);

	RTFACT_sampleDT_4(
			voidptr,
			vi,
			voidptr,
			v, v, v, v,
			vp, vp, vp);

	RTFACT_sampleDT_id_4(
			voidptr,
			vi,
			voidptr,
			u,
			v, v, v, v,
			vp, vp, vp);
}
