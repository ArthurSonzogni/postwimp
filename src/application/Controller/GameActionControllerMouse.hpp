#ifndef GameActionControllerMouse_HEAD
#define GameActionControllerMouse_HEAD

#include "GameActionController.hpp"
#include <map>

class GameActionControllerMouse : public GameActionController
{
    public:
        GameActionControllerMouse();
        virtual void update(GameAction& gameAction, Application& application);
    
    protected:
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

        void assignKey();
        std::map<Keys,int> keys;
};

#endif /* end of include guard: GameActionControllerMouse_HEAD*/
