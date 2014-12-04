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

}
