#ifndef VEC3F_HPP
#define VEC3F_HPP

#include <math.h>
#include <iostream>

#ifdef _RTFACT_EXTENSIONS
#include <RTfact/Utils/Packets/Vec3f.hpp>
#endif

#define DONT_VERWIRR_THE_COMPILERS

namespace Shader {

class Vec3f {
public:
  float x,y,z;

#ifdef DONT_VERWIRR_THE_COMPILERS
  Vec3f() {};
#else
  Vec3f() : x(0.0), y(0.0), z(0.0) {};
#endif


#ifdef _RTFACT_EXTENSIONS

  Vec3f(const RTfact::Vec3f<1> & src)
  {
	  x = src.xAt(0);
	  y = src.yAt(0);
	  z = src.zAt(0);
  }
#endif

  Vec3f(float c) :
	  x(c), y(c), z(c)
  {
  }

  Vec3f(float _x, float _y, float _z)
    : x(_x),y(_y),z(_z)
  {};

// disabled in order to prevent slow code

//  //! Array operator: just for convenience: @f$ a[0]==a.x, a[1]==a.y, a[2]==a.z @f$
//  /*! be careful : a[0] is often slower than accessing a.x !
//   * Also assert that @f$  0 <= i < 3 @f$
//   */
//  inline const float &operator[](const int i) const
//  { return i == 0 ? x : (i == 1 ? y : z); };
//
//  //! Array operator: just for convenience: @f$  a[0]==a.x, a[1]==a.y, a[2]==a.z @f$ ...
//  /*! be careful : a[0] is often slower than accessing a.x !
//   * Also assert that @f$  0 <= i < 3 @f$
//   */
//  inline float &operator[](const int i)
//  { return i == 0 ? x : (i == 1 ? y : z); }; //((float *)(this))[i]; };

  inline Vec3f &operator=(const Vec3f &b)
  { x = b.x; y = b.y; z = b.z; return *this;};

  inline void Set(float x, float y, float z) {
    Vec3f::x = x;
    Vec3f::y = y;
    Vec3f::z = z;
  }
};

#ifdef _RTFACT_EXTENSIONS
inline void assign(RTfact::Vec3f<1> & target, const Vec3f & src)
{
	target = RTfact::Vec3f<1>(src.x, src.y, src.z);
}
#endif

/*** Multiplications ***/

  /*! multiply vector by float */
inline Vec3f operator*(const float f, const Vec3f &v)
{ return Vec3f(f*v.x, f*v.y, f*v.z); };

  /*! multiply vector by float */
inline Vec3f operator*(const Vec3f &v, const float f)
{ return Vec3f(f*v.x, f*v.y, f*v.z); };

  /*! still undocumented */
inline void operator*=(Vec3f &v, const float f)
{ v.x *= f; v.y*=f; v.z*=f; };

  /*! still undocumented */
inline void operator*=(Vec3f &v, const Vec3f &f)
{ v.x *= f.x; v.y*=f.y; v.z*=f.z; };

  /*! still undocumented */
inline Vec3f operator*(const Vec3f &a, const Vec3f &b)
{ return Vec3f(a.x*b.x, a.y*b.y, a.z*b.z); };

/*** Divisions ***/

  /*! still undocumented */
inline Vec3f operator/(const Vec3f &v, const float f)
{ return (1.0/f)*v; };

  /*! still undocumented */
inline void operator/=(Vec3f &v, const float f)
{ v *= (1.0/f); };

/*** Additions ***/

inline Vec3f operator+(const float f, const Vec3f &v) {
  return Vec3f(f+v.x, f+v.y, f+v.z);
};

inline Vec3f operator+(const Vec3f &v, const float f) {
  return Vec3f(f+v.x, f+v.y, f+v.z);
};

inline Vec3f operator+(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x+b.x, a.y+b.y, a.z+b.z);
};

inline void operator+=(Vec3f &v, const Vec3f &f) {
  v.x += f.x; v.y += f.y; v.z += f.z;
};

inline void operator+=(Vec3f &v, const float f) {
  v.x += f; v.y += f; v.z += f;
};

/*** Substractions ***/

inline Vec3f operator-(const float f, const Vec3f &v) {
  return Vec3f(f-v.x, f-v.y, f-v.z);
};

inline Vec3f operator-(const Vec3f &v, const float f) {
  return Vec3f(v.x-f, v.y-f, v.z-f);
};

inline Vec3f operator-(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x-b.x, a.y-b.y, a.z-b.z);
};

inline void operator-=(Vec3f &v, const Vec3f &f) {
  v.x -= f.x; v.y -= f.y; v.z -= f.z;
};

inline void operator-=(Vec3f &v, const float f) {
  v.x -= f; v.y -= f; v.z -= f;
};

/*** Operations ***/

  /*! dot product */
inline float Dot(const Vec3f &a, const Vec3f &b) {
  return a.x*b.x+a.y*b.y+a.z*b.z;
};

  /*! component-wise product */
inline Vec3f Product(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x*b.x,a.y*b.y,a.z*b.z);
};

  /*! vector product */
inline Vec3f Cross(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.y*b.z-a.z*b.y,
	       a.z*b.x-a.x*b.z,
	       a.x*b.y-a.y*b.x);
};

inline Vec3f operator-(const Vec3f &v) {
  return Vec3f(-v.x,-v.y,-v.z);
};

inline float Length(const Vec3f &v) {
  return sqrtf(Dot(v,v));
};

inline void Normalize(Vec3f &v) {
  v *= (1./Length(v));
};

// Normalize and save length in len
inline void Normalize(Vec3f &v, float &len) {
  len = Length(v);
  v *= (1./len);
}

inline Vec3f Normalized(const Vec3f &v) {
  Vec3f nv(v);
  Normalize(nv);
  return nv;
};

inline Vec3f Normalized(const Vec3f &v, float &len) {
  Vec3f nv(v);
  Normalize(nv, len);
  return nv;
}

#ifdef DONT_VERWIRR_THE_COMPILERS

inline void GetFrame(const Vec3f &n, Vec3f &x, Vec3f &y) {
  Vec3f u(0);
  float n0 = n.x * n.x;
  float n1 = n.y * n.y;
  float n2 = n.z * n.z;

  if (n1 < n0) {
    if (n2 < n1) {
      u.z = 1;
    } else {
      u.y = 1;
    }
  } else if (n2 < n0) {
    u.z = 1;
  } else {
    u.x = 1;
  }

  x = Cross(u,n);
  y = Cross(n,x);

  Normalize(x);
  Normalize(y);
}

#else

// Get coordinate frame from surface normal
inline void GetFrame(const Vec3f &n, Vec3f &x, Vec3f &y) {
  Vec3f u(0);

  int k = 0;
  if (n[1]*n[1] < n[k]*n[k]) k = 1;
  if (n[2]*n[2] < n[k]*n[k]) k = 2;
  u[k] = 1;
  x = Cross(u,n);
  y = Cross(n,x);

  Normalize(x);
  Normalize(y);
}

#endif

inline std::ostream &operator<<(std::ostream &o, const Vec3f &v) {
  o << "(" << v.x << "," << v.y << "," << v.z << ")";
  return o;
}

};

#endif
