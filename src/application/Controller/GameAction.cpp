#include "GameAction.hpp"
#include "GameActionController.hpp"
#include "system/Application.hpp"
#include <iostream>

GameAction::GameAction():
    action(Idle),
    projection(glm::perspective(70.0f, 1.0f, 0.1f, 10000.f)),
    view(glm::lookAt(
        glm::vec3(-5.f,-5.f,-5.f),
        glm::vec3( 0.f, 0.f, 0.f),
        glm::vec3( 0.f, 0.f, 1.f)))
{}

void GameAction::plugController(GameActionController* controller)
{
    controllerList.push_back(controller);
}

void GameAction::unplugController(GameActionController* controller)
{
    controllerList.remove(controller);
}

void GameAction::reconnectControllers()
{
    std::cout << "Reconnecting to controllers..." << std::endl;
    for (auto it = controllerList.begin(); it != controllerList.end(); ++it)
    {
        if (! (**it).reconnect())
            std::cout << "Could not reconnect to controller " << (**it).controllerName << "!" << std::endl;
    }
}

void GameAction::update(Application& application)
{
    projection = glm::perspective(70.0f, application.getWindowRatio(), 0.1f, 10000.f);

    for(auto it = controllerList.begin(); it != controllerList.end(); ++it)
    {
        (**it).update(*this,application);
        (**it).processEvents(*this,application,events);
        events.clear();
    }
}
