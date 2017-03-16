/* 
 *  Geometry.cpp
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
 *  Created on: 2008-06-26 16:20:31 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev, Petr Kmoch, Vaclav Krajicek
 */
 
 #include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB 

#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>

#include <RTremote/Geometry.hpp>

#include "RTremotePrivateConfig.hpp"


RTFACT_REMOTE_NAMESPACE_BEGIN

// -------------------------------------------
// helper functions
// -------------------------------------------
RTfact::Vec3f1 createRTfactVec3f(const float3 &v){
    return RTfact::Vec3f1(v[0], v[1], v[2]);
}


RTfact::Triangle createRTfactTriangle(const Triangle &t)
{
    RTfact::Triangle t2;
    for(int i=0; i<3; i++)
    {
        t2.vertices[i] = createRTfactVec3f(t.vertices[i]);
        t2.vertexColors[i] = createRTfactVec3f(t.vertexColors[i]);
        t2.texCoords[i] = t.texCoords[i];
    }
    if(t.normal[0] != 0 || t.normal[1] != 0  || t.normal[2] != 0)
        t2.normal = createRTfactVec3f(t.normal);
    else
        t2.updateNormal();

    if(t.normals[0][0] != 0 || t.normals[0][1] != 0  || t.normals[0][2] != 0)
        for(int i=0; i<3; i++)
            t2.normals[i] = createRTfactVec3f(t.normals[i]);
    else
        for(int i=0; i<3; i++)
            t2.normals[i] = t2.normal;

    RTfact::Vec3f1 q1 = t2.vertices[1] - t2.vertices[0];
    RTfact::Vec3f1 q2 = t2.vertices[2] - t2.vertices[0];
    float s1 = t2.texCoords[1].first  - t2.texCoords[0].first;
    float s2 = t2.texCoords[2].first  - t2.texCoords[0].first;
    float tc1 = t2.texCoords[1].second - t2.texCoords[0].second;
    float tc2 = t2.texCoords[2].second - t2.texCoords[0].second;
    t2.tangent = tc2 * q1 - tc1 * q2;
    t2.tangent.setNormalized();
    t2.binormal = s1 * q2 - s2 * q1;
    t2.binormal.setNormalized();
    return t2;
}



//-----------------------------------------------------------------------------
// Public Class Implementation
//-----------------------------------------------------------------------------


Geometry::Geometry(
	Renderer *r, 
	GeoPimpl *aPimpl, 
	RTAppearanceHandle aAppearance) :
	GraphEntity(r),
	pimpl(aPimpl),
	appearance(aAppearance)
{}


Geometry::Geometry(
    Renderer *r
	) :
    GraphEntity(r)
{
	t_InternMeshHandle mesh = new t_InternMesh(&(r->pimpl->scene));
	pimpl = new GeoPimpl(mesh);		
	appearance = r->defaultAppearance;
}

Geometry::Geometry(
	Renderer *r,
	const float* aVertices,
	const float* aNormals,
	const float* aVertexColors,
	const unsigned int* aTriangles,
	const unsigned int aTriangleCount,
	RTAppearanceHandle aAppearance) :
	GraphEntity(r), appearance(aAppearance)
{
    t_InternMeshHandle mesh = new t_InternMesh(&(r->pimpl->scene));

    for(uint i = 0; i < aTriangleCount; ++i)
    {
        RTfact::Triangle triangle;
        uint i3[3];

        i3[0] = aTriangles[3 * i];
        i3[1] = aTriangles[3 * i + 1];
        i3[2] = aTriangles[3 * i + 2];

        for(uint j = 0; j < 3; ++j)
        {
            uint j3[3];
            
            j3[0] = 3 * i3[j];
            j3[1] = 3 * i3[j] + 1;
            j3[2] = 3 * i3[j] + 2;
                    
            triangle.getVertex(j) = Vec3f1(aVertices[j3[0]], aVertices[j3[1]], aVertices[j3[2]]);
            triangle.getNormal(j) = Vec3f1(aNormals[j3[0]], aNormals[j3[1]], aNormals[j3[2]]);
            triangle.getVertexColor(j) = Vec3f1(aVertexColors[4 * i3[j]], aVertexColors[4 * i3[j] + 1], aVertexColors[4 * i3[j] + 2]);
        }

        triangle.appearanceID = aAppearance->getID();
        triangle.updateNormal();
        mesh->addPrimitive(triangle);
    }
	pimpl = new GeoPimpl(mesh);		
}

Geometry::Geometry(
	Renderer *r,
	const float* aVertices,
	const float* aNormals,
	const unsigned int* aTriangles,
	const unsigned int aTriangleCount,
	RTAppearanceHandle aAppearance) :
	GraphEntity(r), appearance(aAppearance)
{
    t_InternMeshHandle mesh = new t_InternMesh(&(r->pimpl->scene) );

    for(uint i = 0; i < aTriangleCount; ++i)
    {
        RTfact::Triangle triangle;
        uint i3[3];

        i3[0] = aTriangles[3 * i];
        i3[1] = aTriangles[3 * i + 1];
        i3[2] = aTriangles[3 * i + 2];

        for(uint j = 0; j < 3; ++j)
        {
            uint j3[3];
            
            j3[0] = 3 * i3[j];
            j3[1] = 3 * i3[j] + 1;
            j3[2] = 3 * i3[j] + 2;
        
            triangle.getVertex(j) = Vec3f1(aVertices[j3[0]], aVertices[j3[1]], aVertices[j3[2]]);
            triangle.getNormal(j) = Vec3f1(aNormals[j3[0]], aNormals[j3[1]], aNormals[j3[2]]);
            triangle.getVertexColor(j) = Vec3f1(0.f, 0.f, 1.f);
        }
        
        triangle.appearanceID = aAppearance->getID();
        triangle.updateNormal();
        mesh->addPrimitive(triangle);
    }
	pimpl = new GeoPimpl(mesh);		
}


Geometry::~Geometry()
{
	delete pimpl;
}

bool Geometry::operator==(const Geometry &g) const
{
    return pimpl->mMesh == g.pimpl->mMesh;
}

bool Geometry::equal(GeoHandle g1, GeoHandle g2)
{
    return g1==g2 || ( g1 && g2 ? *g1 == *g2 : false );
}


size_t Geometry::getPrimitiveCount(){
    return pimpl->mMesh->getPrimitiveCount();
}

void Geometry::setPrimitives(const TriangleVector & /*triangles*/){ //identifier commented out to prevent 'unused' warnings
    //this->renderer->setPrimitives(this, triangles);
}

void Geometry::addPrimitive(const Triangle &aPrim){
    RTfact::Triangle t = createRTfactTriangle(aPrim);
	t.appearanceID = appearance->getID();
	pimpl->mMesh->addPrimitive(t);
}

void Geometry::setAppearance(const RTAppearanceHandle &aMat)
{
	appearance = aMat;
	pimpl->mMesh->setAppearanceId(aMat->getID());
    //renderer->linkAppearanceToGeometry(this,aMat);
}

size_t Geometry::getParentCount()
{
    return pimpl->mMesh->getParentCount();
}

GroupHandle Geometry::getParent(
    unsigned long i)
{
    t_InternGroupHandle group = pimpl->mMesh->getParent(i);
    return new Group(renderer, new GroupPimpl(group));
}


RTFACT_REMOTE_NAMESPACE_END
