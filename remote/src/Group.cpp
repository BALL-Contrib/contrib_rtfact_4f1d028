/* 
 *  Group.cpp
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

#include <RTremote/Group.hpp>

#include "RTremotePrivateConfig.hpp"


RTFACT_REMOTE_NAMESPACE_BEGIN

// -------------------------------------------
// helper functions
// -------------------------------------------
RTfact::Transform createRTfactTransform(Transform t)
{
    Matrix4f m1(t.matrix);
    Matrix4f m2(t.invMatrix);
    if(!t.hasInverse)
        return RTfact::Transform(m1);
    else
        return RTfact::Transform(m1, m2);

}


GroupHandle Group::getRootGroup(Renderer *r){
	t_InternGroupHandle group = r->pimpl->scene.getRoot();
	return new Group(r,new GroupPimpl(group));	
}

void Group::setRootGroup(Renderer *r, GroupHandle g){
	r->pimpl->scene.setRoot(g->pimpl->mGroup);
}

Group::Group(Renderer *r, GroupPimpl *aPimpl) 
	: GraphEntity(r), pimpl(aPimpl) {}

Group::Group(Renderer *r) : GraphEntity(r) {
	t_InternGroupHandle group = 
		new t_InternGroup(&(r->pimpl->scene), 
            RTfact::Transform::identity());

	pimpl = new GroupPimpl(group);		
}

Group::Group(Renderer *r, const Transform &t) : GraphEntity(r) {
	t_InternGroupHandle group = 
		new t_InternGroup(&(r->pimpl->scene), 
			createRTfactTransform(t));

	pimpl = new GroupPimpl(group);		
}

Group::~Group() {
	delete pimpl;
}

bool Group::operator==(const Group &g) const
{
    return pimpl->mGroup == g.pimpl->mGroup;
}

bool Group::equal(GroupHandle g1, GroupHandle g2)
{
    return g1==g2 || ( g1 && g2 ? *g1 == *g2 : false );
}


GroupHandle Group::createChildGroup(
    const Transform &t)
{
	GroupHandle g = new Group(renderer, t);
	pimpl->mGroup->add(g->pimpl->mGroup);
    return g;
}

void Group::add(
    GroupHandle g)
{
	pimpl->mGroup->add(g->pimpl->mGroup);
}

void Group::add(
    GeoHandle g)
{
	pimpl->mGroup->add(g->pimpl->mMesh);
}

void Group::remove(
    GroupHandle g)
{
	pimpl->mGroup->remove(g->pimpl->mGroup);
}

void Group::remove(
    GeoHandle g)
{
	pimpl->mGroup->remove(g->pimpl->mMesh);
}





size_t Group::getGroupCount()
{
    return pimpl->mGroup->getGroupCount();
}


GroupHandle Group::getGroup(
    unsigned long i)
{
	t_InternGroupHandle group = pimpl->mGroup->getChildGroup(i);
	return new Group(renderer, new GroupPimpl(group));
}

size_t Group::getGeometryCount()
{
    return pimpl->mGroup->getInstanceCount();
}

GeoHandle Group::getGeometry(
    unsigned long i)
{
	t_InternMeshHandle mesh = pimpl->mGroup->getChildInstance(i)->getMesh();
	RTAppearanceHandle a = renderer->getAppearance(mesh->getAppearanceId());

	return new Geometry(renderer, new GeoPimpl(mesh), a); 
}

size_t Group::getParentCount()
{
    return pimpl->mGroup->getParentCount();
}

GroupHandle Group::getParent(
    unsigned long i)
{
	t_InternGroupHandle group = pimpl->mGroup->getParent(i);
	return new Group(renderer, new GroupPimpl(group));
}


void Group::setTransform(
    const Transform &t)
{
    pimpl->mGroup->setTransform(createRTfactTransform(t));
}

void Group::setCameraNode()
{
    renderer->setCameraNode( this );
}

/*
Transform Group::getAccumTransform()
{
	return pimpl->mGroup->getAccumTransform();
}
*/

RTFACT_REMOTE_NAMESPACE_END
