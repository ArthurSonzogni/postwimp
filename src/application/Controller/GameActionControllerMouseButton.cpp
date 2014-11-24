#include "GameActionControllerMouseButton.hpp"
#include "GameAction.hpp"
#include "system/Application.hpp"
#include <GLFW/glfw3.h>
#include "system/Input.hpp"


GameActionControllerMouseButton::GameActionControllerMouseButton()
{
}


void GameActionControllerMouseButton::update(GameAction& gameAction, Application& application)
{
    if (Input::isMouseHold(GLFW_MOUSE_BUTTON_LEFT))
        gameAction.action = GameAction::Add;
    else if (Input::isMouseHold(GLFW_MOUSE_BUTTON_RIGHT))
        gameAction.action = GameAction::Remove;
    else
        gameAction.action = GameAction::Idle;
}
