/**
 * @file   RSLGlue.cpp
 * @date   13.12.2009
 * @author Ralf Karrenberg
 *
 * Copyright (C) 2009 Saarland University
 * Released under the GPL
 */

#include "RSLGlue.h"

//----------------------------------------------------------------------------//
// RTfact shade function implementation
//----------------------------------------------------------------------------//
//specialization for single-ray packets (independent of packetization)
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

	//TODO: use aIntersection->point (Vec3f<taSize>)
	const RTfact::Vec3f<1> position = aRayPacket->org + aIntersection->dist * aRayPacket->dir;

	const unsigned depth = aRayPacket->depth;

	// default opacity is set in RSLGlue.h to be able to optimize it out
	// in shaders that do not use it. If we want a different behaviour, we have
	// to trade that for some performance.
//	float opacityX = 1.f; //Incident ray opacity, default=1
//	float opacityY = 1.f;
//	float opacityZ = 1.f;

#ifdef USE_DERIVATIVES
	inner_shade(
		aContext,
		aParams,
		aTextureTable,
		1u,
		position(0).xAt(0),
		position(0).yAt(0),
		position(0).zAt(0),
		aRayPacket->org(0).xAt(0),
		aRayPacket->org(0).yAt(0),
		aRayPacket->org(0).zAt(0),
		aRayPacket->dir(0).xAt(0),
		aRayPacket->dir(0).yAt(0),
		aRayPacket->dir(0).zAt(0),
		aIntersection->normal(0).xAt(0),
		aIntersection->normal(0).yAt(0),
		aIntersection->normal(0).zAt(0),
		aIntersection->geomNormal(0).xAt(0),
		aIntersection->geomNormal(0).yAt(0),
		aIntersection->geomNormal(0).zAt(0),
		aIntersection->vertexColor(0).xAt(0),
		aIntersection->vertexColor(0).yAt(0),
		aIntersection->vertexColor(0).zAt(0),
		aIntersection->dpdu(0).xAt(0),
		aIntersection->dpdu(0).yAt(0),
		aIntersection->dpdu(0).zAt(0),
		aIntersection->dpdv(0).xAt(0),
		aIntersection->dpdv(0).yAt(0),
		aIntersection->dpdv(0).zAt(0),
		aIntersection->u(0)[0],
		aIntersection->v(0)[0],
		aIntersection->du(0)[0],
		aIntersection->dv(0)[0],
		aIntersection->texCoord1(0)[0],
		aIntersection->texCoord2(0)[0],
		aIntersection->dt1du(0)[0],
		aIntersection->dt1dv(0)[0],
		aIntersection->dt2du(0)[0],
		aIntersection->dt2dv(0)[0],
		depth,
		time,
		&(*oResult)(0).xAt(0),
		&(*oResult)(0).yAt(0),
		&(*oResult)(0).zAt(0)
	);
#else
	inner_shade(
		aContext,
		aParams,
		aTextureTable,
		1u,
		position(0).xAt(0),
		position(0).yAt(0),
		position(0).zAt(0),
		aRayPacket->org(0).xAt(0),
		aRayPacket->org(0).yAt(0),
		aRayPacket->org(0).zAt(0),
		aRayPacket->dir(0).xAt(0),
		aRayPacket->dir(0).yAt(0),
		aRayPacket->dir(0).zAt(0),
		aIntersection->normal(0).xAt(0),
		aIntersection->normal(0).yAt(0),
		aIntersection->normal(0).zAt(0),
		aIntersection->geomNormal(0).xAt(0),
		aIntersection->geomNormal(0).yAt(0),
		aIntersection->geomNormal(0).zAt(0),
		aIntersection->vertexColor(0).xAt(0),
		aIntersection->vertexColor(0).yAt(0),
		aIntersection->vertexColor(0).zAt(0),
		aIntersection->u(0)[0],
		aIntersection->v(0)[0],
		aIntersection->texCoord1(0)[0],
		aIntersection->texCoord2(0)[0],
		depth,
		time,
		&(*oResult)(0).xAt(0),
		&(*oResult)(0).yAt(0),
		&(*oResult)(0).zAt(0)
	);
#endif

}

// scalar shading (all rays sequentially)
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

	//TODO: use aIntersection->point (Vec3f<taSize>)
	const RTfact::Vec3f<taSize> position = aRayPacket->org + aIntersection->dist * aRayPacket->dir;

	const unsigned depth = aRayPacket->depth;

	RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, (*aRayMask))
	{
		for (int j=0; j<4; ++j) {
			// default opacity is set in RSLGlue.h to be able to optimize it out
			// in shaders that do not use it. If we want a different behaviour, we have
			// to trade that for some performance.
//			float opacityX = 1.f; //Incident ray opacity, default=1
//			float opacityY = 1.f;
//			float opacityZ = 1.f;

#ifdef USE_DERIVATIVES
			inner_shade(
				aContext,
				aParams,
				aTextureTable,
				(*aRayMask)(i)[j],
				position(i).xAt(j),
				position(i).yAt(j),
				position(i).zAt(j),
				aRayPacket->org(i).xAt(j),
				aRayPacket->org(i).yAt(j),
				aRayPacket->org(i).zAt(j),
				aRayPacket->dir(i).xAt(j),
				aRayPacket->dir(i).yAt(j),
				aRayPacket->dir(i).zAt(j),
				aIntersection->normal(i).xAt(j),
				aIntersection->normal(i).yAt(j),
				aIntersection->normal(i).zAt(j),
				aIntersection->geomNormal(i).xAt(j),
				aIntersection->geomNormal(i).yAt(j),
				aIntersection->geomNormal(i).zAt(j),
				aIntersection->vertexColor(i).xAt(j),
				aIntersection->vertexColor(i).yAt(j),
				aIntersection->vertexColor(i).zAt(j),
				aIntersection->dpdu(i).xAt(j),
				aIntersection->dpdu(i).yAt(j),
				aIntersection->dpdu(i).zAt(j),
				aIntersection->dpdv(i).xAt(j),
				aIntersection->dpdv(i).yAt(j),
				aIntersection->dpdv(i).zAt(j),
				aIntersection->u(i)[j],
				aIntersection->v(i)[j],
				aIntersection->du(i)[j],
				aIntersection->dv(i)[j],
				aIntersection->texCoord1(i)[j],
				aIntersection->texCoord2(i)[j],
				aIntersection->dt1du(i)[j],
				aIntersection->dt1dv(i)[j],
				aIntersection->dt2du(i)[j],
				aIntersection->dt2dv(i)[j],
				depth,
				time,
				&(*oResult)(i).xAt(j),
				&(*oResult)(i).yAt(j),
				&(*oResult)(i).zAt(j)
			);
#else
			inner_shade(
				aContext,
				aParams,
				aTextureTable,
				(*aRayMask)(i)[j],
				position(i).xAt(j),
				position(i).yAt(j),
				position(i).zAt(j),
				aRayPacket->org(i).xAt(j),
				aRayPacket->org(i).yAt(j),
				aRayPacket->org(i).zAt(j),
				aRayPacket->dir(i).xAt(j),
				aRayPacket->dir(i).yAt(j),
				aRayPacket->dir(i).zAt(j),
				aIntersection->normal(i).xAt(j),
				aIntersection->normal(i).yAt(j),
				aIntersection->normal(i).zAt(j),
				aIntersection->geomNormal(i).xAt(j),
				aIntersection->geomNormal(i).yAt(j),
				aIntersection->geomNormal(i).zAt(j),
				aIntersection->vertexColor(i).xAt(j),
				aIntersection->vertexColor(i).yAt(j),
				aIntersection->vertexColor(i).zAt(j),
				aIntersection->u(i)[j],
				aIntersection->v(i)[j],
				aIntersection->texCoord1(i)[j],
				aIntersection->texCoord2(i)[j],
				depth,
				time,
				&(*oResult)(i).xAt(j),
				&(*oResult)(i).yAt(j),
				&(*oResult)(i).zAt(j)
			);
#endif

		}
	}
}


// instantiation for different packet sizes / common origin
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
