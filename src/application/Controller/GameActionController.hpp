#ifndef GAMEACTIONCONTROLLER_2FUBJ7UE
#define GAMEACTIONCONTROLLER_2FUBJ7UE

#include <string>

class GameAction;
class Application;


class GameActionController
{
    public:
        virtual void update(GameAction& gameAction, Application& application) = 0;
        virtual bool reconnect() {return true;};

        std::string controllerName = "";
};

#endif /* end of include guard: GAMEACTIONCONTROLLER_2FUBJ7UE */
