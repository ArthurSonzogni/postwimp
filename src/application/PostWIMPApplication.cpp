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
    gameAction.plugController(new GameActionControllerMouse());
    gameAction.plugController(new GameActionControllerMouseButton());
    gameAction.plugController(new GameActionControllerWiimote());
    gameAction.plugController(new GameActionControllerOptiTrack("127.0.0.1", "127.0.0.1"));
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
    if (ok)
    {
        ok = false;
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

    static const uint32_t colorMap[]=
    {
        0xFF0000FF,
        0x00FF00FF,
        0x0000FFFF,
        0x00FFFFFF,
        0xFF00FFFF,
        0xFFFF00FF,
    };
    //static uint32_t currentColor = 0xFF0000FF;


    //TODO: Input dans GameActionControllerXYZ
    static int colorIndex = 0;
    if (Input::isKeyPressed(GLFW_KEY_SPACE))
        //currentColor = colorMap[++colorIndex%6];
        gameAction.pencil.color = colorMap[++colorIndex%6];

    const int T = 10 * gameAction.pencil.size;

    int px = gameAction.pencil.position.x;
    int py = gameAction.pencil.position.y;
    int pz = gameAction.pencil.position.z;

    //TODO: pointeur fonction lerp
    for(int x = -T/2; x<=T/2; ++x)
        for(int y = -T/2; y<=T/2; ++y)
            for(int z = -T/2; z<=T/2; ++z)
            {
                float level = x*x+y*y+z*z;
                level *= 0.3 * gameAction.pencil.strength;
                level = exp(-level);
                level *= 3.0;
                if (level>1.0) level=1.0;
                int xx = px + x;
                int yy = py + y;
                int zz = pz + z;
                if (xx>=1 and xx<H and yy>=1 and yy<H and zz>1 and zz<H)
                {
                    if (gameAction.action == GameAction::Add)
                        voxelMap.lerp(xx,yy,zz,Voxel(gameAction.pencil.color,255),level);
                    else if (gameAction.action == GameAction::Remove)
                        voxelMap.lerpDensity(xx,yy,zz,Voxel(gameAction.pencil.color,0.0),level);
                }
            }

    // update the modified region
    voxelMapDisplayer.update(PolyVox::Region(
                PolyVox::Vector3DInt32(px-T/2,py-T/2,pz-T/2),
                PolyVox::Vector3DInt32(px+T/2,py+T/2,pz+T/2)
                ));

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
