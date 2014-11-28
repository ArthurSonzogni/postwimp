#ifndef GAMEACTIONCONTROLLER_2FUBJ7UE
#define GAMEACTIONCONTROLLER_2FUBJ7UE

#include "GameAction.hpp"
#include <string>
#include <list>

class Application;


class GameActionController
{
    public:
        virtual void update(GameAction& gameAction, Application& application) = 0;
        virtual void processEvents(GameAction& gameAction, Application& application, std::list<GameAction::Event> events) {};
        virtual bool reconnect() {return true;};

        std::string controllerName = "";
};

#endif /* end of include guard: GAMEACTIONCONTROLLER_2FUBJ7UE */
