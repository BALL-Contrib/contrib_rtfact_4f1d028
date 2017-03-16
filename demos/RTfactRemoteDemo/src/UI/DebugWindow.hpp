/* 
 *  DebugWindow.hpp
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
 *  Created on: 2010-08-21 21:49:13 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT_REMOTE__DEBUGWINDOW_HPP
#define RTFACT_REMOTE__DEBUGWINDOW_HPP

#include <RTremote/Config/Common.hpp>

#include <cstdlib>
#include <sstream>

RTFACT_REMOTE_NAMESPACE_BEGIN

template<class t_sceneHandler, class t_accelStruct>
class DebugWindow
{
    SDL_Surface *screen;
    //TTF_Font* font;
    SDL_Event event;
    int width;
    int height;

    t_sceneHandler *sceneHandler;
    t_accelStruct *aStruct;

    // View Parameter;
    float zoom;
    float pos_x;
    float pos_z;
    int aim_depth;
    bool showSceneGraph;
    bool showAccelStruct;
    int rotation;

    bool finished;

    bool startdrag;
    bool wait;
    int drag_x, drag_y;

public:

    DebugWindow(int w = 640, int h = 480)
    {
        std::cout<<"Made DebugWindow!\n";
        aStruct=RTFACT_NULL;
        sceneHandler=RTFACT_NULL;

        width=w;
        height=h;
        zoom=5.0;
        pos_x=0;
        pos_z=0;
        rotation = 0;
        finished=false;
        showSceneGraph=true;
        showAccelStruct=true;
        aim_depth=0;
        //TTF_Init();
        //initFont("arial.ttf",12);
        startdrag=false;
    }


    void setSceneHandler(t_sceneHandler &aSH){
        sceneHandler= &aSH;
    }
    void setAccelStruct(t_accelStruct &aAS){
        aStruct= &aAS;
    }

    void setZoom(float z){
        zoom=z;
    }

    void setPosition(float x, float z){
        pos_x=x;
        pos_z=z;
    }

    int open(){
        screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF);
        if (screen == RTFACT_NULL) {
            printf("Unable to set video mode: %s\n", SDL_GetError());
            return 1;
        }
        return 0;
    }



    void updateDisplay(bool doWait = false)
    {
        if(sceneHandler) sceneHandler->update();
        startWait();
        while(wait && !finished)
        {
            prePaint();
            if(isShowAccelStruct())
                drawAccelStruct(-1, BBox::empty(),0);
            if(isShowSceneGraph())
                drawSceneGraph(-1, Transform::identity(), BBox::empty(), 0);
            postPaint();
            handleEvents();
            if(!doWait) wait=false;
        }
    }

    void startWait(){
        wait=true;
    }




    void handleEvents()
    {
        while(SDL_PollEvent(&event))
        {
              switch (event.type)
              {
                  case SDL_KEYUP:
                  {
                      switch (event.key.keysym.sym)
                      {
                          case SDLK_1:
                          aim_depth++;
                          break;
                          case SDLK_2:
                              aim_depth= (aim_depth>0 ? aim_depth-1 : 0);
                          break;
                          case SDLK_a:
                            showAccelStruct=!showAccelStruct;
                          break;
                          case SDLK_r:
                            rotation = (rotation +1) %4;
                          break;
                          case SDLK_g:
                            showSceneGraph=!showSceneGraph;
                          break;
                          case SDLK_RETURN:
                            wait=false;
                          break;
                      }
                      break;
                  }
                  case SDL_MOUSEBUTTONDOWN:
                      {
                          switch(event.button.button)
                          {
                             case SDL_BUTTON_LEFT:
                                 startdrag=true;
                             break;
                          }
                          break;
                      }
                  case SDL_MOUSEBUTTONUP:
                  {
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            startdrag=false;
                            break;
                        case SDL_BUTTON_MIDDLE:
                            break;
                        case SDL_BUTTON_RIGHT:
                            break;
                        case SDL_BUTTON_WHEELUP:
                            zoom/=1.1;
                            break;
                        case SDL_BUTTON_WHEELDOWN:
                            zoom*=1.1;
                            break;
                    }
                    break;

                  }
                  case SDL_MOUSEMOTION:
                  {
                     //std::cout<<"\ndrag...  "<<event.motion.xrel<<" - "<<event.motion.yrel;
                    if(startdrag)
                    {
                        //std::cout<<"\nDRAG! O__o!  "<<event.motion.xrel<<" - "<<event.motion.yrel;
                        int m_x=(event.motion.xrel*1.0/zoom);
                        int m_y=(event.motion.yrel*1.0/zoom);
                        int v1=m_x/2 + m_y;
                        int v2=m_y - m_x/2;
                        if(rotation == 0){
                            pos_x+=v1;
                            pos_z+=v2;
                        }else if(rotation == 1){
                            pos_x+=v2;
                            pos_z-=v1;
                        }else if(rotation == 2){
                            pos_x-=v1;
                            pos_z-=v2;
                        }else if(rotation == 3){
                            pos_x-=v2;
                            pos_z+=v1;
                        }
                    }
                    break;
                  }
                  case SDL_QUIT:
                  finished = true;
                  break;
              }
        }
    }

    bool isFinished(){
        return finished;
    }

    bool isShowAccelStruct(){
        return showAccelStruct && aStruct!=RTFACT_NULL;
    }
    bool isShowSceneGraph(){
        return showSceneGraph && sceneHandler!=RTFACT_NULL;
    }

private:

    /*void initFont(const std::string& aPath, int aFontSize)
    {
        // open the font
        font = TTF_OpenFont(aPath.c_str(), aFontSize);

        assert(font != RTFACT_NULL);
    }*/

    void prePaint()
    {
        if(SDL_MUSTLOCK(screen))
        {
            if(SDL_LockSurface(screen) < 0) return;
        }
        const Uint32 color= SDL_MapRGB( screen->format, 0, 0, 0 );

        static SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = screen->w;
        rect.h = screen->h;

        SDL_FillRect(screen, &rect, color);
    }

    void postPaint(){
        if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
        SDL_Flip(screen);
    }

    void setpixel(int x, int y, Uint32 color)
    {
        //std::cout<<"\nDraw Pixel X: "<<x<<"  Y: "<<y;
        if(x>=0 && y>=0 && x<screen->w && y<screen->h)
        {

            Uint32 *pixmem32;

            pixmem32 = (Uint32*) screen->pixels  + y*width + x;
            *pixmem32 = color;
        }
        //else
        //     std::cout<<" ...NOT!";
    }


    void drawLine(int x1, int y1, int x2, int y2, Uint32 color)
    {
        int d_x=x2-x1;
        int d_y=y2-y1;
        if(d_x==0 && d_y==0)
           return;

        int max;
        double df_x, df_y;
        if(abs(d_x)>abs(d_y))
        {
            max = abs(d_x);
            df_x = (d_x*1.0)/abs(d_x);
            df_y = (d_y*1.0)/abs(d_x);
        }
        else
        {
            max = abs(d_y);
            df_y = (d_y*1.0)/abs(d_y);
            df_x = (d_x*1.0)/abs(d_y);
        }
        //std::cout<<"\nDRAW LINE ("<<x1<<", "<<y1<<") -> ("<<x2<<", "<<y2<<") - "<<df_x<<"  "<<df_y;
        for(int i=0; i<max; i++)
            setpixel(x1+i*df_x, y1+i*df_y, color );
    }

    void drawText(const std::string& aText, const int aPosX, const int aPosY, const int aColor) const
    {
        static SDL_Color sdl_color;
        static SDL_Rect textPosition;
        static SDL_Surface* textSurface;

        sdl_color.r = aColor >> 16;
        sdl_color.g = ((aColor >> 8) & 255);
        sdl_color.b = (aColor & 255);

        //textSurface = TTF_RenderText_Solid(font, aText.c_str(), sdl_color);

        textPosition.x = aPosX;
        textPosition.y = aPosY;

        SDL_BlitSurface(textSurface, &(textSurface->clip_rect), screen, &textPosition);

        SDL_FreeSurface(textSurface);
    }

    void draw3DText(const std::string& aText, const float x, const float y, const float z, const int aColor) const
    {
        int rx= width/2 + zoom*(pos_x+x-z) + 4;
        int ry= height/2 + zoom*(pos_z-y+z/2+x/2);
        drawText(aText, rx, ry, aColor);
    }

    void draw3DLine(float x1,float y1,float z1, float x2, float y2, float z2, Uint32 color )
    {

        //int rx1= width/2 + zoom*(pos_x+x1+z1/2);
        //int rx2= width/2 + zoom*(pos_x+x2+z2/2);
        //int ry1= height/2 + zoom*(pos_y+y1-z1/2);
        //int ry2= height/2 + zoom*(pos_y+y2-z2/2);
        x1+=pos_x; x2+=pos_x; z1+=pos_z; z2+=pos_z;
        int rx1, rx2, ry1, ry2;
        if(rotation == 0){
            rx1= width/2 + zoom*(x1-z1);
            rx2= width/2 + zoom*(x2-z2);
            ry1= height/2 + zoom*(-y1 +z1/2+x1/2);
            ry2= height/2 + zoom*(- y2 +z2/2+x2/2);
        }
        else if(rotation == 1){
            rx1 = width/2 + zoom*(-x1 -z1);
            rx2 = width/2 + zoom*(-x2 -z2);
            ry1 = height/2 + zoom*( - y1 -z1/2+x1/2);
            ry2 = height/2 + zoom*( - y2 -z2/2+x2/2);
        }else if(rotation == 2){
            rx1 = width/2 + zoom*(- x1 +z1);
            rx2 = width/2 + zoom*(- x2 +z2);
            ry1 = height/2 + zoom*(- y1 -z1/2 -x1/2);
            ry2 = height/2 + zoom*(- y2 -z2/2 -x2/2);
        }else{
            rx1 = width/2 + zoom*(x1 +z1);
            rx2 = width/2 + zoom*(x2 +z2);
            ry1 = height/2 + zoom*(- y1 +z1/2 -x1/2);
            ry2 = height/2 + zoom*(- y2 +z2/2 -x2/2);
        }
        //std::cout<<"\nDraw 3D Line ("<<x1<<", "<<y1<<", "<<z1<<") - ("<<x2<<", "<<y2<<", "<<z2<<") ";
        //std::cout<<" 2D Line -> ("<<rx1<<", "<<ry1<<") - ("<<rx2<<", "<<ry2<<") ";
        drawLine(rx1, ry1, rx2, ry2, color);
    }

    void drawBBox(const BBox &b, Uint32 color, const std::string& text = "")
    {
        draw3DLine(b.min.x, b.min.y, b.min.z, b.max.x, b.min.y, b.min.z, color);
        draw3DLine(b.min.x, b.min.y, b.min.z, b.min.x, b.max.y, b.min.z, color);
        draw3DLine(b.min.x, b.min.y, b.min.z, b.min.x, b.min.y, b.max.z, color);

        draw3DLine(b.max.x, b.min.y, b.min.z, b.max.x, b.max.y, b.min.z, color);
        draw3DLine(b.max.x, b.min.y, b.min.z, b.max.x, b.min.y, b.max.z, color);

        draw3DLine(b.min.x, b.max.y, b.min.z, b.max.x, b.max.y, b.min.z, color);
        draw3DLine(b.min.x, b.max.y, b.min.z, b.min.x, b.max.y, b.max.z, color);

        draw3DLine(b.min.x, b.min.y, b.max.z, b.max.x, b.min.y, b.max.z, color);
        draw3DLine(b.min.x, b.min.y, b.max.z, b.min.x, b.max.y, b.max.z, color);

        draw3DLine(b.min.x, b.max.y, b.max.z, b.max.x, b.max.y, b.max.z, color);
        draw3DLine(b.max.x, b.max.y, b.min.z, b.max.x, b.max.y, b.max.z, color);
        draw3DLine(b.max.x, b.min.y, b.max.z, b.max.x, b.max.y, b.max.z, color);

        draw3DText(text, b.max.x, b.min.y, b.max.z, color);
    }

    void drawSceneGraph(int node, const Transform &trans,
         const BBox &container, int depth)
    {
        if(node==-1)
        {
           node=sceneHandler->getRoot();
        }

        float f=abs(aim_depth-depth)*1.0/4;
        if(f>1.0) f=1.0;
        f=0.1+0.90*(1 - f)*(1 - f)*(1 - f);
        float f2=abs(aim_depth-depth-1)*1.0/4;
        if(f2>1.0) f2=1.0;
        f2=0.1+0.90*(1 - f2)*(1 - f2)*(1 - f2);

        //std::cout<<"\nTiefe: "<<depth<<" Farbfaktor: "<<f;
        Uint32 color= getColor(0,255*f,255*f);
        Uint32 color2= getColor(200,0,200);
        Uint32 color3= getColor(200,0,0);
        Uint32 color4= getColor(255*f2,255*f2,0);

        BBox b=trans.apply(sceneHandler->getBounds(node));
        if(depth==aim_depth && depth!=0)
        {
            //std::cout<<"\nDraw Pink Stuff o__o";
            BBox b2;
            b2=b;
            b2.min.y=b2.max.y=container.min.y;
            drawBBox(b2,color2);
        }

        if(depth == aim_depth-1)
           drawBBox(b,color3);
        Transform newtrans= trans*(sceneHandler->getTransform(node));


        typename t_sceneHandler::HandleIndexVec groups;
        sceneHandler->getChildGroups(node,groups);

        for(typename t_sceneHandler::HandleIndexVec::iterator it= groups.begin();
            it != groups.end(); ++it)
        {
            drawSceneGraph(*it, newtrans, b, depth+1);
        }

        typename t_sceneHandler::HandleIndexVec geometries;
        sceneHandler->getChildGeometries(node,geometries);

        if(depth+1>=aim_depth)
            for(typename t_sceneHandler::HandleIndexVec::iterator it= geometries.begin();
                 it != geometries.end(); ++it)
            {
                BBox b3=newtrans.apply(sceneHandler->getBounds(*it));
                if(depth+1==aim_depth)
                {
            std::ostringstream s1;
            s1 << *it << " /P: " <<  sceneHandler->getNumOfPrimitives(*it);
                    drawBBox(b3,color4,s1.str());
                }
                else
                    drawBBox(b3,color4);
            }

        if(depth>=aim_depth)
        {
            if(depth==aim_depth)
            {
        std::ostringstream s1;
        s1 << node << " /I: " << sceneHandler->getNumOfInstances(node)
        << " /P: " << sceneHandler->getNumOfPrimitives(node);

                drawBBox(b,color,s1.str());
            }
            else
                drawBBox(b,color);
        }

    }

    void drawAccelStruct(typename t_accelStruct::Iterator node,
         const BBox &container, int depth)
    {
        if(node==-1)
        {
           node=aStruct->getRoot();
        }

        float f=abs(aim_depth-depth)*1.0/4;
        if(f>1.0) f=1.0;
        f=0.1+0.90*(1 - f)*(1 - f)*(1 - f);
        float f2=abs(aim_depth-depth-1)*1.0/4;
        if(f2>1.0) f2=1.0;
        f2=0.1+0.90*(1 - f2)*(1 - f2)*(1 - f2);

        //std::cout<<"\nTiefe: "<<depth<<" Farbfaktor: "<<f;
        Uint32 color= getColor(0,200*f,0);
        Uint32 color2= getColor(0, 0,255*f);

        BBox b=aStruct->getBounds(node);

        if(!aStruct->isLeaf(node))
        {
            typename t_accelStruct::Iterator first, second;
            aStruct->getChildren(node,first,second);
            drawAccelStruct(first, b, depth+1);
            drawAccelStruct(second, b, depth+1);
        }

        if(depth>=aim_depth)
        {
            if(depth==aim_depth)
            {

                std::ostringstream s1;
        s1 << node;
                drawBBox(b,color,s1.str());

                if(aStruct->isLeaf(node))
                {
                    typename t_accelStruct::Iterator first, second;
                    aStruct->getChildren(node,first,second);

                    typename t_accelStruct::ElementIterator it, begin, end;
                    //aStruct->getElements(first, second, begin, end);
                    aStruct->getElements(node, begin, end);

                    for(it=begin; it != end; ++it)
                    {

                        //Transform trans = aStruct->getLeafTransformation(*it);
                        //int index = aStruct->getLeafGeoIndex(*it);
                        //BBox b4= (*it).bounds;
                        //drawBBox(b4,color2,"");
                    }

                }
            }
            else
                drawBBox(b,color);
        }

    }

    Uint32 getColor(Uint8 r, Uint8 g, Uint8 b)
    {
        return SDL_MapRGB( screen->format, r, g, b );
    }


};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__DEBUGWINDOW_HPP
