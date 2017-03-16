#include "commontypes.hpp"

/// Ken Perlin's "improved noise" from 2002
/// converted to float to support packetization

static inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
static inline float lerp(float t, float a, float b) { return a + t * (b - a); }
static inline float grad(int hash, float x, float y, float z) {
    int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
    float u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
            v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}
static inline float NoiseOrig(float x, float y, float z) {
    static int p[] = { 151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
        151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    int X = (int)floorf(x) & 255,                  // FIND UNIT CUBE THAT
        Y = (int)floorf(y) & 255,                  // CONTAINS POINT.
        Z = (int)floorf(z) & 255;
    x -= floorf(x);                                // FIND RELATIVE X,Y,Z
    y -= floorf(y);                                // OF POINT IN CUBE.
    z -= floorf(z);
    float u = fade(x),                                // COMPUTE FADE CURVES
            v = fade(y),                                // FOR EACH OF X,Y,Z.
            w = fade(z);
    int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
        B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,

    return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                    grad(p[BA  ], x-1, y  , z   )), // BLENDED
                            lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                                    grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                    lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
                                    grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
                            lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                    grad(p[BB+1], x-1, y-1, z-1 ))));
}



/// implementation that uses the random number generator function
/// NOTE: this version has visible artifacts!

static inline int randfunc(int x) {
    x &= 0xff; // should remove artifacts
    x = (x << 13) ^ x;
    x = x * 15731 + 789221;
    return (int) (x * 5.9138983E-8f) + 128;
}

static inline float NoiseFn(float x, float y, float z) {
    int X = (int)floorf(x) & 255,                  // FIND UNIT CUBE THAT
        Y = (int)floorf(y) & 255,                  // CONTAINS POINT.
        Z = (int)floorf(z) & 255;
    x -= floorf(x);                                // FIND RELATIVE X,Y,Z
    y -= floorf(y);                                // OF POINT IN CUBE.
    z -= floorf(z);
    float u = fade(x),                                // COMPUTE FADE CURVES
            v = fade(y),                                // FOR EACH OF X,Y,Z.
            w = fade(z);
    const int A = randfunc(X  )+Y, AA = randfunc(A)+Z, AB = randfunc(A+1)+Z;   // HASH COORDINATES OF
    const int B = randfunc(X+1)+Y, BA = randfunc(B)+Z, BB = randfunc(B+1)+Z;   // THE 8 CUBE CORNERS,

    return lerp(w, lerp(v, lerp(u, grad(randfunc(AA  ), x  , y  , z   ),  // AND ADD
                                    grad(randfunc(BA  ), x-1, y  , z   )), // BLENDED
                            lerp(u, grad(randfunc(AB  ), x  , y-1, z   ),  // RESULTS
                                    grad(randfunc(BB  ), x-1, y-1, z   ))),// FROM  8
                    lerp(v, lerp(u, grad(randfunc(AA+1), x  , y  , z-1 ),  // CORNERS
                                    grad(randfunc(BA+1), x-1, y  , z-1 )), // OF CUBE
                            lerp(u, grad(randfunc(AB+1), x  , y-1, z-1 ),
                                    grad(randfunc(BB+1), x-1, y-1, z-1 ))));
}

/*
 * Optimized version of noise
 */


static inline float int_bits_to_float(uint32_t x)
{
	union { float f; uint32_t i; } u;
	u.i = x;
	return u.f;
}

static inline uint32_t float_bits_to_int(float x)
{
	union { float f; uint32_t i; } u;
	u.f = x;
	return u.i;
}

static inline uint32_t isolate_in_31(uint32_t v, uint32_t n)
{
	return (v & (1 << n)) << (31 - n);
}

static inline float my_grad(uint32_t hash, float x, float y, float z) {

	// hash  u v
	// ---------
	// 0-3   X Y
	// 4-7   X Y
	// 8-11  X Z
	// 12-15 Y Z

	// hash bits
	// 3 mask whether Y or Z should be in v
	// 2 mask whether X or Y should be in u
	// 1 bit if u should be negated
	// 0 bit if v should be negated

	uint32_t ix = float_bits_to_int(x);
	uint32_t iy = float_bits_to_int(y);
	uint32_t iz = float_bits_to_int(z);

	uint32_t vmask = isolate_in_31(hash, 3) >> 31;
	uint32_t v = iz & vmask | iy & ~vmask;

	uint32_t umask = isolate_in_31(hash, 2) >> 31;
	uint32_t ymask = umask & vmask;
	uint32_t u = iy & ymask | ix & ~ymask;

	uint32_t uneg = isolate_in_31(hash, 1);
	uint32_t vneg = isolate_in_31(hash, 0);

	u ^= uneg;
	v ^= vneg;

	float ru = int_bits_to_float(u);
	float rv = int_bits_to_float(v);

	return ru + rv;
}

static inline float my_grad_2d(uint32_t hash, float x, float y)
{
	uint32_t u = *(uint32_t*) &x;
	uint32_t v = *(uint32_t*) &y;

	uint32_t mask = hash << 31;
	u ^= mask;
	mask = (hash >> 1) << 31;
	v ^= mask;

	float ru = *(float*) &u;
	float rv = *(float*) &v;

	return ru + rv;
}

// number of bits (counted from lsb) that really should be used from this hash
#define HASH_VALID_BITS 16

/**
 * Scramble bits in a word.
 * @param a the word.
 * @return scrambled bits of a.
 */
static inline uint32_t hash(uint32_t a)
{
	// noise needs reproducible hashes
	// based on the lower 8 bits of a
	a &= 0xff;

	a -= (a<<6);

	// original:  a ^= (a>>17);
	// since we only had 8 bits,
	// negating suffices
	a  = ~a;

	a -= (a<<9);
	a ^= (a<<4);
	a -= (a<<3);
	a ^= (a<<10);
	a ^= (a>>15);
	return a;
}

static float my_noise_3d(float x, float y, float z) {
	float fx = floorf(x);
	float fy = floorf(y);
	float fz = floorf(z);

	int X = (int) fx & 255,                  // find unit cube that
		Y = (int) fy & 255,                  // contains point.
		Z = (int) fz & 255;

	x -= fx;                                // find relative x,y,z
	y -= fy;                                // of point in cube.
	z -= fz;

	float u = fade(x),                                // compute fade curves
		  v = fade(y),                                // for each of x,y,z.
		  w = fade(z);

	uint32_t A = hash(X  )+Y, AA = hash(A)+Z, AB = hash(A+1)+Z,      // hash coordinates of
			 B = hash(X+1)+Y, BA = hash(B)+Z, BB = hash(B+1)+Z;      // the 8 cube corners,

	return lerp(w, lerp(v, lerp(u, my_grad(hash(AA  ), x  , y  , z   ),  // and add
                                   my_grad(hash(BA  ), x-1, y  , z   )), // blended
                           lerp(u, my_grad(hash(AB  ), x  , y-1, z   ),  // results
                                   my_grad(hash(BB  ), x-1, y-1, z   ))),// from  8
                   lerp(v, lerp(u, my_grad(hash(AA+1), x  , y  , z-1 ),  // corners
                                   my_grad(hash(BA+1), x-1, y  , z-1 )), // of cube
                           lerp(u, my_grad(hash(AB+1), x  , y-1, z-1 ),
                                   my_grad(hash(BB+1), x-1, y-1, z-1 ))));
}

static float my_noise_2d(float x, float y)
{
	float fx = floorf(x);
	float fy = floorf(y);

	int X = (int) fx & 255,
		Y = (int) fy & 255;

	x -= fx;
	y -= fy;

	float u = fade(x),
		  v = fade(y);

	uint32_t A = hash(X  )+Y, AA = hash(A), AB = hash(A+1),
		     B = hash(X+1)+Y, BA = hash(B), BB = hash(B+1);

	return lerp(v, lerp(u, my_grad_2d(hash(AA), x  , y  ),
                           my_grad_2d(hash(BA), x-1, y  )),
                   lerp(u, my_grad_2d(hash(AB), x  , y-1),
                           my_grad_2d(hash(BB), x-1, y-1)));
}

static inline float rand_grad(uint32_t hash)
{
	unsigned exp = 127 << 23;
	unsigned man = (hash << (32 - HASH_VALID_BITS)) >> 9;
	return int_bits_to_float(exp | man);
}

static inline float my_noise_1d(float x)
{
	float fx = floorf(x);
	int X = (int) fx & 255;

	x -= fx;

	float u = fade(x);

	uint32_t A = hash(X);
	uint32_t B = hash(X+1);

	float g0 = rand_grad(A) * x;
	float g1 = rand_grad(B) * (x - 1);

	return lerp(u, g0, g1);
}

///////////////////////
///wrapper functions //
///////////////////////

inline float Noise(float x, float y, float z)
{
	return my_noise_3d(x, y, z);
	//return NoiseOrig(x, y, z);
	//return NoiseFn(x, y, z);
}

inline float Noise(float x, float y)
{
	return my_noise_2d(x, y);
	//return NoiseOrig(x, y, 0.f);
}

inline float Noise(const t_Vec3f1 & pos)
{
	return my_noise_3d(pos.x, pos.y, pos.z);
	//return NoiseOrig(pos.x, pos.y, pos.z);
}
