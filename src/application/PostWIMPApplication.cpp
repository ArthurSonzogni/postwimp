#include "PostWIMPApplication.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "system/Input.hpp"
#include <iostream>
#include "ShaderLib.hpp"
#include <map>
#include "Controller/GameActionControllerKeyboard.hpp"
#include "Controller/GameActionControllerMouse.hpp"
#include "Controller/GameActionControllerMouseButton.hpp"
#include "Controller/GameActionControllerWiimote.hpp"
#include "Controller/GameActionControllerOptiTrack.hpp"

using namespace std;

int32_t H = 128;

PostWIMPApplication::PostWIMPApplication():
    voxelMap(H,H,H),
    voxelMapDisplayer(voxelMap,24),
    sphereObj("obj/sphere.obj",
        ShaderProgram::loadFromFile(
        "shader/cursor-shader.vert",
        "shader/cursor-shader.frag")
    ),
    repereObj("obj/repere.obj",
        ShaderProgram::loadFromFile(
        "shader/cursor-shader.vert",
        "shader/cursor-shader.frag")
    )
{
    ShaderLib shaderlib;

    // plug controllers
    gameAction.plugController(new GameActionControllerKeyboard());
    //gameAction.plugController(new GameActionControllerMouse());
    //gameAction.plugController(new GameActionControllerMouseButton());
    gameAction.plugController(new GameActionControllerWiimote());
    gameAction.plugController(new GameActionControllerOptiTrack("10.10.0.254", 3, 4));
}

void PostWIMPApplication::loop()
{
    step();
    draw();
}

inline float sqr(float t){return t*t;}

void PostWIMPApplication::step()
{
    // update the keyboard/mouse states
    Input::update(getWindow());

    // update the gameAction
    gameAction.update(*this);

    static bool ok = true;
    if (false and ok)
    {
        for(int z = 1 ; z<H; ++z)
        {
            cout<<z<<endl;
            for(int y = 1 ; y<H; ++y)
                for(int x = 1 ; x<H; ++x)
                {
                    float level = glm::perlin(glm::vec3(x,y,z)*0.05f);
                    level = 0.0+500.0*level;
                    if (level>255.0) level = 255.0;
                    if (level<0.0) level = 0.0;

                    uint32_t color = 0xFFFFFFFF;
                    voxelMap.set(x,y,z,Voxel(color,level));
                }
        }
        voxelMapDisplayer.update(PolyVox::Region(
                    PolyVox::Vector3DInt32(0,0,0),
                    PolyVox::Vector3DInt32(H*2,H,H)
                    ));
    }

    if (true && ok)
    {
        for(int i = 1; i<H; ++i)
        {
            voxelMap.set(i,1,1,Voxel(0xFFFFFFFF,255));
            voxelMap.set(i,H-1,1,Voxel(0xFFFFFFFF,255));
            voxelMap.set(i,1,H-1,Voxel(0xFFFFFFFF,255));
            voxelMap.set(i,H-1,H-1,Voxel(0xFFFFFFFF,255));

            voxelMap.set(1,i,1,Voxel(0xFFFFFFFF,255));
            voxelMap.set(H-1,i,1,Voxel(0xFFFFFFFF,255));
            voxelMap.set(1,i,H-1,Voxel(0xFFFFFFFF,255));
            voxelMap.set(H-1,i,H-1,Voxel(0xFFFFFFFF,255));

            voxelMap.set(1,1,i,Voxel(0xFFFFFFFF,255));
            voxelMap.set(H-1,1,i,Voxel(0xFFFFFFFF,255));
            voxelMap.set(1,H-1,i,Voxel(0xFFFFFFFF,255));
            voxelMap.set(H-1,H-1,i,Voxel(0xFFFFFFFF,255));
        }
        voxelMapDisplayer.update(PolyVox::Region(
                    PolyVox::Vector3DInt32(0,0,0),
                    PolyVox::Vector3DInt32(H*2,H,H)
                    ));
    }
    ok = false;


    const int T = gameAction.brush.size;

    int px = gameAction.brush.position.x;
    int py = gameAction.brush.position.y;
    int pz = gameAction.brush.position.z;

    if (gameAction.action == GameAction::Add)
    {
        for(int x = -T/2; x<=T/2; ++x)
        for(int y = -T/2; y<=T/2; ++y)
        for(int z = -T/2; z<=T/2; ++z)
        {
            float level = x*x+y*y+z*z;
            level *= 15.0 / (T*T);
            level = exp(-level);
            level *= 3.0 * gameAction.brush.strength;
            if (level>1.0) level=1.0;
            int xx = px + x;
            int yy = py + y;
            int zz = pz + z;
            if (xx>=1 and xx<H and yy>=1 and yy<H and zz>1 and zz<H)
                voxelMap.lerp(xx,yy,zz,Voxel(gameAction.brush.color,255),level);
        }
        // update the modified region
        voxelMapDisplayer.update(PolyVox::Region(
                    PolyVox::Vector3DInt32(px-T/2,py-T/2,pz-T/2),
                    PolyVox::Vector3DInt32(px+T/2,py+T/2,pz+T/2)
                    ));
    }
    else if (gameAction.action == GameAction::Remove)
    {
        for(int x = -T/2; x<=T/2; ++x)
        for(int y = -T/2; y<=T/2; ++y)
        for(int z = -T/2; z<=T/2; ++z)
        {
            float level = x*x+y*y+z*z;
            level *= 15.0 / (T*T);
            level = exp(-level);
            level *= 3.0 * gameAction.brush.strength;
            if (level>1.0) level=1.0;
            int xx = px + x;
            int yy = py + y;
            int zz = pz + z;
            if (xx>=1 and xx<H and yy>=1 and yy<H and zz>1 and zz<H)        
                voxelMap.lerpDensity(xx,yy,zz,Voxel(gameAction.brush.color,0),level);

        }
        // update the modified region
        voxelMapDisplayer.update(PolyVox::Region(
                    PolyVox::Vector3DInt32(px-T/2,py-T/2,pz-T/2),
                    PolyVox::Vector3DInt32(px+T/2,py+T/2,pz+T/2)
                    ));
    }


    // print periodically fps
#ifdef PRINTFPS
    static float deltaMean = getFrameDeltaTime();
    static int n = 0;
    deltaMean = 0.9*deltaMean + 0.1*getFrameDeltaTime();
    if (n++%100==0)
        cout << "fps=" << (int)(1/deltaMean) << endl;
#endif


    // wireframe mode
    static bool isWireframeEnabled = false;
    if (Input::isKeyPressed(GLFW_KEY_V))
    {
        isWireframeEnabled = ! isWireframeEnabled;
        if (isWireframeEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }
    }
    
    // detect if brush is in the volume
    static int ttl = 0;
    static bool brushWasInVolume = false;
    
    auto brushVoxel = voxelMap.get(px,py,pz);
    
    if (brushVoxel.getDensity() >= 128) //brushVoxel.getThreshold()
    {
        if (! brushWasInVolume)
        {
           if (gameAction.action == GameAction::Idle)
            {
                gameAction.events.push_back(GameAction::BrushEntersVolume);
                brushWasInVolume = true;
                ttl = 10;
            }
        }
        else
        {
            ttl--;
            if (ttl == 0)
                gameAction.events.push_back(GameAction::BrushLeavesVolume);
        }
    }
    else
    {
        if (brushWasInVolume && ttl > 0)
        {
            gameAction.events.push_back(GameAction::BrushLeavesVolume);
            brushWasInVolume = false;
        }
    }
}

void PostWIMPApplication::draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    ShaderLib::voxel -> use();
    ShaderLib::voxel -> setUniform("projection", gameAction.projection);
    ShaderLib::voxel -> setUniform("view", gameAction.view);
    ShaderLib::voxel -> setUniform("model", glm::mat4(1.0));


    glDisable(GL_BLEND);
    voxelMapDisplayer.display();

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    sphereObj.getShader().use();
    sphereObj.getShader().setUniform("projection",gameAction.projection);
    sphereObj.getShader().setUniform("view",gameAction.view);
    sphereObj.getShader().setUniform("size",gameAction.brush.size/2.f);
    sphereObj.getShader().setUniform("pos",gameAction.brush.position-glm::vec3(0.5f,0.5f,0.5f));

    {
        glm::vec4 color;
        uint32_t m = gameAction.brush.color;
        color.a = m&0xFF; m>>=8;
        color.b = m&0xFF; m>>=8;
        color.g = m&0xFF; m>>=8;
        color.r = m&0xFF; m>>=8;
        color /= 255.0;
        sphereObj.getShader().setUniform("color",color);
    }
    sphereObj.draw();


    // repere
    //repereObj.getShader().use();
    //repereObj.getShader().setUniform("projection",gameAction.projection);
    //repereObj.getShader().setUniform("view",gameAction.view);
    //repereObj.getShader().setUniform("model",glm::translate(glm::scale(glm::mat4(1.f),glm::vec3(100.f)),glm::vec3(10.f,10.f,10.f)));

    //repereObj.draw();

    glm::mat4 axesView = gameAction.view;
    axesView[3].x =  - getHeight()/getWidth()*2.0;
    axesView[3].y =  - 2.0;
    axesView[3].z =  -5.0;

    axes.draw(gameAction.projection,axesView);

    glDisable(GL_BLEND);

}
