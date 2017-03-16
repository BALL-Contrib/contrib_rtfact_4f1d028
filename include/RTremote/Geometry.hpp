/* 
 *  Geometry.hpp
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
 *  Created on: 2008-11-06 02:52:20 +0100
 *  Author(s): Felix Klein, Iliyan Georgiev, Petr Kmoch, Vaclav Krajicek
 */
 
 #ifndef RTFACT_REMOTE__GEOMETRY_HPP
#define RTFACT_REMOTE__GEOMETRY_HPP

#include <RTremote/Config/Common.hpp>

#include <RTremote/Renderer.hpp>

//Disable MSVC warning about non-exported dll interface caused by boost::intrusive_ptr
#if defined(RTREMOTE_COMPILER_MSVC)
#   pragma warning(push)
#   pragma warning(disable:4251)
#endif

RTFACT_REMOTE_NAMESPACE_BEGIN

/**
 *  The Geometry Class corresponds to a geometry node that is or is not connected
 *  to a scene graph.
 *
 *
 *  Multiple geometry objects can correspond to the same scene graph node.
 *  Always use the class operator for comparision and no pointer-equality.
 *
 *  A geometry node contains a mesh (represented as a list of primitives). Each
 *  geometry is linked to an RTAppearance, defining the appearance of the mesh.
 */
class GeoPimpl;

class RTFACT_REMOTE_API Geometry :
    public GraphEntity
{
    friend class Renderer;
	friend class Group;
	friend class Picking;

private:

	GeoPimpl *pimpl;

	Geometry(
        Renderer *s,
		GeoPimpl *aPimpl,
		RTAppearanceHandle aAppearance);

protected:

    RTAppearanceHandle appearance;
    virtual ~Geometry();

public:


	Geometry(
        Renderer *s);

	Geometry(
        Renderer *s,
		const float* aVertices,
		const float* aNormals,
		const float* aVertexColors,
		const unsigned int* aTriangles,
		const unsigned int aTriangleCount,
		RTAppearanceHandle aAppearance);

	Geometry(
        Renderer *s,
		const float* aVertices,
		const float* aNormals,
		const unsigned int* aTriangles,
		const unsigned int aTriangleCount,
		RTAppearanceHandle aAppearance);

    /**
     *  compare the geometry to another geometry
     *  Be aware, that multiple geometry objects can be generated
     *  for the same node in the scene graph
     *  This method should be used instead of pointer comparision
     *  @param g a geometry to compare too
     *  @return true if both geometries refer to the same node in the scene graph, false otherwise
     */
    bool operator==(const Geometry &g) const;

    /**
     *  compare two geometries with == operator
     *  works with GeoHandles and considers 0 pointers.
     *  @param g1 a handle to a geometry
     *  @param g2 another handle to a geometry
     *  @return true if both geometries refer to the same node in the scene graph, false otherwise
     */
    static bool equal(GeoHandle g1, GeoHandle g2);


    /**
     *  get the number of primitives (triangles) of the geometry
     *  @return the number of primitives
     */
    size_t getPrimitiveCount();

    /**
     *  set the primitives of this geometry. Old primitives are replaced
     *  @param triangles a vector containing the triangles
     */
    void setPrimitives(
        const TriangleVector &triangles);

    /**
     *  add a primitive to the geometry
     *  @param aPrim the triangle to be added to the geometry
     */
    void addPrimitive(
        const RTfact::Remote::Triangle& aPrim);

    /**
     *  set the appearance of the geometry
     *  @param aMat the appearance to be used by the geometry
     */
    void setAppearance(
        const RTAppearanceHandle &aMat);

    /**
     *  @return the number of parents
     */
    size_t getParentCount();

    /**
     *  get the i-th parent.
     *  @param i the index of the parent.
     */
    GroupHandle getParent(
        unsigned long i);

};

RTFACT_REMOTE_NAMESPACE_END

//disabling MSVC warning 4251
#if defined(RTREMOTE_COMPILER_MSVC)
#   pragma warning(pop)
#endif

#endif // RTFACT_REMOTE__GEOMETRY_HPP
