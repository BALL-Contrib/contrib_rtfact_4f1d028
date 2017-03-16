/* 
 *  GraphScene.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2008-06-20 17:30:16 +0200
 *  Author(s): Iliyan Georgiev, Felix Klein, Dmitri Rubinstein
 */
 
 #ifndef _RTFACT_LACHSSCENEHANDLER_HPP_INCLUDED
#define _RTFACT_LACHSSCENEHANDLER_HPP_INCLUDED

#include <RTfact/Model/Scene/HandledScene.hpp>
#include <queue>
#include <vector>
#include <iostream>
#include <RTfact/Utils/Transform.hpp>
#include <RTfact/Utils/Containers/IntMap.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tPrimitiveConfig,
    class tAppearanceConfig, 
    class tLightConfig,
    class tEnvironmentTexture,
    umask taProperties = 0>
class GraphScene :
    public HandledScene<
        tAppearanceConfig, tLightConfig, tEnvironmentTexture, taProperties>
{
    typedef HandledScene<
        tAppearanceConfig, tLightConfig, tEnvironmentTexture, taProperties> t_Base;

public:

    typedef typename tPrimitiveConfig::PrimitiveBase PrimitiveBase;
    typedef IntMap<PrimitiveBase>                    PrimitiveMap;
    typedef Vector<int>                              IndexVector; 
    typedef Transform                                TransformType;
    typedef typename t_Base::HandleIndex              HandleIndex;
    typedef typename t_Base::HandleIndexVec          HandleIndexVec;
    typedef typename t_Base::Handle                  Handle;

    //typedef Vector<Triangle> FaceImportVec;

    enum
    {
        EMPTY = -1,
        GEOMETRY = 1,
        GROUP =2
    };
    
    struct LachsGeometry
    {
        RTFACT_SIMD_ALIGNED_NEW_OPERATORS

        BBox bounds;
        typename PrimitiveMap::key_vector_ptr primitives;
        IndexVector parents;
        bool need_build_update;

        template<class ta_iterator>
        LachsGeometry(PrimitiveMap &prims, ta_iterator aBegin, ta_iterator aEnd)
        {
            setPrimitives(prims, aBegin, aEnd);
        }
        template<class ta_iterator>
        void setPrimitives(PrimitiveMap &prims, ta_iterator aBegin, ta_iterator aEnd)
        {
            need_build_update=true;
            bounds = BBox::empty();
            for(ta_iterator it= aBegin; it != aEnd; ++it)
                bounds.extend(it->getBounds());
            if(aEnd - aBegin > 0)
				primitives = prims.insert(aBegin, aEnd);
			else
				primitives = new typename PrimitiveMap::key_vector();
        }
        template<class ta_iterator>
        void addPrimitives(PrimitiveMap &prims, ta_iterator aBegin, ta_iterator aEnd)
        {
			typedef typename PrimitiveMap::key_vector_ptr t_primKeysPtr;
			typedef typename PrimitiveMap::key_vector::iterator t_primKeysIt;
			
            need_build_update=true;
            for(ta_iterator it= aBegin; it != aEnd; ++it)
                bounds.extend(it->getBounds());

            t_primKeysPtr new_primitives = prims.insert(aBegin, aEnd);
			for(t_primKeysIt it = new_primitives->begin(); it != new_primitives->end(); it++)
			{
				primitives->push_back(*it);
			}
        }

        void addPrimitive(PrimitiveMap &prims, const PrimitiveBase &aPrim)
        {
			typedef typename PrimitiveMap::key_vector_ptr t_primKeysPtr;
			typedef typename PrimitiveMap::key_type t_primKey;
			typedef typename PrimitiveMap::key_vector::iterator t_primKeysIt;
            need_build_update=true;

            t_primKey newKey = prims.insert(aPrim);
			
			// use bound of inserted element, because aPrim might not be aligned.
			bounds.extend(prims[newKey].getBounds());
			// Update Normal (since it's not set properly anyway...)
			prims[newKey].updateNormal();

			primitives->push_back(newKey);
        }
    };

    struct LachsGroup
    {    
        RTFACT_SIMD_ALIGNED_NEW_OPERATORS

        BBox bounds;
        TransformType trans;

        IndexVector geoChildren;
        IndexVector groupChildren;
        IndexVector parents;

        bool need_update;
        bool need_build_update;
        int depth;
        int numOfPrimitives;
        int numOfInstances;
        int subtreeSize;
        LachsGroup() : need_update(false), depth(0), numOfPrimitives(0), numOfInstances(0), subtreeSize(0) {}
    };    




    typedef Vector<LachsGeometry*> geoVec;
    typedef Vector<LachsGroup*> groupVec;

    struct compareFunction
    {    
        bool operator()(std::pair<int, int> x, std::pair<int, int> y) {
            return x.second < y.second;
        }
    };



    // container for geometries and groups:
    geoVec geometries;
    groupVec groups;
    PrimitiveMap primitives;
    Vector<PrimitiveBase> flattenPrimitives;

    int root; // Index of the Group, that is the root of the Graph

        // FIX ME: priority queue doesn't work with RTfact::Vector on GCC
    //std::priority_queue< std::pair<int,int> , std::vector< std::pair<int,int> >, compareFunction> update_queue;
    std::priority_queue< std::pair<int,int> , Vector< std::pair<int,int> >, compareFunction> update_queue;
    // Some functions for better Memory-Management
    IndexVector free_geometries;
    IndexVector free_groups;

    // Some Testfunction1 that prints the structure of the SceneHandler.
    void vecString(const Vec3f<1> &v)
    {
        std::cout<<"<"<<v.xf<<", "<<v.yf<<", "<<v.zf<<">";
    }

    void print_content()
    {
/*      std::cout<<"\nContent:----------------------------------------------------------";
        std::cout<<"\n\nGeometries - amount: "<<geometries.size();
        for(uint i=0; i<geometries.size(); ++i)
        {
            LachsGeometry *geo=geometries[i];
            if(!geo)
                std::cout<<"\n- REMOVED Geo ";
            else
            {
                std::cout<<"\n- Geo#"<<i<<" Bounds: [";
                vecString(geo->bounds.min);
                std::cout<<",";
                vecString(geo->bounds.max);
                std::cout<<"]";
                std::cout<<" Prims: "<<geo->primitives.size();
                
                for(typename Vector<PrimitiveBase>::iterator it=geo->primitives.begin(); it!=geo->primitives.end(); ++it)
                {
                    std::cout<<"\n   |-(";vecString((*it).vertices[0]);std::cout<<", ";
                    vecString((*it).vertices[1]);std::cout<<", ";vecString((*it).vertices[2]);
                    std::cout<<")";
                }
                
            }
        }
        std::cout<<"\n\nGroups: - amount: "<<groups.size();
        for(uint i=0; i<groups.size(); ++i)
        {
            LachsGroup *group=groups[i];
            if(!group)
                std::cout<<"\n- REMOVED Group ";
            else
            {
                std::cout<<"\n- Group#"<<i<<" Bounds: [";vecString(group->bounds.min);std::cout
                    <<",";vecString(group->bounds.max);std::cout<<"]"<<", Depth: "<<group->depth;
                for(IndexVector::iterator it=group->geoChildren.begin(); it!=group->geoChildren.end(); ++it)
                    std::cout<<"\n   |- Geo#"<<(*it);
                for(IndexVector::iterator it=group->groupChildren.begin(); it!=group->groupChildren.end(); ++it)
                    std::cout<<"\n   |- Group#"<<(*it);
            }
        }
        std::cout<<"\n\nGroups to be updated: ";
        Vector< std::pair<int,int> > queue_vec;
        while(!update_queue.empty())
        {
            std::cout<<"\n- Group#"<<update_queue.top().first;
            queue_vec.push_back(update_queue.top());
            update_queue.pop();
        }
        while(!queue_vec.empty())
        {
            update_queue.push(queue_vec.back());
            queue_vec.pop_back();
        }

        std::cout<<"\n\nStructure: ";
        char buf[100];
        strcpy_s(buf,100,"  ");
        print_structure(buf);

        std::cout<<"\n\nErrors: ";
        check_group_linking();
        check_geo_linking();

        std::cout<<"\n----------------------------------------------------------Ende";
*/
    }
    
    void print_structure(char *pre, int g_i=-1)
    {    
/*        if(g_i==-1){
            g_i=root;
            std::cout<<"\n(root "<<g_i<<")";
        }
        if(g_i==-1){
            std::cout<<"\nRoot not defined - Empty Tree";
            return;
        }
        char pre2[100];
        strcpy_s(pre2,100,pre);
        strcat_s(pre2,100,"  ");
        for(uint i=0; i<groups[g_i]->geoChildren.size(); i++)
            std::cout<<"\n"<<pre<<"|-(Geo "<<groups[g_i]->geoChildren[i]<<")";
        
        for(uint i=0; i<groups[g_i]->groupChildren.size(); i++){
            std::cout<<"\n"<<pre<<"|-(G "<<groups[g_i]->groupChildren[i]<<")";
            print_structure(pre2,groups[g_i]->groupChildren[i]);
        }

*/
    }

    void check_group_linking(int node=-1){
        // Checks Linking of the Tree starting from the root
        if(node==-1)
            node=root;
        if(node==-1)
            return;

        LachsGroup* lg=groups[node];
        for(IndexVector::iterator it=lg->geoChildren.begin(); it !=lg->geoChildren.end(); ++it)
        {
            LachsGeometry* lg2=geometries[*it];
            IndexVector::iterator it2=lg2->parents.begin();
            while(it2 != lg2->parents.end() && (*it2)!=node) ++it2;
            if(it2 == lg2->parents.end())
                std::cout<<"\n!! Group#"<<node<<" -> Geo#"<<(*it)<<" MISSING PARENT"; 
        }
        for(IndexVector::iterator it=lg->groupChildren.begin(); it !=lg->groupChildren.end(); ++it)
        {
            LachsGroup* lg2=groups[*it];
            IndexVector::iterator it2=lg2->parents.begin();
            while(it2 != lg2->parents.end() && (*it2)!=node) ++it2;
            if(it2 == lg2->parents.end())
                std::cout<<"\n!! Group#"<<node<<" -> Group#"<<(*it)<<" MISSING PARENT"; 
            check_group_linking(*it);
        }
        for(IndexVector::iterator it=lg->parents.begin(); it !=lg->parents.end(); ++it)
        {
            LachsGroup* lg2=groups[*it];
            IndexVector::iterator it2=lg2->groupChildren.begin();
            while(it2 != lg2->groupChildren.end() && (*it2)!=node) ++it2;
            if(it2 == lg2->groupChildren.end())
                std::cout<<"\n!! Group#"<<node<<" -> Group#"<<(*it)<<" MISSING CHILD"; 
        }
    }
    void check_geo_linking(){
        // Checks the linking of all geometries.
        for(uint i=0; i<geometries.size(); ++i)
        {    
            LachsGeometry* lg=geometries[i];
            if(lg)
            {
                for(IndexVector::iterator it= lg->parents.begin(); it != lg->parents.end(); ++it)
                {
                    LachsGroup* lg2=groups[*it];
                    IndexVector::iterator it2=lg2->geoChildren.begin();
                    while(it2 != lg2->geoChildren.end() && (*it2)!=i) ++it2;
                    if(it2 == lg2->geoChildren.end())
                        std::cout<<"\n!! Group#"<<(*it)<<" -> Geo#"<<i<<" MISSING CHILD"; 
                }
            }    
        }
    }




    GraphScene() : root(-1){
    }
    

    // Updating Functions:

    template<class ta_iterator>
    HandleIndex     createGeometry(ta_iterator aBegin, ta_iterator aEnd)
    {    
        // First put the geometry inside the scene.

        LachsGeometry *lg = new LachsGeometry(primitives, aBegin, aEnd);

        if(free_geometries.size()>0)
        { 
            // fill empty space in geometrie-Vector.
            int i=free_geometries.back();
            free_geometries.pop_back();
            geometries[i]=lg;
            return createHandle(GEOMETRY,i);
        }
        else{
            // extend geometrie-Vector.
            geometries.push_back(lg);
            return createHandle(GEOMETRY, geometries.size()-1);
        }
    }

    void        removeGeometry(HandleIndex geoHandle)
    {    
        Handle h= getHandle(geoHandle);
        // TODO: Implement Exception or something.
        assert(h.type==GEOMETRY);

        LachsGeometry *lg= geometries[h.index];
        geometries[h.index]=RTFACT_NULL;
        // Remove FaceList of Geometry from Scene.
        primitives.erase(lg->primitives->begin(),lg->primitives->end());
        
        // Delete all Handles pointing to the deleted geometry
        releaseHandle(h);
        
        // Remove Links to this Geometry.
        for(uint i=0; i<lg->parents.size(); ++i){
            int k=lg->parents[i];
            IndexVector::iterator it= groups[k]->geoChildren.begin();
            while(it != groups[k]->geoChildren.end())
                if(*it==h.index)
                {
                    setUpdateGroup(k);
                    it= groups[k]->geoChildren.erase(it);
                }
                else
                    ++it;
        }
        
        // Mark the geometry as free.
        free_geometries.push_back(h.index);
        
        delete lg;
    }

    HandleIndex        createGroup(const TransformType& trans = Transform::identity(), 
        const BBox &boundings = BBox::empty())
    {
        LachsGroup* lgp = new LachsGroup;
        lgp->bounds=boundings;
        lgp->trans=trans;

        int index;
        if(free_groups.size()>0){ 
            std::cerr << "Size > 0 : "<<free_groups.size();
            // fill empty space in group-Vector.
            int i=free_groups.back();
            free_groups.pop_back();
            groups[i]=lgp;
            index=i;
        }
        else{
            // extend group-Vector.
            groups.push_back(lgp);
            index=groups.size()-1;
        }

        if(root==-1)
            root=index;

        return createHandle(GROUP,index);
    }

    void        removeGroup(HandleIndex groupHandle){
        Handle h=getHandle(groupHandle);
        // TODO: Implement Exception or something.
        assert(h.type==GROUP);

        // You cannot delete a root, that has more than one child.
        assert(root!=h.index || groups[h.index]->groupChildren.size()<2);

        // Delete all Handles pointing to the deleted group
        releaseHandle(h);
        
        LachsGroup *lgp= groups[h.index];
        groups[h.index]=RTFACT_NULL;

        if(root==h.index)
            if(lgp->groupChildren.size()>0)
                root=lgp->groupChildren[0];
            else
                root=-1;

        // Remove Links from Parents to this Group.
        for(uint i=0; i<lgp->parents.size(); ++i){
            int k=lgp->parents[i];
            IndexVector::iterator it= groups[k]->groupChildren.begin();
            setUpdateGroup(k);
            while(it != groups[k]->groupChildren.end())
                if(*it==h.index){
                    it=groups[k]->groupChildren.erase(it);
                }
                else
                    it++;
        }

        // Remove Links from GroupChildren to this Group.
        for(uint i=0; i<lgp->groupChildren.size(); ++i){
            int k=lgp->groupChildren[i];
            IndexVector::iterator it= groups[k]->parents.begin();
            while(it != groups[k]->parents.end())
                if(*it==h.index){
                    it=groups[k]->parents.erase(it);
                    resetDepth(k);
                }
                else
                    it++;
        }

        // Remove Links from GeometryChildren to this Group.
        for(unsigned  int i=0; i<lgp->geoChildren.size(); ++i){
            int k=lgp->geoChildren[i];
            IndexVector::iterator it= geometries[k]->parents.begin();
            while(it != geometries[k]->parents.end())
                if(*it==h.index)
                    it=geometries[k]->parents.erase(it);
                else
                    it++;
        }
        
        
        // Mark the group as free.
        free_groups.push_back(h.index);

        delete lgp;
    }

    void        linkChildGeometry(HandleIndex parent, HandleIndex child)
    {
        Handle hp=getHandle(parent);
        Handle hc=getHandle(child);
        // TODO: Implement Exception or something.
        assert(hp.type==GROUP && hc.type==GEOMETRY);

        LachsGroup *p= groups[hp.index];
        LachsGeometry *c= geometries[hc.index];

        for(uint i=0; i < p->geoChildren.size() ; ++i)
            assert(p->geoChildren[i]!=hc.index);
                // Parent has already this Geometry as child.
        
        // Connection
        p->geoChildren.push_back(hc.index);
        c->parents.push_back(hp.index);

        // Update Parent Bounds etc.
        setUpdateGroup(hp.index);
    }

    void        unlinkChildGeometry(HandleIndex parent, HandleIndex child)
    {
        Handle hp=getHandle(parent);
        Handle hc=getHandle(child);
        // TODO: Implement Exception or something.
        assert(hp.type==GROUP && hc.type==GEOMETRY);    
    
        LachsGroup *p= groups[hp.index];
        LachsGeometry *c= geometries[hc.index];    
        
        IndexVector::iterator it= p->geoChildren.begin();
        while(it != p->geoChildren.end())
            if(*it==hc.index)
                it=p->geoChildren.erase(it);
            else
                it++;

        it= c->parents.begin();
        while(it != c->parents.end())
            if(*it==hp.index)
                it=c->parents.erase(it);
            else
                it++;

        // Update Parent Bounds etc.
        setUpdateGroup(hp.index);
    }

    
    void        linkChildGroup(HandleIndex parent, HandleIndex child)
    {
        Handle hp=getHandle(parent);
        Handle hc=getHandle(child);
        // TODO: Implement Exception or something.
        assert(hp.type==GROUP && hc.type==GROUP);

        LachsGroup *p= groups[hp.index];
        LachsGroup *c= groups[hc.index];

        for(uint i=0; i < p->groupChildren.size() ; ++i)
            if(p->groupChildren[i]==hc.index)
                return; // Parent has already this Group as child.
        
        // Connection
        p->groupChildren.push_back(hc.index);
        c->parents.push_back(hp.index);
        // Update Root
        if(root==hc.index)
            root=hp.index;
        // Update Depth
        increaseDepth(hc.index, p->depth + 1, hp.index);

        // Update Parent Bounds etc.
        setUpdateGroup(hp.index);
    }

    void        unlinkChildGroup(HandleIndex parent, HandleIndex child)
    {
        Handle hp=getHandle(parent);
        Handle hc=getHandle(child);
        // TODO: Implement Exception or something.
        assert(hp.type==GROUP && hc.type==GROUP);    
    
        LachsGroup *p= groups[hp.index];
        LachsGroup *c= groups[hc.index];    
        
        IndexVector::iterator it= p->groupChildren.begin();

        while(it != p->groupChildren.end())
            if(*it==hc.index)
                it=p->groupChildren.erase(it);
            else
                it++;

        it= c->parents.begin();
        while(it != c->parents.end())
            if(*it==hp.index)
                it=c->parents.erase(it);
            else
                it++;

        resetDepth(hc.index);

        // Update Parent Bounds etc.
        setUpdateGroup(hp.index);
    }

    HandleIndex getRoot() const{
        if(root!=-1)
            return createHandle(GROUP, root);
        else
            return -1;
            
    }

    void    getChildGroups(HandleIndex parent, HandleIndexVec &result) const
    {
        Handle hp=getHandle(parent);
        assert(hp.type==GROUP);
        
        LachsGroup *lgp= groups[hp.index];
        result.resize(lgp->groupChildren.size());

        for(uint i=0; i< lgp->groupChildren.size(); ++i)
            result[i]=createHandle(GROUP, lgp->groupChildren[i]);
    }


    void    getChildGeometries(HandleIndex parent, HandleIndexVec &result)
    {
        Handle hp=getHandle(parent);
        assert(hp.type==GROUP);
        
        LachsGroup *lgp= groups[hp.index];
        result.resize(lgp->geoChildren.size());

        for(uint i=0; i< lgp->geoChildren.size(); ++i)
            result[i]=createHandle(GEOMETRY, lgp->geoChildren[i]);
    }

    bool isConnected(HandleIndex node)
    {
        Handle hp=getHandle(node);
        if(hp.type==GROUP){
            return hp.index==root || groups[hp.index]->parents.size() > 0;
        }
        else if(hp.type==GEOMETRY){
            return geometries[hp.index]->parents.size() > 0;
        }
        //RTFACT_USER_ERROR
        return false;

    }

    BBox getBounds(HandleIndex aIndex)
    {
        Handle hp=getHandle(aIndex);
        assert(hp.type==GEOMETRY || hp.type==GROUP);

        if(hp.type==GEOMETRY)
            return geometries[hp.index]->bounds;
        else
            return groups[hp.index]->bounds;
    }

    int getNumOfPrimitives(HandleIndex aIndex) const
    {
        Handle hp=getHandle(aIndex);
        assert(hp.type==GEOMETRY || hp.type==GROUP);

        if(hp.type==GEOMETRY)
            return geometries[hp.index]->primitives->size();
        else
            return groups[hp.index]->numOfPrimitives;
    }

    int getNumOfInstances(HandleIndex aIndex) const
    {
        Handle hp=getHandle(aIndex);
        assert(hp.type==GEOMETRY || hp.type==GROUP);

        if(hp.type==GEOMETRY)
            return 1;
        else
            return groups[hp.index]->numOfInstances;
    }

    const Transform& getTransform(HandleIndex aIndex) const
    {
        Handle hp=getHandle(aIndex);

        // Only Groups may have transformations
        assert(hp.type==GROUP);
        
        return groups[hp.index]->trans;
    }

    Transform getAccumTransform(HandleIndex aIndex)
    {
        Handle hp=getHandle(aIndex);

        // Only Groups may have transformations
        assert(hp.type==GROUP);
        
        LachsGroup* current = groups[hp.index];
        Transform result = current->trans;
        
        while(current->parents.size()>0){
            current = groups[current->parents[0]];
            result = current->trans * result;
        }

        return result;
    }


    void        setTransformation(HandleIndex groupHandle, const TransformType& t)
    {
        Handle hp=getHandle(groupHandle);
        assert(hp.type==GROUP);
        groups[hp.index]->trans=t;
        setUpdateGroup(hp.index);
    }
    
    template<class ta_iterator>
    void    setPrimitives(HandleIndex geoHandle, ta_iterator aBegin, ta_iterator aEnd){
        Handle hp=getHandle(geoHandle);
        assert(hp.type==GEOMETRY);
        geometries[hp.index]->setPrimitives(primitives, aBegin, aEnd);
        for(IndexVector::iterator it = geometries[hp.index]->parents.begin(); 
            it < geometries[hp.index]->parents.end(); ++it)
            setUpdateGroup(*it);
    }

    template<class ta_iterator>
    void    addPrimitives(HandleIndex geoHandle, ta_iterator aBegin, ta_iterator aEnd){
        Handle hp=getHandle(geoHandle);
        assert(hp.type==GEOMETRY);
        geometries[hp.index]->addPrimitives(primitives, aBegin, aEnd);
        //for(IndexVector::iterator it = geometries[hp.index]->parents.begin(); 
        //    it < geometries[hp.index]->parents.end(); ++it)
        //    setUpdateGroup(*it);
    }

    void    addPrimitive(HandleIndex geoHandle, const PrimitiveBase& aPrim)
	{
        Handle hp=getHandle(geoHandle);
        assert(hp.type==GEOMETRY);
        geometries[hp.index]->addPrimitive(primitives, aPrim);
        //for(IndexVector::iterator it = geometries[hp.index]->parents.begin(); 
        //    it < geometries[hp.index]->parents.end(); ++it)
        //    setUpdateGroup(*it);
    }

   size_t getAppearanceID(HandleIndex geoHandle)
   {
        typedef typename PrimitiveMap::key_vector::Iterator tKeyIterator;

        Handle hp=getHandle(geoHandle);
        assert(hp.type==GEOMETRY);

        LachsGroup* geo = geometries[hp.index];
        tKeyIterator it = geo->primitives->begin();
        return primitives[*it].appearanceID;
    }

    void setAppearanceID(HandleIndex geoHandle, size_t aAppearanceID){
        typedef typename PrimitiveMap::key_vector::iterator tKeyIterator;

        Handle hp=getHandle(geoHandle);
        assert(hp.type==GEOMETRY);
        
        LachsGeometry* geo = geometries[hp.index];
        
        for(tKeyIterator it = geo->primitives->begin(); it != geo->primitives->end(); ++it)
            primitives[*it].appearanceID = aAppearanceID;
    }

    
    // Update all Boundings and numOfInstances. Has to be called before rendering starts!
    void        update()
    {

        while(!update_queue.empty())
        {
            int index= update_queue.top().first;
            update_queue.pop();
            LachsGroup* lg= groups[index];
            if(lg && lg->need_update)
            {    
                lg->need_update=false;
                lg->need_build_update=true;
                //std::cout<<"\nUpdate Group: "<<index;
                //if(index == root)
                //    std::cout<<"(this is the root!)";
                lg->numOfInstances=0;
                lg->numOfPrimitives=0;
                lg->subtreeSize=1;
                lg->bounds=BBox::empty();

                for(IndexVector::iterator it=lg->geoChildren.begin(); it != lg->geoChildren.end(); ++it)
                {
                    ++(lg->numOfInstances);
                    ++(lg->subtreeSize);
                    lg->numOfPrimitives+=geometries[*it]->primitives->size();
                    lg->bounds.extend(lg->trans.applyAffine(geometries[*it]->bounds));
                }

                for(IndexVector::iterator it=lg->groupChildren.begin(); it != lg->groupChildren.end(); ++it)
                {
                    lg->numOfInstances += groups[*it]->numOfInstances;
                    lg->numOfPrimitives += groups[*it]->numOfPrimitives;
                    lg->subtreeSize += groups[*it]->subtreeSize;
                    lg->bounds.extend(lg->trans.applySafeAffine(groups[*it]->bounds));
                }

                for(IndexVector::iterator it=lg->parents.begin(); it != lg->parents.end(); ++it)
                    if(!groups[*it]->need_update)
                        setUpdateGroup(*it);
                        
            }
        }
    }


    // Builder related Methods:
    int getNumOfInstances() const
    {
        return groups[root]->numOfInstances;
    }

    int getNumOfInnernodes() const
    {
        return groups[root]->subtreeSize - groups[root]->numOfInstances;
    }

    // Special Methods for Testing:

    void computeFlattenedPrimitives(){
        flattenPrimitives.clear();
        rec_flatten(groups[root], Transform::identity());
    }


private:

    void increaseDepth(int index, int depth, int origin)
    {
        // No cycles!
        assert(index!=origin);

        LachsGroup *lgp=groups[index];



        if(lgp->depth<depth)
        {
            lgp->depth=depth;

            // if the group already needs an update, another request is inserted,
            // since the depth has increased.
            if(lgp->need_update)
                setUpdateGroup(index);

            for(uint i=0; i<lgp->groupChildren.size(); ++i)
                increaseDepth(lgp->groupChildren[i], depth+1, origin);
        }
    }

    void resetDepth(int index)
    {
        LachsGroup *lgp=groups[index];
        int max_depth=0;
        for(uint i=0; i<lgp->parents.size(); ++i)
            if(groups[lgp->parents[i]]->depth > max_depth)
                max_depth=groups[lgp->parents[i]]->depth;

        if(lgp->depth<max_depth)
        {
            lgp->depth=max_depth;
            for(uint i=0; i<lgp->groupChildren.size(); ++i)
                resetDepth(lgp->groupChildren[i] );
        }
    }

    void setUpdateGroup(int index)
    {
        groups[index]->need_update=true;
        update_queue.push(std::make_pair(index,groups[index]->depth));
    }

    void rec_flatten(LachsGroup* g, const Transform &t)
    {
        Transform t2= t*(g->trans);
        for(uint i=0; i < g->geoChildren.size() ; ++i)
        {
            LachsGeometry* geo= geometries[g->geoChildren[i]];
            for(uint j=0; j<geo->primitives->size(); ++j)
            {
                PrimitiveBase p = primitives[(*geo->primitives)[j]]; 
                for(int k=0; k<3; ++k)
                {
                    p.vertices[k]= t2.applyToPointAffine(p.vertices[k]);
                    p.normals[k]= t2.applyToNormalAffine(p.normals[k]).normalize();

                }
                p.normal= t2.applyToNormalAffine(p.normal).normalize();
                flattenPrimitives.push_back(p);
            }
        }

        for(uint i=0; i < g->groupChildren.size() ; ++i)
            rec_flatten(groups[g->groupChildren[i]],t2);
    }

};

RTFACT_NAMESPACE_END

#endif // _RTFACT_LACHSSCENEHANDLER_HPP_INCLUDED
