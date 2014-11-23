#ifndef GAMEACTION_6YQTHSWZ
#define GAMEACTION_6YQTHSWZ

#include "utils/glm.hpp"
#include <list>

class GameActionController;
class Application;

class GameAction
{
    public:
        GameAction();
        void plugController(GameActionController* controller);
        void unplugController(GameActionController* controller);
        void update(Application& application);

        enum Action
        {
            Add,
            Remove,
            Idle
        };

        // state of the GameAction
        Action action;
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 pointerPosition;

    protected:

        // list of plugged Controller
        std::list<GameActionController*> controllerList;
};

#endif /* end of include guard: GAMEACTION_6YQTHSWZ */
