/*
 *  Parameterizable.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2008-11-27 16:51:44 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein, Ken Dahm, Petr Kmoch, Lukas Marsalek, Dmitri Rubinstein
 */

 #ifndef RTFACT_REMOTE__PARAMETRIZABLE_HPP
#define RTFACT_REMOTE__PARAMETRIZABLE_HPP

#include <RTremote/Config/Common.hpp>

//#include <RTfact/Utils/Packets/Vec3f.hpp>
//#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTremote/ReferenceCounted.hpp>
#include <string>
#include <iostream>
#include <boost/variant.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class Renderer;
class GraphEntity;
class Geometry;
class Group;
class RTAppearance;
class RTLight;
class RTImage;
class Effect;
class PostprocessorParameter;
class Parameterizable;

#ifdef RTFACT_REMOTE_OS_WIN

RTFACT_REMOTE_NAMESPACE_END

RTFACT_REMOTE_NAMESPACE_BEGIN

#endif

typedef boost::intrusive_ptr<Geometry>                  GeoHandle;
typedef boost::intrusive_ptr<Group>                     GroupHandle;
typedef boost::intrusive_ptr<RTAppearance>              RTAppearanceHandle;
typedef boost::intrusive_ptr<RTImage>                   RTImageHandle;
typedef boost::intrusive_ptr<RTLight>                   RTLightHandle;
typedef boost::intrusive_ptr<Effect>                    EffectHandle;
typedef boost::intrusive_ptr<PostprocessorParameter>    PostprocessorParameterHandle;
typedef boost::intrusive_ptr<Parameterizable>           ParameterizableHandle;


struct float2
{
    union {
        struct { float x, y; };
        struct { float data[2]; };
    };

    float2() {}

    float2(
        const float aX,
        const float aY
    ) :
        x(aX),
        y(aY)
    {}

    explicit float2(
        const float* aData
    ) :
        x(aData[0]),
        y(aData[1])
    {}

    float& operator[](
        const uint aIndex)
    {
        //RTFACT_ASSERT(aIndex < 2);

        return data[aIndex];
    }

    const float& operator[](
        const uint aIndex) const
    {
        //RTFACT_ASSERT(aIndex < 2);

        return data[aIndex];
    }
};


struct float3
{
    union {
        struct { float x, y, z; };
        struct { float data[3]; };
    };

    float3() {}

    float3(
        const float aX,
        const float aY,
        const float aZ
    ) :
        x(aX),
        y(aY),
        z(aZ)
    {}

    explicit float3(
        const float* aData
    ) :
        x(aData[0]),
        y(aData[1]),
        z(aData[2])
    {}

    float& operator[](
        const uint aIndex)
    {
        //RTFACT_ASSERT(aIndex < 3);

        return data[aIndex];
    }

    const float& operator[](
        const uint aIndex) const
    {
        //RTFACT_ASSERT(aIndex < 3);

        return data[aIndex];
    }
};


struct Triangle
{
    typedef std::pair<float, float> TextCoordPair;

    float3        vertices[3];
    float3        normals[3];
    float3        normal;
    float3        vertexColors[3];
    TextCoordPair texCoords[3];

    Triangle()
    {}

    Triangle(
        const float3& aVertex1,
        const float3& aVertex2,
        const float3& aVertex3)
    {
        vertices[0] = aVertex1;
        vertices[1] = aVertex2;
        vertices[2] = aVertex3;

        vertexColors[0] = float3(1, 0, 0);
        vertexColors[1] = float3(0, 1, 0);
        vertexColors[2] = float3(0, 0, 1);

        normal = float3(0,0,0);
        normals[0] = float3(0,0,0);
        normals[1] = float3(0,0,0);
        normals[2] = float3(0,0,0);
    }
};

struct Transform
{
    float matrix[4][4];
    float invMatrix[4][4];
    bool hasInverse;

    Transform() : hasInverse(false)
    {}

    Transform(const float aM[4][4]) : hasInverse(false)
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                matrix[i][j] = aM[i][j];
    }
    Transform(const float *aM) : hasInverse(false)
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                matrix[i][j] = aM[4*i + j];
    }

    Transform(const float aM[4][4], const float aIM[4][4]) : hasInverse(true)
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
            {
                matrix[i][j] = aM[i][j];
                invMatrix[i][j] = aIM[i][j];
            }
    }
    Transform(const float *aM, const float *aIM) : hasInverse(false)
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
            {
                matrix[i][j]         = aM[4*i + j];
                invMatrix[i][j] = aIM[4*i + j];
            }
    }

    static const Transform identity(){
        static const float matID[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
        static const Transform id(matID, matID);
        return id;
    }

    static Transform translation(float aX, float aY, float aZ){
        const float mat[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            aX, aY, aZ, 1};
        const float matInv[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -aX, -aY, -aZ, 1};
        return Transform(mat, matInv);
    }
};


/** Datatype which describes all possible parameter values that can be set */
typedef boost::variant<
    int,
    float,
    float3,
    bool,
    std::string,
    RTImageHandle,
    GroupHandle,
    void*> ParamValue;

/** Type IDs of every type that can be contained in ParamValue.
*  The order of declaration must be the same because ID should be
*  always equal to ParamValue::which() (i.e. variant<T>::which())
*/
enum ParamValueType
{
    INT_VALUE      = 0,
    FLOAT_VALUE    = 1,
    FLOAT3_VALUE   = 2,
    BOOL_VALUE     = 3,
    STRING_VALUE   = 4,
    IMAGEREF_VALUE = 5,
    GROUPREF_VALUE = 6,
    VOIDPTR_VALUE  = 7
};

/** ParamInfo provides information about parameter name, type and short
*  explanation of the functionality.
*/
struct ParamInfo
{
    std::string name;
    std::string description;
    ParamValueType type;

    ParamInfo(
        const std::string &name,
        ParamValueType type,
        const std::string &description = ""
    ) :
        name(name),
        description(description),
        type(type)
    {}
};

class RTFACT_REMOTE_API Parameterizable :
    public ReferenceCounted
{
public:

    virtual void beginParams()
    {}

    virtual void endParams()
    {}

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value) = 0;

    virtual bool getParam(
        const std::string &name,
        ParamValue &value) = 0;

    virtual bool hasParam(
        const std::string &name) = 0;

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos) = 0;

    virtual ~Parameterizable()
    {}
};


RTFACT_REMOTE_NAMESPACE_END

namespace boost
{
    // TODO: There is no macro which define a RTfact namespace name and
    //       RTfact::Remote namespace name. Currently namespace name is specified
    //       directly.

    //    template <>
    // struct has_nothrow_copy< ::RTfact::Vec3f1>
    //    : mpl::true_
    // {};
    /*
    template <>
    struct has_nothrow_copy< ::RTfact::Remote::RTImageHandle>
        : mpl::true_
    {};
    */

} // namespace boost

#endif
