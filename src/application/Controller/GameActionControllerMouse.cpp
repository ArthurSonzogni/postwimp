#include "GameActionControllerMouse.hpp"
#include "GameAction.hpp"
#include "system/Application.hpp"
#include <GLFW/glfw3.h>
#include "system/Input.hpp"


GameActionControllerMouse::GameActionControllerMouse()
{
}


void GameActionControllerMouse::update(GameAction& gameAction, Application& application)
{
    const float T = 10.f;
    float mouseX = float(Input::mouseX()) / float(application.getWidth()) ; 
    float mouseY = 1.0-float(Input::mouseY()) / float(application.getHeight()) ; 
    mouseX = 2.0*mouseX-1.0;
    mouseY = 2.0*mouseY-1.0;
    // inverse mouse position
    glm::mat4 inverse = glm::inverse(gameAction.projection*gameAction.view);
    glm::vec4 m1 = inverse*glm::vec4(mouseX,mouseY,+0.0,1.0);
    glm::vec4 m2 = inverse*glm::vec4(mouseX,mouseY,+1.0,1.0);
    m1 = m1/m1.w;
    m2 = m2/m2.w;
    float lambda = (m1.z-T)/(m1.z-m2.z);
    glm::vec3 p = glm::vec3(m1 + (m2-m1) * lambda);
    gameAction.pencil.position = p;
}
