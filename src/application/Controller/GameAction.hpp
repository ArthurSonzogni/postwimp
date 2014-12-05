#ifndef GAMEACTION_6YQTHSWZ
#define GAMEACTION_6YQTHSWZ

#include <list>
#include "utils/glm.hpp"

class GameActionController;
class Application;


class GameAction
{
    public:
        GameAction();
        void plugController(GameActionController* controller);
        void unplugController(GameActionController* controller);
        void reconnectControllers();
        void update(Application& application);

        enum Action
        {
            Add,
            Remove,
            Idle
        };
        
        Action action;

        struct Brush
        {
            glm::vec3 position;
            uint32_t color;
            float size;
            float strength;
        };

        Brush brush = { {0.0,0.0,0.0}, 0xFF0000FF, 5.0, 1.0 };

        enum Event
        {
            BrushEntersVolume,
            BrushLeavesVolume,
            LostBrushTracking,
            RecoveredBrushTracking
        };

        std::list<Event> events;

        bool isViewControlled = false;

        glm::mat4 projection;
        glm::mat4 view;

    protected:

        // list of plugged Controllers
        std::list<GameActionController*> controllerList;
};

#endif /* end of include guard: GAMEACTION_6YQTHSWZ */
