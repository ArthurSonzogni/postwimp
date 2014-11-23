#include "PostWIMPApplication.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "system/Input.hpp"
#include <iostream>
#include "ShaderLib.hpp"
#include <map>

using namespace std;

int32_t H = 128;

PostWIMPApplication::PostWIMPApplication():
    voxelMap(H,H,H),
    voxelMapDisplayer(voxelMap,24)
{
    ShaderLib shaderlib;
    projection = glm::perspective(70.0f, getWindowRatio(), 0.1f, 10000.f);
    view  = glm::lookAt(
        glm::vec3(-10,-10,-10),
        glm::vec3(0.0,0.0,0.0),
        glm::vec3(0.0,0.0,1.0)
    );
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

    static float i = 2;
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

    if (true)
    {
        static const uint32_t colorMap[]=
        {
            0xFF0000FF,
            0x00FF00FF,
            0x0000FFFF,
            0x00FFFFFF,
            0xFF00FFFF,
            0xFFFF00FF,
        };
        static uint32_t currentColor = 0xFF0000FF;


        static int colorIndex = 0;
        if (Input::isKeyPressed(GLFW_KEY_SPACE))
            currentColor = colorMap[++colorIndex%6];

        const int T = 10;
        float mouseX = float(Input::mouseX()) / float(getWidth()) ; 
        float mouseY = 1.0-float(Input::mouseY()) / float(getHeight()) ; 
        mouseX = 2.0*mouseX-1.0;
        mouseY = 2.0*mouseY-1.0;
        // inverse mouse position
        glm::mat4 inverse = glm::inverse(projection*view);
        glm::vec4 m1 = inverse*glm::vec4(mouseX,mouseY,+0.0,1.0);
        glm::vec4 m2 = inverse*glm::vec4(mouseX,mouseY,+1.0,1.0);
        m1 = m1/m1.w;
        m2 = m2/m2.w;
        float lambda = (m1.z-T)/(m1.z-m2.z);
        glm::vec3 p = glm::vec3(m1 + (m2-m1) * lambda);

        int px = p.x;
        int py = p.y;
        int pz = T;
        bool isAdd = Input::isMouseHold(GLFW_MOUSE_BUTTON_LEFT);
        bool isRem = Input::isMouseHold(GLFW_MOUSE_BUTTON_RIGHT);
        uint8_t currentDensity;
        if (isAdd)
            currentDensity = 255;
        if (isRem)
            currentDensity = 0;
        if (isAdd)
        {
            for(int x = -T/2; x<=T/2; ++x)
            for(int y = -T/2; y<=T/2; ++y)
            for(int z = -T/2; z<=T/2; ++z)
            {
                float level = x*x+y*y+z*z;
                level *= 0.3;
                level = exp(-level);
                level *= 3.0;
                if (level>1.0) level=1.0;
                int xx = px + x;
                int yy = py + y;
                int zz = pz + z;
                if (xx>=1 and xx<H and yy>=1 and yy<H and zz>1 and zz<H)
                voxelMap.lerp(xx,yy,zz,Voxel(currentColor,255),level);
            }

            // update the modified region
            voxelMapDisplayer.update(PolyVox::Region(
                PolyVox::Vector3DInt32(px-T/2,py-T/2,pz-T/2),
                PolyVox::Vector3DInt32(px+T/2,py+T/2,pz+T/2)
            ));
        }
        if (isRem)
        {
            for(int x = -T/2; x<=T/2; ++x)
            for(int y = -T/2; y<=T/2; ++y)
            for(int z = -T/2; z<=T/2; ++z)
            {
                float level = x*x+y*y+z*z;
                level *= 0.3;
                level = exp(-level);
                level *= 3.0;
                if (level>1.0) level=1.0;
                int xx = px + x;
                int yy = py + y;
                int zz = pz + z;
                if (xx>=1 and xx<H and yy>=1 and yy<H and zz>1 and zz<H)
                voxelMap.lerpDensity(xx,yy,zz,Voxel(currentColor,0.0),level);
            }

            // update the modified region
            voxelMapDisplayer.update(PolyVox::Region(
                PolyVox::Vector3DInt32(px-T/2,py-T/2,pz-T/2),
                PolyVox::Vector3DInt32(px+T/2,py+T/2,pz+T/2)
            ));
        }
    }

    cameraEvent();

    // escape
    if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
        exit();

    // print periodically fps
    static float deltaMean = getFrameDeltaTime();
    static int n = 0;
    deltaMean = 0.9*deltaMean + 0.1*getFrameDeltaTime();
    if (n++%100==0)
        cout << "fps=" << (int)(1/deltaMean) << endl;


    // resize the window is needed
    if (windowDimensionChange())
    {
        projection = glm::perspective(70.0f, getWindowRatio(), 0.1f, 10000.f);
    }

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
    ShaderLib::voxel -> setUniform("projection", projection);
    ShaderLib::voxel -> setUniform("view", view);
    ShaderLib::voxel -> setUniform("model", glm::mat4(1.0));

    voxelMapDisplayer.display();
}

void PostWIMPApplication::cameraEvent()
{
    float tDelta = 40.0 * getFrameDeltaTime();
    float rDelta = 2.0 * getFrameDeltaTime();

    enum Keys {
        KEY_ESCAPE,
        KEY_CAMERA_LEFT,
        KEY_CAMERA_RIGHT,
        KEY_CAMERA_FORWARD,
        KEY_CAMERA_BACKWARD,
        KEY_CAMERA_UP,
        KEY_CAMERA_DOWN,
        KEY_CAMERA_TURN_LEFT,
        KEY_CAMERA_TURN_RIGHT,
        KEY_CAMERA_TURN_XN,
        KEY_CAMERA_TURN_XP,
        KEY_CAMERA_TURN_YN,
        KEY_CAMERA_TURN_YP,
        KEY_CAMERA_TURN_ZN,
        KEY_CAMERA_TURN_ZP,
        KEY_SAVE
    };
    std::map<Keys,int> keys;

    keys[KEY_ESCAPE] = GLFW_KEY_ESCAPE;
    keys[KEY_CAMERA_LEFT] = GLFW_KEY_A;
    keys[KEY_CAMERA_RIGHT] = GLFW_KEY_D;
    keys[KEY_CAMERA_FORWARD] = GLFW_KEY_W;
    keys[KEY_CAMERA_BACKWARD] = GLFW_KEY_S;
    keys[KEY_CAMERA_UP] = GLFW_KEY_E;
    keys[KEY_CAMERA_DOWN] = GLFW_KEY_Q;
    keys[KEY_CAMERA_TURN_XN] = GLFW_KEY_I;
    keys[KEY_CAMERA_TURN_XP] = GLFW_KEY_K;
    keys[KEY_CAMERA_TURN_YN] = GLFW_KEY_J;
    keys[KEY_CAMERA_TURN_YP] = GLFW_KEY_L;
    keys[KEY_CAMERA_TURN_ZN] = GLFW_KEY_U;
    keys[KEY_CAMERA_TURN_ZP] = GLFW_KEY_O;

    // translation
    if (Input::isKeyHold(keys[KEY_CAMERA_LEFT]))
        view = glm::translate(glm::mat4(1.0),glm::vec3(+tDelta,0.0,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_RIGHT]))
        view = glm::translate(glm::mat4(1.0),glm::vec3(-tDelta,0.0,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_FORWARD]))
        view = glm::translate(glm::mat4(1.0),glm::vec3(0.0,0.0,+tDelta))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_BACKWARD]))
        view = glm::translate(glm::mat4(1.0),glm::vec3(0.0,0.0,-tDelta))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_UP]))
        view = glm::translate(glm::mat4(1.0),glm::vec3(0.0,-tDelta,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_DOWN]))
        view = glm::translate(glm::mat4(1.0),glm::vec3(0.0,+tDelta,0.0))*view;

    // currentRotation
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_XN]))
        view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(-1.0,0.0,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_XP]))
        view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(+1.0,0.0,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_YN]))
        view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,-1.0,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_YP]))
        view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,+1.0,0.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_ZN]))
        view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,0.0,-1.0))*view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_ZP]))
        view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,0.0,+1.0))*view;


}
