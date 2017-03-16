/* 
 *  BVHDebugOpenGLRenderer.hpp
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
 *  Created on: 2009-02-28 01:27:43 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__BVHOPENGLDEBUGRENDERER_HPP
#define RTFACT__BVHOPENGLDEBUGRENDERER_HPP

#include <RTfact/Config/Common.hpp>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <RTfact/Utils/BBox.hpp>

#include <RTfact/Concept/Renderer.hpp>

#define RTFACTDEMO__BACKGROUND_COLOR4 0.f, 0.f, 0.f, 0.f

#define RTFACTDEMO__LEFT_COLOR  .6f, .1f, .6f
#define RTFACTDEMO__RIGHT_COLOR .1f, .6f, .6f
#define RTFACTDEMO__LEAF_COLOR  .6f, .6f, .1f


#define RTFACTDEMO__LEFT_TRANSPARENCY  .7f
#define RTFACTDEMO__RIGHT_TRANSPARENCY .7f
#define RTFACTDEMO__LEAF_TRANSPARENCY  .7f

#define RTFACTDEMO__LINE_WIDTH        1.f
#define RTFACTDEMO__LEFT_LINE_WIDTH   8.f
#define RTFACTDEMO__RIGHT_LINE_WIDTH  6.f
#define RTFACTDEMO__LEAF_LINE_WIDTH   4.f

#define RTFACTDEMO__LIGHT_INTENSITY4 0.2f, 0.2f, 0.2f, 1.f
#define RTFACTDEMO__LIGHT_POSITION4  0.f, 0.f, 0.f, 1.f

RTFACT_NAMESPACE_BEGIN

class BVHOpenGLDebugRenderer :
    public Renderer
{
    Vec3f1 mBoxColors[3];
    float mBoxTransparencies[3];
    float mLineWidths[3];

public:

    BVHOpenGLDebugRenderer()
    {
        mBoxColors[0] = Vec3f1(RTFACTDEMO__LEFT_COLOR);
        mBoxColors[1] = Vec3f1(RTFACTDEMO__RIGHT_COLOR);
        mBoxColors[2] = Vec3f1(RTFACTDEMO__LEAF_COLOR);

        mBoxTransparencies[0] = RTFACTDEMO__LEFT_TRANSPARENCY;
        mBoxTransparencies[1] = RTFACTDEMO__RIGHT_TRANSPARENCY;
        mBoxTransparencies[2] = RTFACTDEMO__LEAF_TRANSPARENCY;

        mLineWidths[0] = RTFACTDEMO__LEFT_LINE_WIDTH;
        mLineWidths[1] = RTFACTDEMO__RIGHT_LINE_WIDTH;
        mLineWidths[2] = RTFACTDEMO__LEAF_LINE_WIDTH;
    }

    template<
        class tCamera,
        class tScene>
    void init(
        tCamera& aCamera,
        tScene& aScene)
    {}

    template<
        class tCamera,
        class tScene,
        class tBVH>
    void render(
        tCamera& aCamera,
        tScene& aScene,
        tBVH& aBVH,
        uint aLevel = 1)
    {
        typename tCamera::Params camParams;

        aCamera.getParams(camParams);

        glViewport(
            0,
            0,
            camParams.resX,
            camParams.resY);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(
            camParams.openingAngle,
            (GLfloat) camParams.resX / (GLfloat) camParams.resY,
            camParams.nearPlane, camParams.farPlane);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gluLookAt(
            camParams.position.x,
            camParams.position.y,
            camParams.position.z,
            camParams.position.x + camParams.direction.x,
            camParams.position.y + camParams.direction.y,
            camParams.position.z + camParams.direction.z,
            camParams.up.x,
            camParams.up.y,
            camParams.up.z);

        glShadeModel(GL_SMOOTH);

        glClearColor(RTFACTDEMO__BACKGROUND_COLOR4);

        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glEnable(GL_LIGHTING);

        glEnable(GL_LIGHT0);

        GLfloat lightIntensity[] = { RTFACTDEMO__LIGHT_INTENSITY4};
        GLfloat lightPosition[] = { RTFACTDEMO__LIGHT_POSITION4};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glLineWidth(RTFACTDEMO__LINE_WIDTH);

        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        //glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        glColor4f(1.f, .7f, .7f, 0.25f);

        for(uint i = 0; i < 3; ++i)
        {
            switch(i)
            {
            case 0:
                continue;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);               
                break;
            case 1:
                continue;
                glCullFace(GL_BACK);
                break;
            case 2:
                glClearDepth(1.0f);
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor4f(1.f, 1.f, 1.f, .5f);
                break;
            }

            glBegin(GL_TRIANGLES);

            for(typename tScene::PrimitiveVector::const_iterator
                    triangleIt = aScene.primitives.begin();
                triangleIt != aScene.primitives.end(); ++triangleIt)
            {
                for(uint vertexIdx = 0; vertexIdx < 3; ++vertexIdx)
                {
                    glNormal3f(triangleIt->normals[vertexIdx].x,
                               triangleIt->normals[vertexIdx].y,
                               triangleIt->normals[vertexIdx].z);

                    glVertex3f(triangleIt->vertices[vertexIdx].x,
                               triangleIt->vertices[vertexIdx].y,
                               triangleIt->vertices[vertexIdx].z);
                }
            }

            glEnd();
        }

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        drawBounds(aScene, aBVH, aBVH.getRoot(), aLevel);
    }

    template<
        class tScene,
        class tBVH>
    void drawBounds(
        tScene& aScene,
        tBVH& aBVH,
        typename tBVH::Iterator aIt,
        uint aDesiredLevel,
        uint aCurrentLevel = 0,
        uint aChildIndex = 0)
    {
        if(aDesiredLevel == aCurrentLevel)
        {
            glClearDepth(1.f);

            const BBox& bounds = aBVH.getBounds(aIt);

            glDisable(GL_CULL_FACE);

            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
            glEnable(GL_COLOR_MATERIAL);

            glLineWidth(mLineWidths[aChildIndex]);

            if(aBVH.isLeaf(aIt))
            {
                aChildIndex = 2;
            }

            glColor4f(
                mBoxColors[aChildIndex].x,
                mBoxColors[aChildIndex].y,
                mBoxColors[aChildIndex].z,
                mBoxTransparencies[aChildIndex]);

            glBegin(GL_QUADS);

            drawBox(bounds);

            glEnd();
        }
        else if(!aBVH.isLeaf(aIt))
        {
            typename tBVH::Iterator leftChild, rightChild;

            aBVH.getChildren(aIt, leftChild, rightChild);

            drawBounds(aScene, aBVH, leftChild, aDesiredLevel, aCurrentLevel + 1, 0);

            drawBounds(aScene, aBVH, rightChild, aDesiredLevel, aCurrentLevel + 1, 1);
        }
    }

    void drawBox(
        const BBox& aBounds)
    {
        glVertex3f(aBounds.min.x, aBounds.min.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.min.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.max.y, aBounds.min.z);
        glVertex3f(aBounds.min.x, aBounds.max.y, aBounds.min.z);

        glVertex3f(aBounds.min.x, aBounds.min.y, aBounds.max.z);
        glVertex3f(aBounds.max.x, aBounds.min.y, aBounds.max.z);
        glVertex3f(aBounds.max.x, aBounds.max.y, aBounds.max.z);
        glVertex3f(aBounds.min.x, aBounds.max.y, aBounds.max.z);

        glVertex3f(aBounds.min.x, aBounds.min.y, aBounds.min.z);
        glVertex3f(aBounds.min.x, aBounds.max.y, aBounds.min.z);
        glVertex3f(aBounds.min.x, aBounds.max.y, aBounds.max.z);
        glVertex3f(aBounds.min.x, aBounds.min.y, aBounds.max.z);

        glVertex3f(aBounds.max.x, aBounds.min.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.max.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.max.y, aBounds.max.z);
        glVertex3f(aBounds.max.x, aBounds.min.y, aBounds.max.z);

        glVertex3f(aBounds.min.x, aBounds.min.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.min.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.min.y, aBounds.max.z);
        glVertex3f(aBounds.min.x, aBounds.min.y, aBounds.max.z);

        glVertex3f(aBounds.min.x, aBounds.max.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.max.y, aBounds.min.z);
        glVertex3f(aBounds.max.x, aBounds.max.y, aBounds.max.z);
        glVertex3f(aBounds.min.x, aBounds.max.y, aBounds.max.z);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__BVHOPENGLDEBUGRENDERER_HPP
