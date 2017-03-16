/**
 * @file   RSLGlue.h
 * @date   13.12.2009
 * @author Ralf Karrenberg
 *
 * Copyright (C) 2009 Saarland University
 * Released under the GPL
 *
 * This file implements all functions and declarations common to all packet sizes.
 * The following files implement the actual shader wrapper functions:
 * RSLGlue.cpp       - scalar functions (sequential shading of single rays)
 * RSLPacketGlue.cpp - packet functions (shading is performed on 4 rays in parallel)
 */

#ifndef RSLGLUE_H_INCLUDED
#define RSLGLUE_H_INCLUDED

#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>
#include <RTfact/Utils/Packets/BitMask.hpp>
#include <RTfact/Utils/SurfaceIntersection.hpp>
#include <RTfact/Utils/RayPacket.hpp>

// TODO: replace commontypes.hpp (use consistent type scheme)
// TODO: refactor RSLStruct to hold pointers to arrays (prevent copying)

//#define ANYSL_USE_COMMON_ORG //currently has no effect (not implemented in JitCalls)
//#define ANYSL_RSL_NO_OPACITY

//#define USE_DERIVATIVES

#ifndef ANYSL_USE_COMMON_ORG
	#warning "taCommonOrg optimization disabled"
#endif
#ifdef ANYSL_RSL_NO_OPACITY
	#warning "opacity disabled"
#endif

//----------------------------------------------------------------------------//
// internal datatype for all vectors
// NOTE: using a union with __m128 or RTFACT_ALIGN_SIMD prevents optimizations!!
//       NOT using RTFACT_ALIGN_SIMD gives wrong results and/or segfaults, though
//----------------------------------------------------------------------------//
#ifdef USE_DERIVATIVES
struct float_d {
	float x, u, v;
	float_d() {}
	float_d(float a) : x(a), u(0.f), v(0.f) {}
	float_d(float a, float au, float av) : x(a), u(au), v(av) {}
};
/*struct RSLVec3f3 {
	float_d x, y, z;
	RSLVec3f3() {}
	RSLVec3f3(float a, float b, float c) : x(a), y(b), z(c) {}
	RSLVec3f3(float_d a, float_d b, float_d c) : x(a), y(b), z(c) {}
};*/
struct RSLVec3f3 {
    float x, xu, xv;
    float y, yu, yv;
    float z, zu, zv;
	RSLVec3f3() {}
	RSLVec3f3(float a, float b, float c)
        : x(a), xu(0.f), xv(0.f), y(b), yu(0.f), yv(0.f), z(c), zu(0.f), zv(0.f) {} //remove initialization of u/v?
	RSLVec3f3(float a, float au, float av, float b, float bu, float bv, float c, float cu, float cv)
        : x(a), xu(au), xv(av), y(b), yu(bu), yv(bv), z(c), zu(cu), zv(cv) {}
	RSLVec3f3(float_d a, float_d b, float_d c) : x(a.x), xu(a.u), xv(a.v),
                                                 y(b.x), yu(b.u), yv(b.v),
                                                 z(c.x), zu(c.u), zv(c.v) {}
};
#else
struct RSLVec3f {
	float x, y, z;
	RSLVec3f() {}
	RSLVec3f(float a, float b, float c) : x(a), y(b), z(c) {}
};
#endif

//----------------------------------------------------------------------------//
// external API functions (RTfact side, implemented in RTAnySLShaderAPI.hpp)
//----------------------------------------------------------------------------//
extern "C" unsigned RTFACT_getNumLight
(
    const void* __restrict aContext
);

extern "C" void RTFACT_trace_1
(
	const void* __restrict aContext,
	const float aPositionX,
	const float aPositionY,
	const float aPositionZ,
	const float aDirectionX,
	const float aDirectionY,
	const float aDirectionZ,
    const unsigned aDepth,
	float* __restrict oResultX,
	float* __restrict oResultY,
	float* __restrict oResultZ
);
extern "C" void RTFACT_sampleLight_1
(
    const void     * __restrict aContext,
    const unsigned        light,
	const float aPointX,
	const float aPointY,
	const float aPointZ,
	float* __restrict oDirectionX,
	float* __restrict oDirectionY,
	float* __restrict oDirectionZ,
	float* __restrict oDistance,
	float* __restrict oRadianceX,
	float* __restrict oRadianceY,
	float* __restrict oRadianceZ,
	float* __restrict oPositionX,
	float* __restrict oPositionY,
	float* __restrict oPositionZ
);
extern "C" void RTFACT_getLC_1
(
    const void* __restrict aContext,
	const float aPositionX,
	const float aPositionY,
	const float aPositionZ,
	const float aDirectionX,
	const float aDirectionY,
	const float aDirectionZ,
	const float aDistance,
    unsigned* __restrict contributes,
	float* __restrict oIntensityX,
	float* __restrict oIntensityY,
	float* __restrict oIntensityZ
);
extern "C" void RTFACT_sampleDT_id_1 //TODO: push table access into glue code
(
    const void* __restrict aContext,
	const void* __restrict aTextureTable,
	const unsigned     id,
	const float        texCoordX,
	const float        texCoordY,
	const float        texCoordOffsetX,
	const float        texCoordOffsetY,
	float* __restrict oResultX,
	float* __restrict oResultY,
	float* __restrict oResultZ
);

//----------------------------------------------------------------------------//
// RSL specific context struct (internal, only used in inner_shade)
// This context serves as another list of parameters common to all shaders
//----------------------------------------------------------------------------//
#ifdef USE_DERIVATIVES
struct RSLContext
{
    void* rtContext;
    void* textureTable; //pointer to texture table, each index corresponds to one RTfact texture pointer

    RSLVec3f3 Cs; //Surface color [color]
    RSLVec3f3 Ci; //Incident ray color [color]
    RSLVec3f3 Os; //Surface opacity [color]
    RSLVec3f3 Oi; //Incident ray opacity [color]
    RSLVec3f3 P;  //Surface position [point]
    RSLVec3f3 N;  //Surface shading normal [normal]
    RSLVec3f3 Ng; //Surface geometric normal [normal]
    RSLVec3f3 E;  //Position of the eye (uniform) [point]
    RSLVec3f3 I;  //Incident ray direction [vector]

    RSLVec3f3 dPdu; //Derivative of surface position along u [vector]
    RSLVec3f3 dPdv; //Derivative of surface position along v [vector]
    float_d u, v;    //Surface parameters
    float_d du, dv;  //Change in surface parameters
    float_d s, t;    //Surface texture coordinates
    float_d ncomps;  //Number of color components (uniform)
    float_d time;    //Current shutter time (uniform)
    float_d dtime;   //The amount of time covered by this shading sample. (uniform)
    RSLVec3f3 dPdtime; //How the surface position P is changing per unit time,
	                //as described by motion blur in the scene. [vector]

    //Available only inside illuminance statements:
    //rslVec3f* L;  //Incoming light ray direction [vector]
    //rslVec3f* Cl; //Incoming light ray color [vector]
    //rslVec3f* Ol; //Incoming light ray opacity [vector]

    unsigned depth;
};
#else
struct RSLContext
{
    void* rtContext;
    void* textureTable; //pointer to texture table, each index corresponds to one RTfact texture pointer

    RSLVec3f Cs; //Surface color [color]
    RSLVec3f Ci; //Incident ray color [color]
    RSLVec3f Os; //Surface opacity [color]
    RSLVec3f Oi; //Incident ray opacity [color]
    RSLVec3f P;  //Surface position [point]
    RSLVec3f N;  //Surface shading normal [normal]
    RSLVec3f Ng; //Surface geometric normal [normal]
    RSLVec3f E;  //Position of the eye (uniform) [point]
    RSLVec3f I;  //Incident ray direction [vector]

	RSLVec3f dPdu; //Derivative of surface position along u [vector]
	RSLVec3f dPdv; //Derivative of surface position along v [vector]
	float u, v;    //Surface parameters
	float du, dv;  //Change in surface parameters
	float s, t;    //Surface texture coordinates
	float ncomps;  //Number of color components (uniform)
	float time;    //Current shutter time (uniform)
	float dtime;   //The amount of time covered by this shading sample. (uniform)
	RSLVec3f dPdtime; //How the surface position P is changing per unit time,
	                //as described by motion blur in the scene. [vector]

	//Available only inside illuminance statements:
	//rslVec3f* L;  //Incoming light ray direction [vector]
	//rslVec3f* Cl; //Incoming light ray color [vector]
	//rslVec3f* Ol; //Incoming light ray opacity [vector]

	unsigned depth;
};
#endif

//----------------------------------------------------------------------------//
// internal API function implementation (BC side)
// trace/ambient/illuminance should be inlined into the shader during
// instantiation, only calls to external RTFACT_* functions should remain
// NOTE: we do not need packetized versions, these here are inlined before!
//----------------------------------------------------------------------------//
#ifdef USE_DERIVATIVES
extern "C" void ar_trace(
        const void* __restrict rslContext,
        void* __restrict res,
        const void* __restrict P,
        const void* __restrict R
)
{
    const RSLContext* __restrict ctx = (const RSLContext* __restrict)rslContext;
    const RSLVec3f3* __restrict pos = (const RSLVec3f3* __restrict)P;
    const RSLVec3f3* __restrict dir = (const RSLVec3f3* __restrict)R;
    RSLVec3f3* __restrict result = (RSLVec3f3* __restrict)res;

	float resX, resY, resZ;
	RTFACT_trace_1(ctx->rtContext, pos->x, pos->y, pos->z, dir->x, dir->y, dir->z, ctx->depth, &resX, &resY, &resZ);
	result->x = resX; result->xu = 0.f; result->xv = 0.f;
	result->y = resY; result->yu = 0.f; result->yv = 0.f;
	result->z = resZ; result->zu = 0.f; result->zv = 0.f;
//	RTFACT_trace_1(ctx->rtContext, pos->x, pos->y, pos->z, dir->x, dir->y, dir->z, ctx->depth, &result->x, &result->y, &result->z);
}
extern "C" void ar_ambient(
        void* __restrict rslContext,
        void* __restrict res
)
{
	RSLVec3f3* __restrict result = (RSLVec3f3* __restrict)res;
	result->x = .2f; result->xu = 0.f; result->xv = 0.f;
	result->y = .2f; result->yu = 0.f; result->yv = 0.f;
	result->z = .2f; result->zu = 0.f; result->zv = 0.f;
}
extern "C" void ar_illuminance2(
        const void* __restrict rslContext,
        const void* __restrict bodyParams,
        const void (*body)(const void* __restrict, const void* __restrict, const void* __restrict, const void* __restrict, const void* __restrict),
        const void* __restrict P,
        const void* __restrict axis,
        const void* __restrict angle
)
{
	const RSLVec3f3* __restrict p = (const RSLVec3f3* __restrict)P;
	const RSLVec3f3 oAxis = *(const RSLVec3f3* __restrict)axis;
	const float_d oAngle = *(const float_d*)angle;

	const RSLContext* __restrict ctx = (const RSLContext* __restrict)rslContext;
	const unsigned numLights = RTFACT_getNumLight(ctx->rtContext);

	for (unsigned light = 0u ; light < numLights; ++light)
	{
		//
		// get properties of current light source
		//
		float dirX, dirY, dirZ;
		float oDistance;
		float radX, radY, radZ;
		float posX, posY, posZ;
		RTFACT_sampleLight_1(ctx->rtContext, light, p->x, p->y, p->z, &dirX, &dirY, &dirZ, &oDistance, &radX, &radY, &radZ, &posX, &posY, &posZ);

		//normalize
		const float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
		//oDirection *= 1.f/len;
		dirX = dirX * 1.f/len;
		dirY = dirY * 1.f/len;
		dirZ = dirZ * 1.f/len;

		//
		// check if angle of incident light direction to surface normal is outside given angle
		//
		const float LdotAxis = (oAxis.x * dirX + oAxis.y * dirY + oAxis.z  * dirZ);
		if (LdotAxis > oAngle.x) continue;

		//
		// check if light source is occluded
		// NOTE: oDirection still has to be negated by external function!
		//
		unsigned contributes;
		RTFACT_getLC_1(
			ctx->rtContext,
			posX, posY, posZ,
			dirX, dirY, dirZ,
			oDistance, &contributes, &radX, &radY, &radZ);
		if (!contributes) continue;

		//
		// execute body of illuminance statement
		//
		typedef const void (*IlluminanceBody)(const void* __restrict rslContext, const void* __restrict params, const void* __restrict L, const void* __restrict Cl, const void* __restrict Ol);
		IlluminanceBody bodyFn = (IlluminanceBody)body;
//		const void* L = (const void*)&(RSLVec3f3(dirX, dirY, dirZ));
//		const void* Cl = (const void*)&(RSLVec3f3(radX, radY, radZ));
		const RSLVec3f3 tmpL = RSLVec3f3(dirX, dirY, dirZ);
		const RSLVec3f3 tmpCl = RSLVec3f3(radX, radY, radZ);
		const void* L = (const void*)&tmpL;
		const void* Cl = (const void*)&tmpCl;
		const void* Ol = (const void*)&ctx->Oi; //1-Oi? probably ignored anyway...
		bodyFn(ctx, bodyParams, L, Cl, Ol);
	}
}
extern "C" void ar_texture(
	void* __restrict rslContext,
	void* __restrict res,
	const void* __restrict texObject,
	const void* __restrict coordX,
	const void* __restrict coordY
)
{
    unsigned int texID = *static_cast<const unsigned int *>(texObject);
	const RSLContext* __restrict ctx = (const RSLContext* __restrict)rslContext;
	const float xCoord = *(const float*)coordX;
	const float yCoord = *(const float*)coordY;
	RSLVec3f3* __restrict result = (RSLVec3f3* __restrict)res;

	float resX, resY, resZ;
	RTFACT_sampleDT_id_1(ctx->rtContext, ctx->textureTable, texID, xCoord, yCoord, 0.f, 0.f, &resX, &resY, &resZ);
	result->x = resX; result->xu = 0.f; result->xv = 0.f;
	result->y = resY; result->yu = 0.f; result->yv = 0.f;
	result->z = resZ; result->zu = 0.f; result->zv = 0.f;
}
#else
extern "C" void ar_trace(
        const void* __restrict rslContext,
        void* __restrict res,
        const void* __restrict P,
        const void* __restrict R
)
{
    const RSLContext* __restrict ctx = (const RSLContext* __restrict)rslContext;
    const RSLVec3f* __restrict pos = (const RSLVec3f* __restrict)P;
    const RSLVec3f* __restrict dir = (const RSLVec3f* __restrict)R;
    RSLVec3f* __restrict result = (RSLVec3f* __restrict)res;

	float resX, resY, resZ;
	RTFACT_trace_1(ctx->rtContext, pos->x, pos->y, pos->z, dir->x, dir->y, dir->z, ctx->depth, &resX, &resY, &resZ);
	result->x = resX;
	result->y = resY;
	result->z = resZ;
//	RTFACT_trace_1(ctx->rtContext, pos->x, pos->y, pos->z, dir->x, dir->y, dir->z, ctx->depth, &result->x, &result->y, &result->z);
}
extern "C" void ar_ambient(
        void* __restrict rslContext,
        void* __restrict res
)
{
	RSLVec3f* __restrict result = (RSLVec3f* __restrict)res;
	result->x = .2f;
	result->y = .2f;
	result->z = .2f;
}
extern "C" void ar_illuminance2(
        const void* __restrict rslContext,
        const void* __restrict bodyParams,
        const void (*body)(const void* __restrict, const void* __restrict, const void* __restrict, const void* __restrict, const void* __restrict),
        const void* __restrict P,
        const void* __restrict axis,
        const void* __restrict angle
)
{
	const RSLVec3f* __restrict p = (const RSLVec3f* __restrict)P;
	const RSLVec3f oAxis = *(const RSLVec3f* __restrict)axis;
	const float oAngle = *(const float*)angle;

	const RSLContext* __restrict ctx = (const RSLContext* __restrict)rslContext;
	const unsigned numLights = RTFACT_getNumLight(ctx->rtContext);

	for (unsigned light = 0u ; light < numLights; ++light)
	{
		//
		// get properties of current light source
		//
		float dirX, dirY, dirZ;
		float oDistance;
		float radX, radY, radZ;
		float posX, posY, posZ;
		RTFACT_sampleLight_1(ctx->rtContext, light, p->x, p->y, p->z, &dirX, &dirY, &dirZ, &oDistance, &radX, &radY, &radZ, &posX, &posY, &posZ);

		//normalize
		const float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
		//oDirection *= 1.f/len;
		dirX = dirX * 1.f/len;
		dirY = dirY * 1.f/len;
		dirZ = dirZ * 1.f/len;

		//
		// check if angle of incident light direction to surface normal is outside given angle
		//
		const float LdotAxis = (oAxis.x * dirX + oAxis.y * dirY + oAxis.z  * dirZ);
		if (LdotAxis > oAngle) continue;

		//
		// check if light source is occluded
		// NOTE: oDirection still has to be negated by external function!
		//
		unsigned contributes;
		RTFACT_getLC_1(
			ctx->rtContext,
			posX, posY, posZ,
			dirX, dirY, dirZ,
			oDistance, &contributes, &radX, &radY, &radZ);
		if (!contributes) continue;

		//
		// execute body of illuminance statement
		//
		typedef const void (*IlluminanceBody)(const void* __restrict rslContext, const void* __restrict params, const void* __restrict L, const void* __restrict Cl, const void* __restrict Ol);
		IlluminanceBody bodyFn = (IlluminanceBody)body;
//		const void* L = (const void*)&(RSLVec3f(dirX, dirY, dirZ));
//		const void* Cl = (const void*)&(RSLVec3f(radX, radY, radZ));
		const RSLVec3f tmpL = RSLVec3f(dirX, dirY, dirZ);
		const RSLVec3f tmpCl = RSLVec3f(radX, radY, radZ);
		const void* L = (const void*)&tmpL;
		const void* Cl = (const void*)&tmpCl;
		const void* Ol = (const void*)&ctx->Oi; //1-Oi? probably ignored anyway...
		bodyFn(ctx, bodyParams, L, Cl, Ol);
	}
}
extern "C" void ar_texture(
	void* __restrict rslContext,
	void* __restrict res,
	const void* __restrict texObject,
	const void* __restrict coordX,
	const void* __restrict coordY
)
{
    unsigned int texID = *static_cast<const unsigned int*>(texObject);
	const RSLContext* __restrict ctx = (const RSLContext* __restrict)rslContext;
	const float xCoord = *(const float*)coordX;
	const float yCoord = *(const float*)coordY;
	RSLVec3f* __restrict result = (RSLVec3f* __restrict)res;

	float resX, resY, resZ;
	RTFACT_sampleDT_id_1(ctx->rtContext, ctx->textureTable, texID, xCoord, yCoord, 0.f, 0.f, &resX, &resY, &resZ);
	result->x = resX;
	result->y = resY;
	result->z = resZ;
}
#endif

//predef accessor functions
//NOTE: this code should completely disappear in the final shader code
//      due to inlining and optimizations (RSLContext struct is allocated in shader).
#define PREDEF_ACCESSOR_FN(opName)                    \
extern "C" void* ar_predef_##opName(const void* __restrict rslContext) \
{                                                     \
    return &((RSLContext*)rslContext)->opName;        \
}

PREDEF_ACCESSOR_FN(Cs)
PREDEF_ACCESSOR_FN(Ci)
PREDEF_ACCESSOR_FN(Os)
PREDEF_ACCESSOR_FN(Oi)
PREDEF_ACCESSOR_FN(P)
PREDEF_ACCESSOR_FN(N)
PREDEF_ACCESSOR_FN(Ng)
PREDEF_ACCESSOR_FN(E)
PREDEF_ACCESSOR_FN(I)

PREDEF_ACCESSOR_FN(dPdu)
PREDEF_ACCESSOR_FN(dPdv)
PREDEF_ACCESSOR_FN(u)
PREDEF_ACCESSOR_FN(v)
PREDEF_ACCESSOR_FN(du)
PREDEF_ACCESSOR_FN(dv)
PREDEF_ACCESSOR_FN(s)
PREDEF_ACCESSOR_FN(t)
PREDEF_ACCESSOR_FN(ncomps)
PREDEF_ACCESSOR_FN(time)
PREDEF_ACCESSOR_FN(dtime)
PREDEF_ACCESSOR_FN(dPdtime)

#undef PREDEF_ACCESSOR_FN



//----------------------------------------------------------------------------//
// RSL shade function (resolved by linking against specific shader)
//----------------------------------------------------------------------------//
extern "C" void shade(void* __restrict rslContext, const void* __restrict params);

//----------------------------------------------------------------------------//
// internal shade wrapper
//----------------------------------------------------------------------------//
// This function is required in order to prevent context from being pulled out of
// the RayPacket-loop (thus the noinline attribute, inlining is forced during
// our own linking process).
// This is also the base function for packetization (after shade() is inlined).
// NOTE: context should entirely go away with optimizations
// NOTE: the mask is required here in order to let packetized function profit
#ifdef USE_DERIVATIVES
extern "C" void __attribute__((noinline)) inner_shade(
	void* __restrict aContext,
	const void* __restrict aParams,
	void* __restrict aTextureTable,
	const unsigned mask,
	const float posX,
	const float posY,
	const float posZ,
	const float originX,
	const float originY,
	const float originZ,
	const float dirX,
	const float dirY,
	const float dirZ,
	const float normalX,
	const float normalY,
	const float normalZ,
	const float geomNormalX,
	const float geomNormalY,
	const float geomNormalZ,
	const float vertColorX,
	const float vertColorY,
	const float vertColorZ,
	const float dPduX,
	const float dPduY,
	const float dPduZ,
	const float dPdvX,
	const float dPdvY,
	const float dPdvZ,
	const float u,
	const float v,
	const float du,
	const float dv,
	const float texCoord1,
	const float texCoord2,
	const float dT1du,
	const float dT1dv,
	const float dT2du,
	const float dT2dv,
	const unsigned depth,
	const float time,
//	float* __restrict opacityX,
//	float* __restrict opacityY,
//	float* __restrict opacityZ,
	float* __restrict resX,
	float* __restrict resY,
	float* __restrict resZ
) {
	if (!mask) return;

	//insert predef values into context
	RSLContext ctx;
	ctx.rtContext = aContext;
	ctx.textureTable = aTextureTable;
	ctx.depth = depth;
#if 1
    ctx.Os.x = 1.f; //Surface opacity, default=1  //TODO: can remove initialization?
    ctx.Os.xu = 0.f;
    ctx.Os.xv = 0.f;
	ctx.Os.y = 1.f;
    ctx.Os.yu = 0.f;
    ctx.Os.yv = 0.f;
	ctx.Os.z = 1.f;
    ctx.Os.zu = 0.f;
    ctx.Os.zv = 0.f;
//	ctx.Ci.x = 0.f;          //Incident Ray color //initialization not required
//	ctx.Ci.y = 0.f;
//	ctx.Ci.z = 0.f;
	//TODO: shouldn't all these have shadow values set to 0?
	ctx.Cs.x = vertColorX;  //Surface color
	ctx.Cs.xu = 0.f;  //Surface color
	ctx.Cs.xv = 0.f;  //Surface color
	ctx.Cs.y = vertColorY;
	ctx.Cs.yu = 0.f;
	ctx.Cs.yv = 0.f;
	ctx.Cs.z = vertColorZ;
	ctx.Cs.zu = 0.f;
	ctx.Cs.zv = 0.f;
	ctx.P.x = posX;         //Surface position
	ctx.P.xu = dPduX;         //Surface position
	ctx.P.xv = dPdvX;         //Surface position
	ctx.P.y = posY;
	ctx.P.yu = dPduY;
	ctx.P.yv = dPdvY;
	ctx.P.z = posZ;
	ctx.P.zu = dPduZ;
	ctx.P.zv = dPdvZ;
	ctx.N.x = normalX;      //Surface shading normal
	ctx.N.xu = 0.f;      //Surface shading normal
	ctx.N.xv = 0.f;      //Surface shading normal
	ctx.N.y = normalY;
	ctx.N.yu = 0.f;
	ctx.N.yv = 0.f;
	ctx.N.z = normalZ;
	ctx.N.zu = 0.f;
	ctx.N.zv = 0.f;
	ctx.Ng.x = geomNormalX; //Surface geometric normal
	ctx.Ng.xu = 0.f; //Surface geometric normal
	ctx.Ng.xv = 0.f; //Surface geometric normal
	ctx.Ng.y = geomNormalY;
	ctx.Ng.yu = 0.f;
	ctx.Ng.yv = 0.f;
	ctx.Ng.z = geomNormalZ;
	ctx.Ng.zu = 0.f;
	ctx.Ng.zv = 0.f;
	ctx.E.x = originX;      //Position of the eye
	ctx.E.xu = 0.f;      //Position of the eye
	ctx.E.xv = 0.f;      //Position of the eye
	ctx.E.y = originY;
	ctx.E.yu = 0.f;
	ctx.E.yv = 0.f;
	ctx.E.z = originZ;
	ctx.E.zu = 0.f;
	ctx.E.zv = 0.f;
	ctx.I.x = dirX;         //Incident ray direction
	ctx.I.xu = 0.f;         //Incident ray direction
	ctx.I.xv = 0.f;         //Incident ray direction
	ctx.I.y = dirY;
	ctx.I.yu = 0.f;
	ctx.I.yv = 0.f;
	ctx.I.z = dirZ;
	ctx.I.zu = 0.f;
	ctx.I.zv = 0.f;
	ctx.Oi.x = *opacityX;          //Incident ray opacity, default=1
	ctx.Oi.xu = 0.f;          //Incident ray opacity, default=1
	ctx.Oi.xv = 0.f;          //Incident ray opacity, default=1
	ctx.Oi.y = *opacityY;
	ctx.Oi.yu = 0.f;
	ctx.Oi.yv = 0.f;
	ctx.Oi.z = *opacityZ;
	ctx.Oi.zu = 0.f;
	ctx.Oi.zv = 0.f;

	//TODO: should dPdu.x.u, dPdu.x.v, dPdv.x.u etc. be 0 (all shadow values)?
	ctx.dPdu.x = dPduX; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.xu = 0.f; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.xv = 0.f; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.y = dPduY; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.yu = 0.f; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.yv = 0.f; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.z = dPduZ; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.zu = 0.f; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.zv = 0.f; //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdv.x = dPdvX; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.xu = 0.f; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.xv = 0.f; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.y = dPdvY; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.yu = 0.f; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.yv = 0.f; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.z = dPdvZ; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.zu = 0.f; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.zv = 0.f; //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.u = float_d(u, u, u); //Surface parameters
	ctx.v = float_d(v, v, v);
	ctx.du = float_d(du); //Change in surface parameters
	ctx.dv = float_d(dv);
	ctx.s = float_d(texCoord1, dT1du, dT1dv); //texture derivatives? //Surface texture coordinates
	ctx.t = float_d(texCoord2, dT2du, dT2dv); //texture derivatives?

	ctx.time = float_d(time);

	shade((void*)&ctx, aParams);

	*opacityX = ctx.Oi.x;
	*opacityY = ctx.Oi.y;
	*opacityZ = ctx.Oi.z;
	*resX = ctx.Ci.x;
	*resY = ctx.Ci.y;
	*resZ = ctx.Ci.z;
//    printf("xxxxxxxxx inner shade end!\n");
#else
	ctx.Os.x = float_d(1.f); //Surface opacity, default=1  //TODO: can remove initialization?
	ctx.Os.y = float_d(1.f);
	ctx.Os.z = float_d(1.f);
//	ctx.Ci.x = float_d(0.f);          //Incident Ray color //initialization not required
//	ctx.Ci.y = float_d(0.f);
//	ctx.Ci.z = float_d(0.f);
	//TODO: shouldn't all these have shadow values set to 0?
	ctx.Cs.x = float_d(vertColorX, vertColorX, vertColorX);  //Surface color
	ctx.Cs.y = float_d(vertColorY, vertColorY, vertColorY);
	ctx.Cs.z = float_d(vertColorZ, vertColorZ, vertColorZ);
	ctx.P.x = float_d(posX, dPduX, dPdvX);         //Surface position
	ctx.P.y = float_d(posY, dPduY, dPdvY);
	ctx.P.z = float_d(posZ, dPduZ, dPdvZ);
	ctx.N.x = float_d(normalX, normalX, normalX);      //Surface shading normal
	ctx.N.y = float_d(normalY, normalY, normalY);
	ctx.N.z = float_d(normalZ, normalZ, normalZ);
	ctx.Ng.x = float_d(geomNormalX, geomNormalX, geomNormalX); //Surface geometric normal
	ctx.Ng.y = float_d(geomNormalY, geomNormalY, geomNormalY);
	ctx.Ng.z = float_d(geomNormalZ, geomNormalZ, geomNormalZ);
	ctx.E.x = float_d(originX, originX, originX);      //Position of the eye
	ctx.E.y = float_d(originY, originY, originY);
	ctx.E.z = float_d(originZ, originZ, originZ);
	ctx.I.x = float_d(dirX, dirX, dirX);         //Incident ray direction
	ctx.I.y = float_d(dirY, dirY, dirY);
	ctx.I.z = float_d(dirZ, dirZ, dirZ);
	ctx.Oi.x = float_d(*opacityX);          //Incident ray opacity, default=1
	ctx.Oi.y = float_d(*opacityY);
	ctx.Oi.z = float_d(*opacityZ);

	//TODO: should dPdu.x.u, dPdu.x.v, dPdv.x.u etc. be 0 (all shadow values)?
	ctx.dPdu.x = float_d(dPduX); //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.y = float_d(dPduY); //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdu.z = float_d(dPduZ); //should link to Du(P) //Derivative of surface position along u [vector]
	ctx.dPdv.x = float_d(dPdvX); //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.y = float_d(dPdvY); //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.dPdv.z = float_d(dPdvZ); //should link to Dv(P) //Derivative of surface position along u [vector]
	ctx.u = float_d(u, u, u); //Surface parameters
	ctx.v = float_d(v, v, v);
	ctx.du = float_d(du); //Change in surface parameters
	ctx.dv = float_d(dv);
	ctx.s = float_d(texCoord1, dT1du, dT1dv); //texture derivatives? //Surface texture coordinates
	ctx.t = float_d(texCoord2, dT2du, dT2dv); //texture derivatives?

	ctx.time = float_d(time);

	shade((void*)&ctx, aParams);

	*opacityX = ctx.Oi.x.x;
	*opacityY = ctx.Oi.y.x;
	*opacityZ = ctx.Oi.z.x;
	*resX = ctx.Ci.x.x;
	*resY = ctx.Ci.y.x;
	*resZ = ctx.Ci.z.x;
#endif
};
#else
extern "C" void __attribute__((noinline)) inner_shade(
	void* __restrict aContext,
	const void* __restrict aParams,
	void* __restrict aTextureTable,
	const unsigned mask,
	const float posX,
	const float posY,
	const float posZ,
	const float originX,
	const float originY,
	const float originZ,
	const float dirX,
	const float dirY,
	const float dirZ,
	const float normalX,
	const float normalY,
	const float normalZ,
	const float geomNormalX,
	const float geomNormalY,
	const float geomNormalZ,
	const float vertColorX,
	const float vertColorY,
	const float vertColorZ,
	const float u,
	const float v,
	const float texCoord1,
	const float texCoord2,
	const unsigned depth,
	const float time,
//	float* __restrict opacityX,
//	float* __restrict opacityY,
//	float* __restrict opacityZ,
	float* __restrict resX,
	float* __restrict resY,
	float* __restrict resZ
) {
	if (!mask) return;

	//insert predef values into context
	RSLContext ctx;
	ctx.rtContext = aContext;
	ctx.textureTable = aTextureTable;
	ctx.depth = depth;
	ctx.Os.x = 1.f; //Surface opacity, default=1  //TODO: can remove initialization?
	ctx.Os.y = 1.f;
	ctx.Os.z = 1.f;
//	ctx.Ci.x = 0.f;          //Incident Ray color //initialization not required
//	ctx.Ci.y = 0.f;
//	ctx.Ci.z = 0.f;
	ctx.Cs.x = vertColorX;  //Surface color
	ctx.Cs.y = vertColorY;
	ctx.Cs.z = vertColorZ;
	ctx.P.x = posX;         //Surface position
	ctx.P.y = posY;
	ctx.P.z = posZ;
	ctx.N.x = normalX;      //Surface shading normal
	ctx.N.y = normalY;
	ctx.N.z = normalZ;
	ctx.Ng.x = geomNormalX; //Surface geometric normal
	ctx.Ng.y = geomNormalY;
	ctx.Ng.z = geomNormalZ;
	ctx.E.x = originX;      //Position of the eye
	ctx.E.y = originY;
	ctx.E.z = originZ;
	ctx.I.x = dirX;         //Incident ray direction
	ctx.I.y = dirY;
	ctx.I.z = dirZ;
	// if we want the ray tracer to be able to set some kind of ray opacity,
	// we possibly have to trade some performance by moving the default
	// initialization from here to the outer shade function which we might not
	// be able to optimized as good.
	ctx.Oi.x = 1.f; //*opacityX;          //Incident ray opacity, default=1
	ctx.Oi.y = 1.f; //*opacityY;
	ctx.Oi.z = 1.f; //*opacityZ;

	ctx.u = u;
	ctx.v = v;
	ctx.s = texCoord1;
	ctx.t = texCoord2;

	ctx.time = time;

	shade((void*)&ctx, aParams);

#ifndef ANYSL_RSL_NO_OPACITY
	//if opacity is non-zero, shoot additional ray and blend colors
	if (ctx.Oi.x < 1.f || ctx.Oi.y < 1.f || ctx.Oi.z < 1.f)
	{
		float tmpResultX, tmpResultY, tmpResultZ;

		RTFACT_trace_1(aContext, posX, posY, posZ, dirX, dirY, dirZ, depth, &tmpResultX, &tmpResultY, &tmpResultZ);

		ctx.Ci.x = ctx.Oi.x * ctx.Ci.x + (1.f - ctx.Oi.x) * tmpResultX;
		ctx.Ci.y = ctx.Oi.y * ctx.Ci.y + (1.f - ctx.Oi.y) * tmpResultY;
		ctx.Ci.z = ctx.Oi.z * ctx.Ci.z + (1.f - ctx.Oi.z) * tmpResultZ;
	}
#endif

	*resX = ctx.Ci.x;
	*resY = ctx.Ci.y;
	*resZ = ctx.Ci.z;
//	*opacityX = ctx.Oi.x;
//	*opacityY = ctx.Oi.y;
//	*opacityZ = ctx.Oi.z;

};
#endif

//extern "C" void __attribute__((noinline)) inner_shade_commonOrg() //TODO!


//----------------------------------------------------------------------------//
// generic RTfact shade function declaration
// implemented both in RSLGlue.cpp (scalar) and RSLPacketGlue.cpp (packetized)
// TODO: try to remove all RTfact types?
//----------------------------------------------------------------------------//
template<bool taCommonOrg, RTfact::uint taSize>
struct JitCall
{
	RTFACT_APPEARANCE_INLINE static void shade_wrapper(
            void * __restrict aContext,
            void * __restrict aParams,
            void * __restrict aTextureTable,
            const float time,
            const RTfact::RayPacket<taSize>* __restrict aRayPacket,
            const RTfact::ActiveMask<taSize>* __restrict aRayMask,
            const RTfact::SurfaceIntersection<taSize>* __restrict aIntersection,
            RTfact::Vec3f<taSize>* __restrict oResult);
};

//specialization for single-ray packets
template<bool taCommonOrg>
struct JitCall<taCommonOrg, 1>
{
	RTFACT_APPEARANCE_INLINE static void shade_wrapper(
            void * __restrict aContext,
            void * __restrict aParams,
            void * __restrict aTextureTable,
            const float time,
            const RTfact::RayPacket<1>* __restrict aRayPacket,
            const RTfact::ActiveMask<1>* __restrict aRayMask,
            const RTfact::SurfaceIntersection<1>* __restrict aIntersection,
            RTfact::Vec3f<1>* __restrict oResult);
};

#endif RSLGLUE_H_INCLUDED
