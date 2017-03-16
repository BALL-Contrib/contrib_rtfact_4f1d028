#ifndef MATH_HXX
#define MATH_HXX

#include <math.h>
#include <stdlib.h>
#include "commontypes.hpp"

// Portions from "The RenderMan Interface Version 3.2 July, 2000"
// RenderMan (R) is a registered trademark of Pixar.

//??? use 1E-4 if problems with floating point precision
//??? was 1E-5
#define Epsilon 1E-4
#define EPS Epsilon
#define RAY_EPS Epsilon //was set to 0.01f for many shaders
#define Infinity HUGE_VAL

#ifndef M_PI
#define M_PI           ((double)3.14159265358979323846)
#endif

#define INV_PI        0.31830988618379067154f

inline float Radians(float degrees) {
  return degrees*M_PI/180.f;
}

inline float Degrees(float radians) {
  return radians*180.f/M_PI;
}

#define Sin(x) sinf(x)
#define ASin(x) asinf(x)
#define Cos(x) cosf(x)
#define ACos(x) acosf(x)
#define Tan(x) tanf(x)

inline float ATan(float x) {
  return atanf(x);
}

inline float ATan(float y, float x) {
  return atan2f(y,x);
}

#define Pow(x,y) powf((x),(y))
#define Exp(x) expf(x)
#define Sqrt(x) sqrtf(x)

inline float InverseSqrt(float x) {
  return 1.f / Sqrt(x);
}

inline float Log(float x) {
  return logf(x);
}

inline float Log(float x, float base) {
  return Log(x) / Log(base);
}

inline float Mod(float a, float b) {
  // From Texturing & Modeling (a procedural approach) David S. Ebert
  int n = (int)(a/b);
  a -= n*b;
  if (a < 0.f)
    a += b;
  return a;
}

inline float Abs(float x) {
  return x < 0.f ? -x : x;
}

inline float Sign(float x) {
  return x < 0.f ? -1.0f : (x > 0.f ? 1.0f : 0.0f);
}

inline float Min(float a, float b) {
  return a < b ? a : b;
}

inline float Max(float a, float b) {
  return a > b ? a : b;
}

/*
  Clamp(a,min,max) returns min if a is less than min, max if a is greater than
  max; otherwise it returns a.
*/
inline float Clamp(float val, float low, float high) {
  return val < low ? low : (val > high ? high : val);
}

inline Vec2f Clamp(const Vec2f &val,const Vec2f &low, const Vec2f &high) {
  return Vec2f(Clamp(val.x, low.x, high.x),
	       Clamp(val.y, low.y, high.y));
}

inline t_Vec3f1 Clamp(const t_Vec3f1 &val,const t_Vec3f1 &low, const t_Vec3f1 &high) {
  return t_Vec3f1(Clamp(val.x, low.x, high.x),
	       Clamp(val.y, low.y, high.y),
	       Clamp(val.z, low.z, high.z));
}

inline float Mix(float x, float y, float alpha) {
  return x*(1.0f - alpha) + y*alpha;
}

inline Vec2f Mix(const Vec2f &x, const Vec2f &y, float alpha) {
  return x*(1.0f - alpha) + y*alpha;
}

inline t_Vec3f1 Mix(const t_Vec3f1 &x, const t_Vec3f1 &y, float alpha) {
  return x*(1.0f - alpha) + y*alpha;
}

#define Lerp(x,y,alpha) Mix((x),(y),(alpha))

inline float Floor(float x) {
  return floorf(x);
}

inline float Ceil(float x) {
  return ceilf(x);
}

inline float Round(float x) {
  double v;
  return modf(x, &v) > 0.5f ? v + 1.0f : v;
}

inline float Step(float min, float value) {
  return value < min ? 0.0f : 1.0f;
}

/*
  SmoothStep returns 0 if value is less than min, 1 if value is greater than
  or equal to max, and performs a smooth Hermite interpolation between
  0 and 1 in the interval min to max.
*/
inline float SmoothStep(float min, float max, float value) {
  if (value < min) return 0.0f;
  if (value >= max) return 1.0f;

  float v = (value - min) / (max - min);
  return (-2.0f * v + 3.0f) * v * v;
}

#define PLUSE(a,b,x) (Step((a),(x)) - Step((b),(x)))

// One-dimensional Catmull-Rom interpolating spline
float Spline(float x, int nknots, float *knot);

// Generates a uniform random float in [0,1)
inline float Random() {
  const float INV_RAND_MAX_PLUS_1 = 1.0 / (RAND_MAX + 1);
  return ::rand() * INV_RAND_MAX_PLUS_1;
}

inline Vec2f RandomVec2f() {
  return Vec2f(Random(), Random());
}

inline t_Vec3f1 RandomVec3f() {
  return t_Vec3f1(Random(), Random(), Random());
}

/* Coefficients of basis matrix. */
#define CR00     -0.5f
#define CR01      1.5f
#define CR02     -1.5f
#define CR03      0.5f
#define CR10      1.0f
#define CR11     -2.5f
#define CR12      2.0f
#define CR13     -0.5f
#define CR20     -0.5f
#define CR21      0.0f
#define CR22      0.5f
#define CR23      0.0f
#define CR30      0.0f
#define CR31      1.0f
#define CR32      0.0f
#define CR33      0.0f

inline float Spline(float x, int nknots, float *knot) {
  int span;
  int nspans = nknots - 3;
  float c0, c1, c2, c3;	/* coefficients of the cubic.*/

  if (nspans < 1) {  /* illegal */
    return 0.f;
  }

  /* Find the appropriate 4-point span of the spline. */
  x = Clamp(x, 0.f, 1.f) * nspans;
  span = (int) x;
  if (span >= nknots - 3)
    span = nknots - 3;
  x -= span;
  knot += span;

  /* Evaluate the span cubic at x using Horner's rule. */
  c3 = CR00*knot[0] + CR01*knot[1]
    + CR02*knot[2] + CR03*knot[3];
  c2 = CR10*knot[0] + CR11*knot[1]
    + CR12*knot[2] + CR13*knot[3];
  c1 = CR20*knot[0] + CR21*knot[1]
    + CR22*knot[2] + CR23*knot[3];
  c0 = CR30*knot[0] + CR31*knot[1]
    + CR32*knot[2] + CR33*knot[3];

  return ((c3*x + c2)*x + c1)*x + c0;
}

/*inline float Max(float a, float b)
{
	return a > b ? a : b;
}*/

inline t_Vec3f1 Max(const t_Vec3f1 & left, const t_Vec3f1 & right)
{
	return t_Vec3f1(Max(left.x, right.x), Max(left.y, right.y), Max(left.z, right.z));
}

inline t_Vec3f1 Min(const t_Vec3f1 & left, const t_Vec3f1 & right)
{
	return t_Vec3f1(Min(left.x, right.x), Min(left.y, right.y), Min(left.z, right.z));
}

#endif
