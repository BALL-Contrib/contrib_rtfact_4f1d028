/* 
 *  Group.hpp
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
 *  Author(s): Felix Klein, Iliyan Georgiev, Petr Kmoch, Dmitri Rubinstein
 */
 
 #ifndef RTFACT_REMOTE__GROUP_HPP
#define RTFACT_REMOTE__GROUP_HPP

#include <RTremote/Config/Common.hpp>

#include <RTremote/Renderer.hpp>


RTFACT_REMOTE_NAMESPACE_BEGIN

/**
 *  The Group Class corresponds to a group node that is or is not connected
 *  to a scene graph.
 *
 *  Multiple group objects can correspond to the same scene graph node.
 *  Always use the class operator for comparision and no pointer-equality.
 *
 *  A group node contains a number of other group nodes and geometry nodes.
 *  Additionally, a group specifies a transformation that is applied on
 *  it's content
 */
class GroupPimpl;

class RTFACT_REMOTE_API Group :
    public GraphEntity
{
    friend class Renderer;
	friend class Geometry;
  friend class Picking;

private:

	GroupPimpl *pimpl;

	static GroupHandle getRootGroup(Renderer *r);
    static void setRootGroup(Renderer *r, GroupHandle g);

	Group(
        Renderer *r, GroupPimpl *aPimpl);

protected:

  virtual ~Group();

public:

    Group(
        Renderer *r);

	  Group(
        Renderer *r, const Transform &t);

    /**
     *  compare the group to another group
     *  Be aware, that multiple group objects can be generated
     *  for the same node in the scene graph
     *  This method should be used instead of pointer comparision
     *  @param g a group to compare too
     *  @return true if both groups refer to the same node in the scene graph, false otherwise
     */
    bool operator==(const Group &g) const;

    /**
     *  compare two groups with == operator
     *  works with GroupHandles and considers 0 pointers.
     *  @param g1 a handle to a group
     *  @param g2 another handle to a group
     *  @return true if both groups refer to the same node in the scene graph, false otherwise
     */
    static bool equal(GroupHandle g1, GroupHandle g2);

    /**
     *  create a new group and add it to the child-nodes.
     *  @param t the transformation used by the newly created group
     *  @return a GroupHandle to the newly created group
     */
    GroupHandle createChildGroup(
        const Transform &t);


    /**
     *  add a geometry to the child-nodes
     *  @param aGeo a handle to the geometry to be added to this group
     */
    void add(
        boost::intrusive_ptr<Geometry> aGeo);

    /**
     *  remove a geometry from the child-nodes
     *  @param aGeo a handle to the geometry to be removed from this group
     */
    void remove(
        boost::intrusive_ptr<Geometry> aGeo);

    /**
     *  add a group to the child-nodes
     *  @param aGroup a handle to the group to be added to this group
     */
    void add(
        boost::intrusive_ptr<Group> aGroup);

    /**
     *  remove a group from the child-nodes
     *  @param aGroup a handle to the group to be removed from this group
     */
    void remove(
        boost::intrusive_ptr<Group> aGroup);

    /**
     *  @return the number of group children
     */
    size_t getGroupCount();

    /**
     *  get the i-th group child.
     *  @param i the index of the group child.
     */
    GroupHandle getGroup(
        unsigned long i);

    /**
     *  @return the number of geometry children
     */
    size_t getGeometryCount();

    /**
     *  get the i-th geometry child.
     *  @param i the index of the geometry child.
     */
    GeoHandle getGeometry(
        unsigned long i);

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

    /**
     *  set the Tranformation of the group
     *  @param t the transformation to be used by the group
     */
    void setTransform(
        const Transform &t);

    /**
     *  set this Group to be the Group containing the Camera.
     */
    void setCameraNode();





	//Transform getAccumTransform();
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__GROUP_HPP
