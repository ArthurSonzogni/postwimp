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
    voxelMapDisplayer(voxelMap,24)
{
    ShaderLib shaderlib;

    // plug controllers
    gameAction.plugController(new GameActionControllerKeyboard());
    //gameAction.plugController(new GameActionControllerMouse());
    gameAction.plugController(new GameActionControllerMouseButton());
    gameAction.plugController(new GameActionControllerWiimote());
    gameAction.plugController(new GameActionControllerOptiTrack("10.10.0.6", "10.10.0.254")); // TODO: get local IP programmatically
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

    static bool ok = false;
    if (ok)
    {
        ok = false;
        for(int z = 1 ; z<H; ++z)
        {
            //cout<<z<<endl;
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


    const int T = 10 * gameAction.brush.size;

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
            level *= 3.0 / T;
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
            level *= 0.03 / T;
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
    static float deltaMean = getFrameDeltaTime();
    static int n = 0;
    deltaMean = 0.9*deltaMean + 0.1*getFrameDeltaTime();
    if (n++%100==0)
        cout << "fps=" << (int)(1/deltaMean) << endl;


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
    // TODO: Arthur aides moi ici!!!
    /*static bool brushWasInVolume = false;
    auto brushVoxel = voxelMap.get(px,py,pz);
    if (brushVoxel.convertToDensity() >= brushVoxel.getThreshold())
    {
        if (! brushWasInVolume)
        {
            GameAction::events.push_back(GameAction::BrushEntersVolume);
            brushWasInVolume = true;
        }
    }
    else
    {
        if (brushWasInVolume)
        {
            GameAction::events.push_back(GameAction::BrushLeavesVolume);
            brushWasInVolume = false;
        }
    }*/
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

    voxelMapDisplayer.display();
}
