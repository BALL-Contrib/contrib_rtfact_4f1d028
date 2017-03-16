/*
 * ShaderUtil.hpp
 *
 *  Created on: Mar 30, 2009
 *      Author: moll
 */

#ifndef SHADERUTIL_HPP_
#define SHADERUTIL_HPP_

#include "ShaderMath.hpp"


/*
  Flip N so that it faces in the direction opposite to I,
  from the point of view of the current surface element.
*/

inline Vector FaceForward(const Vector &N, const Vector &I) {
  if ((float) Dot(N, I) > 0.0f) {
    return -N;
  }
  return N;
}

/*
   Return the reflection vector given an incident direction I
   and a normal vector N.
*/

inline Vector Reflect(const Vector &I, const Vector &N) {
  return I - 2.f * Dot(I, N) * N;
}

/*
  Return the transmitted vector given an incident direction I,
  the normal vector N and the relative index of refraction eta.
  eta is the ratio of the index of refraction in the volume containing
  the incident vector to that of the volume being entered.
  This vector is computed using Snell s law. If the returned vector has zero
  length, then there is no transmitted light because of total internal
  reflection.
*/

inline Vector Refract(const Vector &I, const Vector &N, float eta) {
  float IdotN = Dot(I, N);
  float k = 1.0f - eta * eta * (1.f - IdotN * IdotN);
  return k < 0.f ? Vector(0.f) : eta * I - (eta * IdotN + sqrtf(k)) * N;
}

/*
  Return the reflection coefficient Kr and refraction (or transmission) 
  coefficient Kt given an incident direction I, the surface normal N, 
  and the relative index of refraction eta. eta is the ratio of the index of 
  refraction in the volume containing the incident vector to that of the volume
  being entered. These coefficients are computed using the Fresnel formula. 
  Optionally, this procedure also returns the reflected (R) and
  transmitted (T) vectors. The transmitted vector is computed using Snell's 
  law.

  Source code from Pixie (LGPL) : http://www.renderpixie.com/

*/
inline void Fresnel(const Vector &I, const Vector &N, float eta,
                    float &Kr, float &Kt)
{
    const float e =   1.0f / eta;
    const float c =   -Dot(I, N); // cos theta
    const float t =   e*e+c*c-1.0f;
    const float g =   sqrtf(Max(t,0));
    const float a =   (g - c) / (g + c);
    const float b =   (c*(g+c) - 1.0f) / (c*(g-c) + 1.0f);

    Kr          =   0.5f*a*a*(1.0f + b*b);
    Kr          =   Min(Kr, 1.0f);
    Kr          =   Max(Kr, 0.0f);
    Kt          =   1.0f - Kr;
}

inline void Fresnel(const Vector &I, const Vector &N, float eta,
                    float &Kr, float &Kt, Vector &R, Vector &T)
{
    Fresnel(I, N, eta, Kr, Kt);
    R = Reflect(I,N);
    T = Refract(I,N,eta);
}

/*
  Transforms the vector from world to tangent space.
*/
inline Vector tangentToWorld(const Vector& worldVec, const Vector& tangent, const Vector& binormal)
{
    return Vector(Dot(worldVec, tangent), Dot(worldVec, binormal), Dot(worldVec, Cross(tangent, binormal)));
}

/*
  Transforms the vector from tangent to world space
*/
inline Vector worldToTangent(const Vector& tangentVec, const Vector& tangent, const Vector& binormal, const Vector& normal)
{
    Vector invT = Normalized(tangent - normal * Dot(tangent, normal));
    Vector invB = Cross(normal, invT) * ((Dot(Cross(invT, binormal), normal) >= 0.0f) ? 1.0f : -1.0f);
    Vector invN = Cross(invT, invB);
    return Vector(Dot(tangentVec, invT), Dot(tangentVec, invB), Dot(tangentVec, invN));
}

inline const Vector transformToWorld(
    const Vector& aVector,
    const Vector& aNormal,
    const Vector& aBinormal,
    const Vector& aTangent)
{
    return
        aVector.x * aBinormal +
        aVector.y * aTangent +
        aVector.z * aNormal;
}

inline const Vector transformToWorld(
    const Vector& aVector,
    const Vector& aNormal)
{
    Vector binormal, tangent;
    GetFrame(aNormal, binormal, tangent);

    return transformToWorld(aVector, aNormal, binormal, tangent);
}

inline float Luminance(const Color& color)
{
  return Dot(Color(0.212671f, 0.715160f, 0.072169f), color);
}

#endif /* SHADERUTIL_HPP_ */
