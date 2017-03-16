/* 
 *  GraphBVHBuilder.hpp
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
 *  Created on: 2008-06-27 11:29:41 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef _RTFACT_LACHSGRAPHBVHBUILDER_HPP_INCLUDED
#define _RTFACT_LACHSGRAPHBVHBUILDER_HPP_INCLUDED

#include <RTfact/Concept/Builder.hpp>
#include <RTfact/Model/Primitive/GraphBVH.hpp>
#include <RTfact/Model/Scene/GraphScene.hpp>
#include <queue>
#include <boost/lambda/bind.hpp>

#include <RTfact/Utils/System/TimeMeasurer.hpp>

RTFACT_NAMESPACE_BEGIN

#define _RTFACT__MAX_DEPTH 40
#define _RTFACT__MAX_FACES 1
#define _RTFACT__MAX_EXPAND 1024    

#define _RTFACT__COST_TRAVERSAL 1
#define _RTFACT__COST_INTERSECT 2

template <class ta_innerStruct, class ta_innerBuilder >
class GraphBVHBuilder : public Builder
{
    bool m_onceSkipped;
    int max_expand;
    TimeMeasurer memoAllocTime;
    TimeMeasurer buildingTime;
    TimeMeasurer transformingTime;
    TimeMeasurer SAHTime;
    TimeMeasurer SAHTimeB;
    TimeMeasurer SAHTimeC;
    TimeMeasurer SAHTimeD;
    TimeMeasurer SAHTimeE;


    struct BuildState{
        int index;
        Transform trans;
        BBox bounds;

        BuildState(): index(-1) {}
        BuildState(int i, const Transform &t, const BBox &b): index(i),trans(t),bounds(b) {}
    };

    struct cmpLess
    {    
        bool operator()(std::pair<int, float> x, std::pair<int, float> y) {
            return x.second < y.second;
        }
    } cmpLessObj; 

//    typedef std::priority_queue< std::pair<int,float> , Vector< std::pair<int,float> > , compSmaller > smaller_queue;
//    typedef std::priority_queue< std::pair<int,float> , Vector< std::pair<int,float> > , compGreater > greater_queue;

       typedef Vector<size_t> t_buildNodesVector;
    typedef Vector<BuildState> t_buildGroupsVector;
       typedef typename GraphBVH<ta_innerStruct>::BuildInstanceVector t_buildGeosVector;
       typedef typename GraphBVH<ta_innerStruct>::BuildInstance Instance;
       typedef GraphBVH<ta_innerStruct>  MyStruct;




public:




    GraphBVHBuilder() : m_onceSkipped(false), max_expand(_RTFACT__MAX_EXPAND)
    {}

    // control Options:

    void setMaxExpand(int mx){
        max_expand=mx;
    }
    int getMaxExpand(){
        return max_expand;
    }

    template <class LSH>
    void build(MyStruct &aTree, LSH& lsh)
    {
        memoAllocTime.reset();
        buildingTime.reset();
        transformingTime.reset();
        SAHTime.reset();
        SAHTimeB.reset();
        SAHTimeC.reset();
        SAHTimeD.reset();
           
        if(false)
        {
            m_onceSkipped = true;
        }
        else
        {
           
            memoAllocTime.start();


            //std::cout<<"\n---------- Building Starts ------------";
            // Make sure that we split up the graph for more elements then the maximum to create a leaf.
            assert(max_expand > _RTFACT__MAX_FACES);
            // Set Bounds of the whole scene.
            BBox sceneBounds= lsh.groups[lsh.root]->bounds;
            aTree.init(lsh);


            ta_innerBuilder builder;


            // Create inner acceleration structure for each geometrie.
            for(int i=0; i<lsh.geometries.size(); i++){
                if(lsh.geometries[i] && lsh.geometries[i]->need_build_update)
                {
                    lsh.geometries[i]->need_build_update=false;
                    ta_innerStruct *inner=aTree.getInnerStruct(i);
                    Vector< typename LSH::PrimitiveBase* > itArray;
                    for( typename LSH::PrimitiveMap::key_vector::iterator it = lsh.geometries[i]->primitives->begin();
                        it != lsh.geometries[i]->primitives->end(); it++)
                        itArray.push_back(&(lsh.primitives[*it]));
                    builder.build(*inner, &lsh.primitives.begin(), 
                        itArray.begin(),itArray.end(),
                        boost::lambda::bind(
                            &LSH::PrimitiveBase::getBounds,
                            boost::lambda::_1));
                }
                else
                {
                    // always: Update Elementbase. This can change, whenever any geometry changes.
                    aTree.getInnerStruct(i)->setElementBase(*lsh.primitives.begin());
                }
                    
            }


            t_buildGroupsVector buildGroups;
            buildGroups.reserve(lsh.getNumOfInnernodes());
            t_buildGeosVector buildGeos;
            //Vector<BBox> geoBounds;

            buildGeos.reserve(lsh.getNumOfInstances());
            //geoBounds.reserve(lsh.getNumOfInstances());
            
            // build Stacks containing depth and index-ranges of buildGroups and buildGeos for the right Child-Nodes
            // The Range is always determined by the start Element, the last element of the Range is always
            // the last Element of buildGroups/Geos
            t_buildNodesVector stackDepth;
            t_buildNodesVector stackGroupRange;
            t_buildNodesVector stackGeoRange;

            Vector<typename MyStruct::Iterator> stackNodes;
            Vector<BBox> stackBounds;
            

            stackDepth.reserve(_RTFACT__MAX_DEPTH);
            stackGroupRange.reserve(_RTFACT__MAX_DEPTH);
            stackGeoRange.reserve(_RTFACT__MAX_DEPTH);
            stackNodes.reserve(_RTFACT__MAX_DEPTH);
            stackBounds.reserve(_RTFACT__MAX_DEPTH);

            // buildGroupsNow and buildGeosNow are temporary index Container, when expanding the number of
            // geometries and groups to be considered. (best for queue behaviour)
            t_buildGroupsVector buildGroupsNow;
            buildGroupsNow.reserve(max_expand*3/2);
            
            t_buildGeosVector buildGeosNow;
            buildGeosNow.reserve(max_expand*3/2);

            //Vector<BBox> geoBoundsNow;
            //geoBoundsNow.reserve(max_expand*3/2);
            
            BuildState bs(lsh.root, lsh.groups[lsh.root]->trans, sceneBounds);
            buildGroups.push_back(bs);

            stackDepth.push_back(0);
            stackGroupRange.push_back(0);
            stackGeoRange.push_back(0);
            stackNodes.push_back(aTree.getRoot());
            stackBounds.push_back(sceneBounds);
            // Starting iterativ Build Process:
            
            int depth, group_start, geo_start;
            int group_queue_start;
            BBox bounds;
            typename MyStruct::Iterator node;

            // Structures for SAH Computation (initialising localy too expensive)
            Vector< Vector< std::pair<int, float> > > sah_split_middle(3);
            Vector< Vector<std::pair<float, float> > > sah_area(3);
            Vector< Vector<int> > sah_primitives(3);

            // used for SAH-Stuff...
            Vector<bool> geoLeft;
            Vector<bool> groupLeft;


            memoAllocTime.stop();
            buildingTime.start();

            while(stackDepth.size()>0){
                // get Stack Content
                depth=stackDepth.back();
                group_start=stackGroupRange.back();
                geo_start=stackGeoRange.back();
                node=stackNodes.back();
                bounds=stackBounds.back();

                stackDepth.pop_back();
                stackGroupRange.pop_back();
                stackGeoRange.pop_back();
                stackNodes.pop_back();
                stackBounds.pop_back();

                SAHTimeC.start();
                // Construct build(Groups|Geos)Now
                buildGroupsNow.resize(buildGroups.size()-group_start);
                for (int i=group_start; i<buildGroups.size(); ++i){
                    buildGroupsNow[i-group_start]=buildGroups[i];    
                }
                buildGroups.resize(group_start);
                
                buildGeosNow.resize(buildGeos.size()-geo_start);
                for (int i=geo_start; i<buildGeos.size(); ++i){
                    buildGeosNow[i-geo_start]=buildGeos[i];    
                }
                buildGeos.resize(geo_start);
                SAHTimeC.stop();
                
                //geoBoundsNow.resize(geoBounds.size()-geo_start);
                //for (int i=geo_start; i<geoBounds.size(); ++i){
                //    geoBoundsNow[i-geo_start]=geoBounds[i];    
                //}
                //geoBounds.resize(geo_start);

                // Now build in this loop until we construct a leaf
                
                while(true)
                {
                    //std::cout<<"\n Depth: "<<depth;
                    //std::cout<<"\n Current Groups: "<<buildGroupsNow.size()<<" / Geometries: "<<buildGeosNow.size();
                    //std::cout<<"\n Expanding...";
                    group_queue_start = 0;
                    // Max Depth reached => break up all groups
                    if(depth > _RTFACT__MAX_DEPTH){
                        while(buildGroupsNow.size()-group_queue_start>0)
                            breakUp(lsh, buildGroupsNow, buildGeosNow, group_queue_start);
                        
                    }
                    else
                    { // break up, until you have enough elements (groups + geometries)
                        while(buildGroupsNow.size()-group_queue_start>0 &&
                        buildGroupsNow.size()-group_queue_start+buildGeosNow.size()< max_expand )
                            breakUp(lsh, buildGroupsNow, buildGeosNow, group_queue_start);
                    }
                    //std::cout<<"\n Current Groups: "<<(buildGroupsNow.size()-group_queue_start)<<" / Geometries: "<<buildGeosNow.size();
                    // too deep or less enough elements? Create Leaf!
                    if(depth > _RTFACT__MAX_DEPTH || 
                    (buildGroupsNow.size()-group_queue_start==0 && buildGeosNow.size() <= _RTFACT__MAX_FACES))
                    {
                        //std::cout<<"\n Create Leaf";
                        
                        aTree.createLeafNode(node, buildGeosNow.begin(), buildGeosNow.end());
                        
                        aTree.setBounds(node,bounds);
                        break; // time to leave this loop

                    }
                    // --------------- Want to split up the node --------------                
                    // Determine the splitplane and splitaxis.
                    int splitAxis = bounds.getMaximumExtent();
                    
                    //std::cout<<"\n Now: Split!";
                    // Create ChildNodes

                    if(!SAHPartition(lsh, bounds, buildGeosNow, buildGroupsNow, group_queue_start, 
                        splitAxis, geoLeft, groupLeft,
                        sah_split_middle, sah_area, sah_primitives)){
                        // was not able to find a good partition
                        // expand all remaning Groups (there shouldn't be anymore left...)
                        while(buildGroupsNow.size()-group_queue_start>0)
                            breakUp(lsh, buildGroupsNow, buildGeosNow, group_queue_start);
                        // create leaf.
                        aTree.createLeafNode(node, buildGeosNow.begin(), buildGeosNow.end());
                        aTree.setBounds(node,bounds);
                        break; // time to leave this loop
                    }


                    BBox leftBounds = BBox::empty();
                    BBox rightBounds = BBox::empty();
                    typename MyStruct::ChildIteratorPair p=aTree.createInnerNode(node);
                    aTree.setBounds(node,bounds);
                    
                    // Split up Geometries.
                    
                    int now_index=0;
                    for(int i=0; i<buildGeosNow.size(); ++i)
                    {
                        if(geoLeft[i])
                        {
                            leftBounds.extend(buildGeosNow[i].bounds);
                            buildGeosNow[now_index]=buildGeosNow[i];
                            ++now_index;
                        }
                        else
                        {
                            rightBounds.extend(buildGeosNow[i].bounds);
                            buildGeos.push_back(buildGeosNow[i]);
                        }
                    }
                    buildGeosNow.resize(now_index);
                        
                    // Split up Groups.

                    now_index=0;
                    for(int i=group_queue_start; i<buildGroupsNow.size(); ++i)
                    {
                        if(groupLeft[i])
                        {
                            leftBounds.extend(buildGroupsNow[i].bounds);
                            buildGroupsNow[now_index]=buildGroupsNow[i];
                            ++now_index;
                        }
                        else
                        {
                            rightBounds.extend(buildGroupsNow[i].bounds);
                            buildGroups.push_back(buildGroupsNow[i]);
                        }
                    }
                    buildGroupsNow.resize(now_index);
                    

                    
                    ++depth;
                    // update Stack-Information for right-Child:
                    stackDepth.push_back(depth);
                    stackGroupRange.push_back(group_start);
                    stackGeoRange.push_back(geo_start);
                    stackNodes.push_back( p.second );
                    stackBounds.push_back(rightBounds);

                    //update Local Information for left-Child:
                    group_start=buildGroups.size();
                    geo_start=buildGeos.size();
                    node=p.first;
                    bounds=leftBounds;
                }
                //std::cout<<"\n Now: Jump up!";
                

            }

            aTree.finalizeBuild();

            buildingTime.stop();
            //std::cout<<"\nBuild Complete. Time Result: ";
            //std::cout<<"\nTime to Allocate: "<<memoAllocTime.getTotalTime();
            //std::cout<<"\nTime to Build: "<<buildingTime.getTotalTime();
            //std::cout<<"\nTime to Tranform: "<<transformingTime.getTotalTime();
            //std::cout<<"\nTime for SAH    : "<<SAHTime.getTotalTime();
            //std::cout<<"\nTime for SAH (B): "<<SAHTimeB.getTotalTime();
            //std::cout<<"\nTime for SAH (C): "<<SAHTimeC.getTotalTime();
            //std::cout<<"\nTime for SAH (D): "<<SAHTimeD.getTotalTime();
            
    }
    }

    bool cmpLess( std::pair<int, float> a, std::pair<int, float> b) {
        return a.second < b.second;
    }       

private:

    template <class LSH>
    void breakUp(LSH &lsh, t_buildGroupsVector &groupVec, t_buildGeosVector &geoVec, int &groupStart){

        // removes the first group of groupVec (defined by groupStart) and adds all it's child geometries and groups.
        transformingTime.start();
        for(int i=0; i<lsh.groups[groupVec[groupStart].index]->geoChildren.size(); ++i)
        {
            Instance inst;
            inst.index=lsh.groups[groupVec[groupStart].index]->geoChildren[i];
            inst.trans=groupVec[groupStart].trans;
            BBox oldbounds=lsh.geometries[inst.index]->bounds;
            inst.bounds=inst.trans.applyAffine(oldbounds);
            Vec3f<1> old_e= oldbounds.getExtent();
            Vec3f<1> new_e= inst.bounds.getExtent();
            if( !((new_e.xf == 0 || new_e.yf == 0 || new_e.zf == 0) && (old_e.xf!= 0 && old_e.yf != 0 && old_e.zf != 0)) )
            geoVec.push_back(inst);


        }
        for(int i=0; i<lsh.groups[groupVec[groupStart].index]->groupChildren.size(); ++i)
        {

            BuildState bs;
            bs.index=lsh.groups[groupVec[groupStart].index]->groupChildren[i];
            if(!(lsh.groups[bs.index]->bounds).isEmpty())
            {
                bs.trans=groupVec[groupStart].trans;
                bs.bounds=bs.trans.applyAffine(lsh.groups[bs.index]->bounds);
                bs.trans*=(lsh.groups[bs.index]->trans); // right order or invers?

                groupVec.push_back(bs);
            }
        }
        transformingTime.stop();
        ++groupStart;            
    }

        

     bool cmpGreater( std::pair<int, float> a, std::pair<int, float> b) {
       return a.second > b.second;
     }     

    template <class LSH>
    inline bool SAHPartition(LSH& lsh, const BBox &b, const t_buildGeosVector &geos,
        const t_buildGroupsVector &groups, int queue_start,
        int &splitAxis, Vector<bool> &geoLeft, Vector<bool> &groupLeft,
        Vector< Vector< std::pair<int, float> > > &split_middle,
        Vector< Vector<std::pair<float, float> > > &area,
        Vector< Vector<int> > &primitives        
        )
    {
        SAHTime.start();
        for(int i=0; i<3; i++)
        {
            area[i].resize(geos.size()+groups.size()-queue_start);
            primitives[i].resize(geos.size()+groups.size()-queue_start);
        }
        
        float full_area=b.getArea();
        int max_prims=0;
        for(int i=0; i<geos.size(); ++i)
            max_prims+=lsh.geometries[geos[i].index]->primitives->size();
        for(int i=queue_start; i<groups.size(); ++i)
            max_prims+=lsh.groups[groups[i].index]->numOfPrimitives;
        
        float best_sam=max_prims*_RTFACT__COST_INTERSECT;
        int best_axis=-1; 
        int best_event=-1;
        
        
        // Bound-Border-Coordinates for each Dimension:
        
        for(int a=0; a<3; ++a){
            // Reserve
            
            split_middle[a].resize(geos.size()+groups.size()-queue_start);
            
            
            
            
            //split_left[a].reserve(geoBounds.size()+groups.size()-queue_start);
            //split_right[a].reserve(geoBounds.size()+groups.size()-queue_start);
            // Insert
            
            for(int i=0; i<geos.size(); ++i){
                split_middle[a][i]=std::make_pair<int,float>(i,(geos[i].bounds.min.data[a]+geos[i].bounds.max.data[a])/2);
                //split_left[a].push_back(std::makepair<int,float>(i,geoBounds[i].min[a]));
                //split_right[a].push_back(std::makepair<int,float>(i,geoBounds[i].max[a]));
            }
            for(int i=queue_start; i<groups.size(); ++i){
                split_middle[a][geos.size()+i -queue_start] = std::make_pair<int,float>(geos.size()+i,(groups[i].bounds.min.data[a]+groups[i].bounds.max.data[a])/2);
                //split_left[a].push_back(std::makepair<int,float>(geoBounds.size()+i,groups[i].bounds.min[a]);
                //split_right[a].push_back(std::makepair<int,float>(geoBounds.size()+i,groups[i].bounds.max[a]);
            }
            
            // Sort
            sort(split_middle[a].begin(), split_middle[a].end(), cmpLessObj);
            //sort(split_left[a].begin(), split_left[a].end(), cmpLess);
            //sort(split_right[a].begin(), split_right[a].end(), cmpGreater);
        }
        
        SAHTimeD.start();

        for(int a=0; a<3; ++a)
        {
            int prims=0;
            BBox bounds = BBox::empty();


            for(int i=0; i < split_middle[a].size(); ++i)
            {   
                int moved_obj=split_middle[a][i].first;
                if(moved_obj< geos.size()){
                   bounds.extend(geos[moved_obj].bounds);
                   prims+=lsh.geometries[geos[moved_obj].index]->primitives->size();
                }
                else{
                    bounds.extend(groups[moved_obj-geos.size()].bounds);
                    prims+=lsh.groups[groups[moved_obj-geos.size()].index]->numOfPrimitives;
                }
                area[a][i].first=bounds.getArea();
                primitives[a][i]=prims;
            }
            bounds = BBox::empty();
            for(int i=split_middle[a].size()-1; i >= 0; --i)
            {   
                int moved_obj=split_middle[a][i].first;
                if(moved_obj< geos.size()){
                   bounds.extend(geos[moved_obj].bounds);
                }
                else{
                    bounds.extend(groups[moved_obj-geos.size()].bounds);
                }
                area[a][i].second=bounds.getArea();
                // Calculate SAM:
                float sam=(area[a][i].first/full_area*primitives[a][i] 
                + area[a][i].second/full_area*(max_prims-primitives[a][i]))
                *_RTFACT__COST_INTERSECT + _RTFACT__COST_TRAVERSAL;
                if(sam < best_sam)
                {
                    best_sam=sam;
                    best_event=i;
                    best_axis=a;
                }

            }
        }
        SAHTimeD.stop();
        SAHTime.stop();
        if(best_axis==-1)
            return false;
        SAHTime.start();
        splitAxis=best_axis;
        geoLeft.resize(geos.size());
        groupLeft.resize(groups.size());
        for(int i=0; i<geoLeft.size(); i++)
            geoLeft[i]=false;
        for(int i=0; i<groupLeft.size(); i++)
            groupLeft[i]=false;
        
        for(int i=0; i<best_event+1; i++){
            int left_obj=split_middle[best_axis][i].first;

            if(left_obj< geos.size()){
               geoLeft[left_obj]=true;
            }
            else{
               groupLeft[left_obj - geos.size()]=true;
            }
        }
        SAHTime.stop();
        return true;
        
    }

};

RTFACT_NAMESPACE_END

#undef _RTFACT__MAX_DEPTH
#undef _RTFACT__MAX_FACES

#endif // _RTFACT_LACHSGRAPHBVHBUILDER_HPP_INCLUDED
