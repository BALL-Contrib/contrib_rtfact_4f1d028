/* 
 *  OpenGLRenderer.hpp
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
 *  Created on: 2008-11-16 20:03:17 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__OPENGLRENDERER_HPP
#define RTFACT__OPENGLRENDERER_HPP

#include <RTfact/Config/Common.hpp>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <RTfact/Utils/Transform.hpp>

#include <RTfact/Concept/Renderer.hpp>

RTFACT_NAMESPACE_BEGIN

class OpenGLRenderer :
    public Renderer
{
public:

    template<
        class tCamera,
        class tScene>
    void init(
        tCamera& aCamera,
        tScene& aScene)
    {}

    template<
        class tCamera,
        class tScene>
    void render(
        tCamera& aCamera,
        tScene& aScene)
    {
        typename tCamera::Params camParams;

        aCamera.getParams(camParams);

        glShadeModel(GL_SMOOTH);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glViewport(
            0,
            0,
            camParams.resX,
            camParams.resY);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_TRIANGLES);

        typename tScene::PrimitiveVector::const_iterator triangleIt;
        
        for(triangleIt = aScene.primitives.begin();
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
};

RTFACT_NAMESPACE_END

#endif // RTFACT__OPENGLRENDERER_HPP
