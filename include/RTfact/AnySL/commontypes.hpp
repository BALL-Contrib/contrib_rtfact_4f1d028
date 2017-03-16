/*
 * commontypes.hpp
 *
 *  Created on: Mar 31, 2009
 *      Author: moll
 */

#ifndef COMMONTYPES_HPP_
#define COMMONTYPES_HPP_

#include "Vec3f.hpp"
#include "Vec2f.hpp"
#include <xmmintrin.h>

namespace Shader {

struct RayType
{
    enum
    {
        ShadingRay = 0,
        ImportonRay = 1,
        ParticleRay = 2
    };
};

struct Float4 {
	float data[4];

	float operator()(int i) const {
		return data[i];
	}
};

struct Int4 {
	int data[4];

	int operator()(int i) const {
		return data[i];
	}
};

struct Vec3f4 {
	float data[3][4];

	Shader::Vec3f operator()(int index) const
	{
		return Shader::Vec3f(data[0][index], data[1][index], data[2][index]);
	}

	void set(int index, const Shader::Vec3f & value)
	{
		data[0][index] = value.x;
		data[1][index] = value.y;
		data[2][index] = value.z;
	}
};

namespace Packed {
struct Vec3f4 {
	Vec3f4() {}
	Vec3f4(__m128 & _x, __m128 & _y, __m128 & _z) : x(_x), y(_y), z(_z) {}
	Vec3f4(const __m128 & _x, const __m128 & _y, const __m128 & _z) : x(_x), y(_y), z(_z) {}

	__m128 x;
	__m128 y;
	__m128 z;
};
}
}


//SSE packed vector types
typedef __m128                           t_Packed_Float4;
typedef __m128i                          t_Packed_Int4;
typedef Shader::Packed::Vec3f4           t_Packed_Vec3f4;
typedef __m128i                          t_Packed_Mask4;
typedef __m128                           t_Packed_Vec4f;

//accessible data types
typedef Shader::Float4                   t_Float4;
typedef Shader::Int4                     t_Int4;
typedef Shader::Vec3f4                   t_Vec3f4;
typedef Shader::Int4                     t_Mask4;

//scalar vectorization source types
typedef Shader::Vec3f                    t_Vec3f1;
typedef Shader::Vec3f					 Point;
typedef Shader::Vec3f					 Color;
typedef Shader::Vec3f					 Vector;
typedef Shader::Vec2f 				 Vector2D;
typedef Shader::Vec2f 				 Vec2f;
typedef Shader::Vec3f					 Normal;
typedef float                            t_Float1;
typedef int                              t_Int1;
typedef int                              t_Mask1;


// We don't use void * directly in order to create a new type so
// TypeName can report correct name of the texture type.
struct TextureData { };
typedef TextureData * Texture;

#define NO_TEXTURE static_cast<TextureData*>(0)

struct ParticleData { };
typedef ParticleData * Particle;

struct LightData { };
typedef LightData * Light;


#endif /* COMMONTYPES_HPP_ */
