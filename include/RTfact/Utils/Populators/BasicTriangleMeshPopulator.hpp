/* 
 *  BasicTriangleMeshPopulator.hpp
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
 *  Created on: 2009-02-02 02:21:22 +0100
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */
 
 #ifndef RTFACT__BASICSCENECREATOR_HPP
#define RTFACT__BASICSCENECREATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>
#include <boost/tuple/tuple.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/Containers/HashMap.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>

#define RTFACT__DEFAULT_APPEARANCE_NAME                                        \
    "_default_{89751DFA-0157-4a83-B899-3AD864B98BE0}"

RTFACT_NAMESPACE_BEGIN

template<
    class tBasicTriangleMesh,
    class tAppearanceCreator>
class BasicTriangleMeshPopulator
{
    struct TriangleInfo
    {
        uint vertices[3];
        uint normals[3];
        uint texCoords[3];
        uint vertColors[3];
        uint appearanceID;
    };

    typedef typename tBasicTriangleMesh::PrimitiveBase   t_Triangle;
    typedef HashMap<std::string, uint>                   t_AppearanceMap;
    typedef std::pair<float, float>                      t_TexCoordPair;

    tBasicTriangleMesh*    mTriangleMesh;
    tAppearanceCreator     mAppearanceCreator;
    t_AppearanceMap        mAppearanceIDs;
    uint                   mActiveAppearanceID;

    Vector<Vec3f1>         mVertices;
    Vector<Vec3f1>         mNormals;
    Vector<t_TexCoordPair> mTextureCoords;
    Vector<Vec3f1>         mVertexColors;
    Vector<TriangleInfo>   mTriangleInfos;
   

public:

    typedef tBasicTriangleMesh Mesh;

    BasicTriangleMeshPopulator(
        tBasicTriangleMesh* aTriangleMesh,
        const uint aTriangleSizeReserve = 50000
    ) :
        mTriangleMesh(aTriangleMesh)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        addAppearance(IO::Configuration::EntryGroup(RTFACT__DEFAULT_APPEARANCE_NAME));
        setActiveAppearance(RTFACT__DEFAULT_APPEARANCE_NAME);

        const Vec3f1 zero = Vec3f1::C_0_0_0();
        
        mVertices.clear();
        mVertices.reserve(aTriangleSizeReserve);
        mVertices.push_back(zero);

        mNormals.clear();
        mNormals.reserve(aTriangleSizeReserve);
        mNormals.push_back(zero);

        mTextureCoords.clear();
        mTextureCoords.reserve(aTriangleSizeReserve);
        mTextureCoords.push_back(t_TexCoordPair(0.f, 0.f));

        mVertexColors.clear();
        mVertexColors.reserve(aTriangleSizeReserve);
        mVertexColors.push_back(zero);
    }

    void addAppearance(
        const IO::Configuration::EntryGroup& aAppearanceParams)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        const uint appearanceID = (uint)mTriangleMesh->appearances.size();

        mTriangleMesh->appearances.push_back(
            mAppearanceCreator.create(aAppearanceParams));

        mAppearanceIDs.insert(t_AppearanceMap::value_type(
            aAppearanceParams.getName(), appearanceID));
    }

    void setActiveAppearance(
        const std::string& aAppearanceName)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        t_AppearanceMap::const_iterator it =
            mAppearanceIDs.find(aAppearanceName);

        if(it != mAppearanceIDs.end())
        {
            mActiveAppearanceID = it->second;
        }
        else
        {
            mActiveAppearanceID = 0;
        }
    }

    void addVertex(
        const float aX,
        const float aY,
        const float aZ)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        mVertices.push_back(Vec3f1(aX, aY, aZ));
    }

    void addNormal(
        const float aX,
        const float aY,
        const float aZ)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        mNormals.push_back(Vec3f1(aX, aY, aZ));
    }

    void addTextureCoords(
        const float aS,
        const float aT)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        mTextureCoords.push_back(t_TexCoordPair(aS, aT));
    }

    void addVertexColor(
        const float aR,
        const float aG,
        const float aB)
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        mVertexColors.push_back(Vec3f1(aR, aG, aB));
    }

    void addTriangle(
        const int aVertex1,
        const int aVertex2,
        const int aVertex3,
        const int aNormal1,
        const int aNormal2,
        const int aNormal3,
        const int aTexCoord1,
        const int aTexCoord2,
        const int aTexCoord3,
        const int aVertexColor1 = -1,
        const int aVertexColor2 = -1,
        const int aVertexColor3 = -1)
    {
        mTriangleInfos.push_back(TriangleInfo());

        TriangleInfo& tInfo = mTriangleInfos.back();

        tInfo.vertices[0] = aVertex1 + 1;
        tInfo.vertices[1] = aVertex2 + 1;
        tInfo.vertices[2] = aVertex3 + 1;

        tInfo.normals[0] = aNormal1 + 1;
        tInfo.normals[1] = aNormal2 + 1;
        tInfo.normals[2] = aNormal3 + 1;

        tInfo.texCoords[0] = aTexCoord1 + 1;
        tInfo.texCoords[1] = aTexCoord2 + 1;
        tInfo.texCoords[2] = aTexCoord3 + 1;

        tInfo.vertColors[0] = aVertexColor1 + 1;
        tInfo.vertColors[1] = aVertexColor2 + 1;
        tInfo.vertColors[2] = aVertexColor3 + 1;

        tInfo.appearanceID = mActiveAppearanceID;
    }

    void finalize()
    {
        RTFACT_ASSERT(mTriangleMesh != RTFACT_NULL);

        for(uint i = 0; i < mTriangleInfos.size(); ++i)
        {
            mTriangleMesh->primitives.push_back(t_Triangle());

            t_Triangle& triangle = mTriangleMesh->primitives.back();
            TriangleInfo& tInfo = mTriangleInfos[i];

            for(uint j = 0; j < 3; ++j)
            {
                triangle.getVertex(j)      = mVertices[tInfo.vertices[j]];
                triangle.getNormal(j)      = mNormals[tInfo.normals[j]];
                triangle.getTexCoords(j)   = mTextureCoords[tInfo.texCoords[j]];
                triangle.getVertexColor(j) = mVertexColors[tInfo.vertColors[j]];
            }

            triangle.updateNormal();
            triangle.appearanceID = tInfo.appearanceID;
        }
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__DEFAULT_APPEARANCE_NAME

#endif // RTFACT__BASICSCENECREATOR_HPP
