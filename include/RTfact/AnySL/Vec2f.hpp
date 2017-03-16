#ifndef VEC2F_HXX
#define VEC2F_HXX

#include <math.h>
#include <iostream>

#include "Vec3f.hpp"

namespace Shader {

typedef enum { XY, XZ, YZ } Plane;

class Vec2f {
public:
  float x,y;

  Vec2f() : x(0.0), y(0.0) {};
  Vec2f(float x,float y) : x(x),y(y) {};

  Vec2f(const Vec3f &v, Plane plane) {
    Set(v, plane);
  }

  inline void Set(float x, float y) {
    Vec2f::x = x;
    Vec2f::y = y;
  }

  inline void Set(const Vec3f &v, Plane plane) {
    switch (plane) {
    case XY:
      x = v.x; y = v.y;
      break;
    case XZ:
      x = v.x; y = v.z;
      break;
    case YZ:
      x = v.y; y = v.z;
      break;
    }
  }

  //! Array operator: just for convenience: @f$ a[0]==a.x, a[1]==a.y @f$
  /*! be careful : a[0] is often slower than accessing a.x !
   * Also assert that @f$  0 <= i < 3 @f$
   */
  inline const float &operator[](const int i) const
  { return *(&x+i); };

  //! Array operator: just for convenience: @f$  a[0]==a.x, a[1]==a.y @f$ ...
  /*! be careful : a[0] is often slower than accessing a.x !
   * Also assert that @f$  0 <= i < 3 @f$
   */
  inline float &operator[](const int i)
  { return *(&x+i); };

  inline Vec2f &operator=(const Vec2f &b) {
    x = b.x; y = b.y;
    return *this;
  };

};

/*** Multiplications ***/

  /*! multiply vector by float */
inline Vec2f operator*(const float f, const Vec2f &v) {
  return Vec2f(f*v.x, f*v.y);
};

  /*! multiply vector by float */
inline Vec2f operator*(const Vec2f &v, const float f) {
  return Vec2f(f*v.x, f*v.y);
};

  /*! still undocumented */
inline void operator*=(Vec2f &v, const float f) {
  v.x *= f; v.y*=f;
};

  /*! still undocumented */
inline void operator*=(Vec2f &v, const Vec2f &f) {
  v.x *= f.x; v.y *= f.y;
};

  /*! still undocumented */
inline Vec2f operator*(const Vec2f &a, const Vec2f &b) {
  return Vec2f(a.x*b.x, a.y*b.y);
};

/*** Divisions ***/

  /*! still undocumented */
inline Vec2f operator/(const Vec2f &v, const float f) {
  return (1.0/f)*v;
};

  /*! still undocumented */
inline void operator/=(Vec2f &v, const float f) {
  v *= (1.0/f);
};

/*** Additions ***/

  /*! still undocumented */
inline Vec2f operator+(const float f, const Vec2f &v) {
  return Vec2f(f+v.x, f+v.y);
};

  /*! still undocumented */
inline Vec2f operator+(const Vec2f &v, const float f) {
  return Vec2f(f+v.x, f+v.y);
};

  /*! still undocumented */
inline Vec2f operator+(const Vec2f &a, const Vec2f &b) {
  return Vec2f(a.x+b.x, a.y+b.y);
};

inline void operator+=(Vec2f &v, const Vec2f &f) {
  v.x += f.x; v.y += f.y;
};

inline void operator+=(Vec2f &v, const float f) {
  v.x += f; v.y += f;
};

/*** Substractions ***/

  /*! still undocumented */
inline Vec2f operator-(const Vec2f &a, const Vec2f &b) {
  return Vec2f(a.x-b.x, a.y-b.y);
};

/*** Operations ***/

  /*! dot product */
inline float Dot(const Vec2f &a, const Vec2f &b) {
  return a.x*b.x+a.y*b.y;
};

  /*! component-wise product */
inline Vec2f Product(const Vec2f &a, const Vec2f &b) {
  return Vec2f(a.x*b.x, a.y*b.y);
};

inline Vec2f operator-(const Vec2f &v) {
  return Vec2f(-v.x,-v.y);
};

inline float Length(const Vec2f &v) {
  return sqrtf(Dot(v,v));
};

inline void Normalize(Vec2f &v) {
  v *= (1./Length(v));
};

inline Vec2f Normalized(const Vec2f &v) {
  Vec2f nv(v);
  Normalize(nv);
  return nv;
};

/* Determinant of matrix :
   | a.x  b.x |
   |          |
   | a.y  b.y |
*/
inline float Determinant(const Vec2f &a, const Vec2f &b) {
  return a.x*b.y - b.x*a.y;
}


inline std::ostream &operator<<(std::ostream &o,const Vec2f &v) {
  o << "(" << v.x << "," << v.y << ")";
  return o;
}

}

#endif
