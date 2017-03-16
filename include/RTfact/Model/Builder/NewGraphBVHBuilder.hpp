/*
 *  NewGraphBVHBuilder.hpp
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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein, Petr Kmoch, Vaclav Krajicek
 */

 #ifndef _RTFACT_NEWGRAPHBVHBUILDER_HPP_INCLUDED
#define _RTFACT_NEWGRAPHBVHBUILDER_HPP_INCLUDED

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


class NewGraphBVHBuilder : public Builder
{

    // ----------------------------------------------------
    // Internal Structures
    // ----------------------------------------------------

    template<class tGroupHandle>
    struct BuildState{
        typedef typename RTfact::Vector<tGroupHandle> t_GroupHandleVector;

        tGroupHandle mGroup;
        t_GroupHandleVector mPath;
        Transform mXfm;
        BBox mBounds;

        BuildState(): mGroup(NULL) {}

        BuildState(tGroupHandle aGroup, const Transform &aXfm, const BBox &aBounds):
                mGroup(aGroup),
                mXfm(aXfm),
                mBounds(aBounds) {}
    };

    struct cmpLess
    {
        bool operator()(std::pair<size_t, float> x, std::pair<size_t, float> y) {
            return x.second < y.second;
        }
    } cmpLessObj;

    // ----------------------------------------------------
    // Variables (for performance measure )
    // ----------------------------------------------------

    uint max_expand;
    TimeMeasurer memoAllocTime;
    TimeMeasurer buildingTime;
    TimeMeasurer transformingTime;
    TimeMeasurer SAHTime;
    TimeMeasurer SAHTimeB;
    TimeMeasurer SAHTimeC;
    TimeMeasurer SAHTimeD;
    TimeMeasurer SAHTimeE;


public:

    NewGraphBVHBuilder() : max_expand(_RTFACT__MAX_EXPAND)
    {}


    // ----------------------------------------------------
    // control Options:
    // ----------------------------------------------------
    void setMaxExpand(uint mx){
        max_expand=mx;
    }
    uint getMaxExpand(){
        return max_expand;
    }

    template <class tScene, class tTree, class tBuildData>
    void build(tTree& aTree, tScene &aScene, tBuildData &aBuildData)
    {
        // Scene Types
        typedef tScene                                  t_Scene;
        typedef typename tScene::t_Group                t_Group;
        typedef typename tScene::t_Instance             t_Instance;
        typedef typename tScene::t_Mesh                 t_Mesh;
        typedef typename Traits<t_Group>::Ptr           t_GroupHandle;
        typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;
        typedef typename Traits<t_Mesh>::Ptr            t_MeshHandle;
        typedef Vector<t_GroupHandle>                   t_GroupVector;
        // Build Types
        typedef BuildState<t_GroupHandle>               t_BuildState;
        typedef typename tBuildData::t_AccelInstance    t_AccelInstance;
        typedef Vector<size_t>                          t_BuildNodesVector;
        typedef Vector<t_BuildState>                    t_BuildGroupsVector;
        typedef Vector<t_AccelInstance>                 t_BuildInstancesVector;

        // Tree Types
        typedef tTree                                   t_Tree;
        typedef typename t_Tree::Iterator               t_TreeNode;
        typedef Vector<t_TreeNode>                      t_TreeNodeVector;
        typedef typename t_Tree::ChildIteratorPair      t_TreeChildIteratorPair;



        // Reset Timers for perfomance Measure
        memoAllocTime.reset();
        buildingTime.reset();
        transformingTime.reset();
        SAHTime.reset();
        SAHTimeB.reset();
        SAHTimeC.reset();
        SAHTimeD.reset();

        memoAllocTime.start();


        //std::cout<<"\n---------- Building Starts ------------";
        // Make sure that we split up the graph for more elements then the maximum to create a leaf.
        assert(max_expand > _RTFACT__MAX_FACES);
        // Set Bounds of the whole scene.
        BBox sceneBounds= aScene.getRoot()->mBounds;
		//std::cout << "Scene Bounds: " << sceneBounds.

        // initialize build data and acceleration structure

        aBuildData.mAccelInstances.resize(aScene.getNumOfInstances());

        aTree.init(aScene.getNumOfInstances() );

        //aBuildData.mAccelInstances.resize(1);
        //aTree.setElementBase(aBuildData.mAccelInstances[0]);
        //aBuildData.mAccelInstances.resize(0);

        if(aBuildData.mAccelInstances.size() == 0){

            return; // Empty scene. We're done
        }


        aTree.setElementBase(aBuildData.mAccelInstances[0]);

//        aTree.setElementBase(aBuildData.mAccelInstances.begin());


        // Build Data

        // stores all groups of the build process.
        t_BuildGroupsVector             buildGroups;
        // stores all instances of the build process.
        t_BuildInstancesVector          buildInstances;
        // stores the depth of the current build state
        t_BuildNodesVector              stackDepth;
        // Two sequential integer define the range of groups corresponding to build state.
        t_BuildNodesVector              stackGroupRange;
        // Two sequential integer define the range of instances corresponding to build state.
        t_BuildNodesVector              stackInstanceRange;
        // stores the tree node of acceleration structure corresponding to build state.
        t_TreeNodeVector                stackNodes;
        // stores the bounds corresponding to build state.
        Vector<BBox>                    stackBounds;


        buildGroups.reserve(aScene.getNumOfInnernodes());
        buildInstances.reserve(aScene.getNumOfInstances());
        stackDepth.reserve(_RTFACT__MAX_DEPTH);
        stackGroupRange.reserve(_RTFACT__MAX_DEPTH);
        stackInstanceRange.reserve(_RTFACT__MAX_DEPTH);
        stackNodes.reserve(_RTFACT__MAX_DEPTH);
        stackBounds.reserve(_RTFACT__MAX_DEPTH);

        // buildGroupsNow and buildInstancesNow are temporary index Container, when expanding the number of
        // instances and groups to be considered. (best for queue behaviour)
        t_BuildGroupsVector         buildGroupsNow;
        t_BuildInstancesVector      buildInstancesNow;


        buildGroupsNow.reserve(max_expand*3/2);
        buildInstancesNow.reserve(max_expand*3/2);

        // Ininitialize Values:

        buildGroups.push_back( t_BuildState(aScene.getRoot(), aScene.getRoot()->mXfm, sceneBounds) );
        stackDepth.push_back(0);
        stackGroupRange.push_back(0);
        stackInstanceRange.push_back(0);
        stackNodes.push_back(aTree.getRoot());
        stackBounds.push_back(sceneBounds);

        // Starting iterativ Build Process:

        size_t depth, group_start, instance_start;
        int group_queue_start;
        int buildDataInstance_i = 0;
        BBox bounds;
        t_TreeNode node;

        // Structures for SAH Computation (preallocated - to be reused)
        Vector< Vector< std::pair<size_t, float> > > sah_split_middle(3);
        Vector< Vector<std::pair<float, float> > > sah_area(3);
        Vector< Vector<size_t> > sah_primitives(3);

        // used for SAH-Stuff...
        Vector<bool> instanceLeft;
        Vector<bool> groupLeft;


        memoAllocTime.stop();
        buildingTime.start();

        while(stackDepth.size()>0){
            // get Stack Content
            depth=stackDepth.back();
            group_start=stackGroupRange.back();
            instance_start=stackInstanceRange.back();
            node=stackNodes.back();
            bounds=stackBounds.back();

            stackDepth.pop_back();
            stackGroupRange.pop_back();
            stackInstanceRange.pop_back();
            stackNodes.pop_back();
            stackBounds.pop_back();

            SAHTimeC.start();
            // Construct build(Groups|Instances)Now
            buildGroupsNow.resize(buildGroups.size()-group_start);
            for (size_t i=group_start; i<buildGroups.size(); ++i){
                buildGroupsNow[i-group_start]=buildGroups[i];
            }
            buildGroups.resize(group_start);

            buildInstancesNow.resize(buildInstances.size()-instance_start);
            for (size_t i=instance_start; i<buildInstances.size(); ++i){
                buildInstancesNow[i-instance_start]=buildInstances[i];
            }
            buildInstances.resize(instance_start);
            SAHTimeC.stop();

            // Now build in this loop until we construct a leaf

            while(true)
            {
                group_queue_start = 0;
                // Max Depth reached => break up all groups
                if(depth > _RTFACT__MAX_DEPTH){
                    while(buildGroupsNow.size()-group_queue_start>0)
                        breakUp(aBuildData, buildGroupsNow, buildInstancesNow, group_queue_start);

                }
                else
                { // break up, until you have enough elements (groups + instances)
                    while( buildGroupsNow.size()-group_queue_start > 0 &&
                    buildGroupsNow.size()-group_queue_start+buildInstancesNow.size()< max_expand )
                        breakUp(aBuildData, buildGroupsNow, buildInstancesNow, group_queue_start);
                }
                //std::cout<<"\n Current Groups: "<<(buildGroupsNow.size()-group_queue_start)<<" / Instances: "<<buildInstancesNow.size();
                // too deep or less enough elements? Create Leaf!
                if(depth > _RTFACT__MAX_DEPTH ||
                (buildGroupsNow.size()-group_queue_start==0 && buildInstancesNow.size() <= _RTFACT__MAX_FACES))
                {
                    //std::cout<<"\n Create Leaf";

                    // First: Add Instances to BuildData
                    typename t_BuildInstancesVector::iterator itStart = aBuildData.mAccelInstances.begin() + buildDataInstance_i;
                    for(size_t i=0; i< buildInstancesNow.size(); i++)
                        aBuildData.mAccelInstances[buildDataInstance_i++] = buildInstancesNow[i];
                    typename t_BuildInstancesVector::iterator itEnd = aBuildData.mAccelInstances.begin() +  buildDataInstance_i;
                    // Second: Create Leaf Node in Acceleration Structure
                    aTree.createLeafNode(node, itStart, itEnd);
                    aTree.setBounds(node,bounds);
                    break; // time to leave this loop

                }
                // --------------- Want to split up the node --------------
                // Determine the splitplane and splitaxis.
                int splitAxis = bounds.getMaximumExtent();

                //std::cout<<"\n Now: Split!";
                // Create ChildNodes

                if(!SAHPartition(aScene, bounds, buildInstancesNow, buildGroupsNow, group_queue_start,
                    splitAxis, instanceLeft, groupLeft,
                    sah_split_middle, sah_area, sah_primitives)){

                    // was not able to find a good partition
                    // expand all remaning Groups (there shouldn't be anymore left...)
                    while(buildGroupsNow.size() - group_queue_start>0)
                        breakUp(aBuildData, buildGroupsNow, buildInstancesNow, group_queue_start);


                    // First: Add Instances to BuildData
                    typename t_BuildInstancesVector::iterator itStart = aBuildData.mAccelInstances.begin() + buildDataInstance_i;
                    for(size_t i=0; i< buildInstancesNow.size(); i++)
                        aBuildData.mAccelInstances[buildDataInstance_i++] = buildInstancesNow[i];
                    typename t_BuildInstancesVector::iterator itEnd = aBuildData.mAccelInstances.begin() +  buildDataInstance_i;
                    // Second: Create Leaf Node in Acceleration Structure
                    aTree.createLeafNode(node, itStart, itEnd);

                    aTree.setBounds(node,bounds);
                    break; // time to leave this loop
                }


                BBox leftBounds = BBox::empty();
                BBox rightBounds = BBox::empty();
                t_TreeChildIteratorPair p = aTree.createInnerNode(node);
                aTree.setBounds(node,bounds);

                // Split up Instances.

                int now_index=0;
                for(size_t i=0; i<buildInstancesNow.size(); ++i)
                {
                    if(instanceLeft[i])
                    {
                        leftBounds.extend(buildInstancesNow[i].mBounds);
                        buildInstancesNow[now_index]=buildInstancesNow[i];
                        ++now_index;
                    }
                    else
                    {
                        rightBounds.extend(buildInstancesNow[i].mBounds);
                        buildInstances.push_back(buildInstancesNow[i]);
                    }
                }
                buildInstancesNow.resize(now_index);

                // Split up Groups.

                now_index=0;
                for(size_t i=group_queue_start; i<buildGroupsNow.size(); ++i)
                {
                    if(groupLeft[i])
                    {
                        leftBounds.extend(buildGroupsNow[i].mBounds);
                        buildGroupsNow[now_index]=buildGroupsNow[i];
                        ++now_index;
                    }
                    else
                    {
                        rightBounds.extend(buildGroupsNow[i].mBounds);
                        buildGroups.push_back(buildGroupsNow[i]);
                    }
                }
                buildGroupsNow.resize(now_index);



                ++depth;
                // update Stack-Information for right-Child:
                stackDepth.push_back(depth);
                stackGroupRange.push_back(group_start);
                stackInstanceRange.push_back(instance_start);
                stackNodes.push_back( p.second );
                stackBounds.push_back(rightBounds);

                //update Local Information for left-Child:
                group_start=buildGroups.size();
                instance_start=buildInstances.size();
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

    bool cmpLess( std::pair<size_t, float> a, std::pair<size_t, float> b) {
        return a.second < b.second;
    }

private:

    template <
        class tBuildData,
        class tGroupHandle,
        class tBuildInstanceVector
        >
    void breakUp(tBuildData &aBuildData,
                 Vector<BuildState<tGroupHandle> > &aGroupVec,
                 tBuildInstanceVector &aInstanceVec,
                 int &aGroupStart)
    {
        typedef tGroupHandle                                t_GroupHandle;
        typedef typename tBuildData::t_AccelInstance        t_AccelInstance;
        typedef BuildState<t_GroupHandle>                   t_BuildState;

        // removes the first group of aGroupVec (defined by groupStart)
        // and adds all it's child instances and groups.
        transformingTime.start();
        t_GroupHandle group = aGroupVec[aGroupStart].mGroup;
        for(size_t i=0; i<group->mInstances.size(); ++i)
        {
            t_AccelInstance inst;
            inst.mStructure= aBuildData.mInnerStructs[group->mInstances[i]->mMesh->mID];
            inst.mXfm = aGroupVec[aGroupStart].mXfm;
            inst.mInstance = &*group->mInstances[i];
            inst.mPath = aGroupVec[aGroupStart].mPath;
            inst.mPath.push_back(group);
            if(inst.mInstance->mMesh->getPrimitiveCount() > 0)
            { // Why should we even bother if mesh is empty?
                BBox oldbounds=group->mInstances[i]->mMesh->mBounds;
                inst.mBounds=inst.mXfm.applyAffine(oldbounds);
                Vec3f<1> old_e= oldbounds.getExtent();
                Vec3f<1> new_e= inst.mBounds.getExtent();

                // Do not add Instances that are completly flattened (and thus invisible)
                if( !((new_e.xf == 0 || new_e.yf == 0 || new_e.zf == 0) &&
                    (old_e.xf!= 0 && old_e.yf != 0 && old_e.zf != 0)) )
                    aInstanceVec.push_back(inst);
            }
        }
        for(size_t i=0; i<group->mGroups.size(); ++i)
        {

            t_BuildState bs;
            bs.mGroup = group->mGroups[i];
            bs.mPath = aGroupVec[aGroupStart].mPath;
            bs.mPath.push_back(group);

            if(!(bs.mGroup->mBounds.isEmpty()))
            {
                bs.mXfm = aGroupVec[aGroupStart].mXfm;
                bs.mBounds=bs.mXfm.applyAffine(bs.mGroup->mBounds);
                bs.mXfm*=(bs.mGroup->mXfm); // right order or invers?

                aGroupVec.push_back(bs);
            }
        }
        transformingTime.stop();
        ++aGroupStart;
    }



     bool cmpGreater( std::pair<size_t, float> a, std::pair<size_t, float> b) {
       return a.second > b.second;
     }

    template <class tScene, class tBuildInstancesVector, class tBuildGroupsVector>
    inline bool SAHPartition(tScene &aScene, const BBox &aBounds, const tBuildInstancesVector &aInstances,
        const tBuildGroupsVector &aGroups, int queue_start,
        int &splitAxis, Vector<bool> &aInstanceLeft, Vector<bool> &aGroupLeft,
        Vector< Vector< std::pair<size_t, float> > > &aSplitMiddle,
        Vector< Vector<std::pair<float, float> > > &aArea,
        Vector< Vector<size_t> > &aPrimitives
        )
    {
        SAHTime.start();
        for(int i=0; i<3; i++)
        {
            aArea[i].resize(aInstances.size()+aGroups.size()-queue_start);
            aPrimitives[i].resize(aInstances.size()+aGroups.size()-queue_start);
        }

        float full_area=aBounds.getArea();
        size_t max_prims=0;
        for(size_t i=0; i<aInstances.size(); ++i)
            max_prims+=aInstances[i].mInstance->mMesh->getPrimitiveCount();
        for(size_t i=queue_start; i<aGroups.size(); ++i)
            max_prims+=aGroups[i].mGroup->mNumOfPrimitives;

        float   best_sam=float(max_prims*_RTFACT__COST_INTERSECT);
        int     best_axis=-1;
        size_t  best_event=0;

        const size_t groupOffset = aInstances.size();


        // Bound-Border-Coordinates for each Dimension:

        for(int a=0; a<3; ++a){
            // Reserve

            aSplitMiddle[a].resize(aInstances.size()+aGroups.size()-queue_start);

            // Insert

            for(size_t i=0; i<aInstances.size(); ++i){
                aSplitMiddle[a][i]=std::make_pair(i,
                    (aInstances[i].mBounds.min.data[a]+aInstances[i].mBounds.max.data[a])/2);
            }
            for(size_t i=queue_start; i<aGroups.size(); ++i){
                aSplitMiddle[a][groupOffset + i - queue_start] =
                    std::make_pair<size_t,float>( groupOffset + i,
                    (aGroups[i].mBounds.min.data[a]+aGroups[i].mBounds.max.data[a])/2);
            }

            // Sort
            sort(aSplitMiddle[a].begin(), aSplitMiddle[a].end(), cmpLessObj);
        }

        SAHTimeD.start();

        for(int a=0; a<3; ++a)
        {
            size_t prims=0;
            BBox bounds = BBox::empty();


            for(size_t i=0; i < aSplitMiddle[a].size(); ++i)
            {
                size_t moved_obj=aSplitMiddle[a][i].first;
                if(moved_obj< groupOffset){
                   bounds.extend(aInstances[moved_obj].mBounds);
                   prims+=aInstances[moved_obj].mInstance->mMesh->getPrimitiveCount();
                }
                else{
                    moved_obj -= groupOffset;
                    bounds.extend(aGroups[moved_obj].mBounds);
                    prims+=aGroups[moved_obj].mGroup->mNumOfPrimitives;
                }
                aArea[a][i].first = bounds.getArea();
                aPrimitives[a][i] = prims;
            }
            bounds = BBox::empty();
            for(size_t i = aSplitMiddle[a].size() ; i > 0; )
            {
                --i;
                size_t moved_obj = aSplitMiddle[a][i].first;

                if(moved_obj < groupOffset){
                   bounds.extend( aInstances[moved_obj].mBounds);
                }
                else{
                    bounds.extend( aGroups[moved_obj - groupOffset].mBounds);
                }
                aArea[a][i].second=bounds.getArea();
                // Calculate SAM:
                float sam=(aArea[a][i].first/full_area*aPrimitives[a][i]
                + aArea[a][i].second/full_area*(max_prims-aPrimitives[a][i]))
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

        aInstanceLeft.resize(aInstances.size());
        aGroupLeft.resize(aGroups.size());

        for(size_t i=0; i<aInstanceLeft.size(); i++)
            aInstanceLeft[i]=false;
        for(size_t i=0; i<aGroupLeft.size(); i++)
            aGroupLeft[i]=false;

        for(size_t i=0; i< best_event+1; i++){
            size_t left_obj=aSplitMiddle[best_axis][i].first;

            if(left_obj< groupOffset){
               aInstanceLeft[left_obj]=true;
            }
            else{
               aGroupLeft[left_obj - groupOffset]=true;
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
