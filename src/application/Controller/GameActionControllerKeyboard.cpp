#include "GameActionControllerKeyboard.hpp"
#include "GameAction.hpp"
#include "system/Application.hpp"
#include <GLFW/glfw3.h>
#include "system/Input.hpp"


GameActionControllerKeyboard::GameActionControllerKeyboard()
{
    controllerName = "Keyboard";
    assignKeys();
}

void GameActionControllerKeyboard::assignKeys()
{
    keys[KEY_ESCAPE] = GLFW_KEY_ESCAPE;
    keys[KEY_RECONNECT] = GLFW_KEY_R;
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
    keys[KEY_SIZE_P] = GLFW_KEY_UP;
    keys[KEY_SIZE_M] = GLFW_KEY_DOWN;
    keys[KEY_CHANGE_COLOR] = GLFW_KEY_SPACE;
    keys[KEY_INCREASE_RED] = GLFW_KEY_R;
    keys[KEY_DECREASE_RED] = GLFW_KEY_F;
    keys[KEY_INCREASE_GREEN] = GLFW_KEY_T;
    keys[KEY_DECREASE_GREEN] = GLFW_KEY_G;
    keys[KEY_INCREASE_BLUE] = GLFW_KEY_Y;
    keys[KEY_DECREASE_BLUE] = GLFW_KEY_H;
    keys[KEY_INCREASE_SHIN] = GLFW_KEY_V;
    keys[KEY_DECREASE_SHIN] = GLFW_KEY_B;
}

void changeColor(uint32_t& c, int component, int side)
{
    const int64_t d = 1 << (8 * component);
    int64_t mask = 0;
    for(int i = 0; i<8; ++i) mask = (mask << 1) + d;
    //const int invMask = 0xFFFF ^ mask;

    //int64_t cc = c;

    //cc = (cc & invMask) | (mask & ( (cc & mask) + side*d ));

    c |= mask;
}
void removeColor(uint32_t& c, int component)
{
    const int64_t d = 1 << (8 * component);
    int64_t mask = 0;
    for(int i = 0; i<8; ++i) mask = (mask << 1) + d;
    //const int invMask = 0xFFFF ^ mask;

    //int64_t cc = c;

    //cc = (cc & invMask);
    c &= mask;
}

void GameActionControllerKeyboard::update(GameAction& gameAction, Application& application)
{
    float tDelta = 40.0 * application.getFrameDeltaTime();
    float rDelta = 2.0 * application.getFrameDeltaTime();

    // translation
    if (Input::isKeyHold(keys[KEY_CAMERA_LEFT]))
        gameAction.view= glm::translate(glm::mat4(1.0),glm::vec3(+tDelta,0.0,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_RIGHT]))
        gameAction.view= glm::translate(glm::mat4(1.0),glm::vec3(-tDelta,0.0,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_FORWARD]))
        gameAction.view= glm::translate(glm::mat4(1.0),glm::vec3(0.0,0.0,+tDelta))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_BACKWARD]))
        gameAction.view= glm::translate(glm::mat4(1.0),glm::vec3(0.0,0.0,-tDelta))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_UP]))
        gameAction.view= glm::translate(glm::mat4(1.0),glm::vec3(0.0,-tDelta,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_DOWN]))
        gameAction.view= glm::translate(glm::mat4(1.0),glm::vec3(0.0,+tDelta,0.0))*gameAction.view;

    // currentRotation
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_XN]))
        gameAction.view= glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(-1.0,0.0,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_XP]))
        gameAction.view= glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(+1.0,0.0,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_YN]))
        gameAction.view= glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,-1.0,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_YP]))
        gameAction.view= glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,+1.0,0.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_ZN]))
        gameAction.view= glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,0.0,-1.0))*gameAction.view;
    if (Input::isKeyHold(keys[KEY_CAMERA_TURN_ZP]))
        gameAction.view= glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,0.0,+1.0))*gameAction.view;

    // size
    if (Input::isKeyHold(keys[KEY_SIZE_P]))
        gameAction.brush.size *= 1.01;
    if (Input::isKeyHold(keys[KEY_SIZE_M]))
        gameAction.brush.size /= 1.01;

    // reconnect controllers
    if (Input::isKeyPressed(keys[KEY_RECONNECT]))
        gameAction.reconnectControllers();

    // escape
    if (Input::isKeyPressed(keys[KEY_ESCAPE]))
       application.exit();

    // change color
    if (Input::isKeyPressed(keys[KEY_CHANGE_COLOR]))
    {
        auto& c = gameAction.brush.color;
        c = (c << 1) + (c >> 31);
    }

    if (Input::isKeyHold(keys[KEY_INCREASE_RED]))
        changeColor(gameAction.brush.color,3,1);
    if (Input::isKeyHold(keys[KEY_DECREASE_RED]))
        removeColor(gameAction.brush.color,3);
    if (Input::isKeyHold(keys[KEY_INCREASE_GREEN]))
        changeColor(gameAction.brush.color,2,1);
    if (Input::isKeyHold(keys[KEY_DECREASE_GREEN]))
        removeColor(gameAction.brush.color,2);
    if (Input::isKeyHold(keys[KEY_INCREASE_BLUE]))
        changeColor(gameAction.brush.color,1,1);
    if (Input::isKeyHold(keys[KEY_DECREASE_BLUE]))
        removeColor(gameAction.brush.color,1);
    if (Input::isKeyHold(keys[KEY_INCREASE_SHIN]))
        changeColor(gameAction.brush.color,0,1);
    if (Input::isKeyHold(keys[KEY_DECREASE_SHIN]))
        removeColor(gameAction.brush.color,0);

}
