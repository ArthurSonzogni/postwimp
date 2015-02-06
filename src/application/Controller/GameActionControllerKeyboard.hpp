#ifndef GameActionControllerKeyboard_HEAD
#define GameActionControllerKeyboard_HEAD

#include "GameActionController.hpp"
#include <map>

class GameActionControllerKeyboard : public GameActionController
{
    public:
        GameActionControllerKeyboard();
        virtual void update(GameAction& gameAction, Application& application);
    
    protected:
        enum Keys {
            KEY_ESCAPE,
            KEY_RECONNECT,
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
            KEY_SIZE_P,
            KEY_SIZE_M,
            KEY_SAVE,
            KEY_CHANGE_COLOR,
            KEY_INCREASE_RED,
            KEY_DECREASE_RED,
            KEY_INCREASE_BLUE,
            KEY_DECREASE_BLUE,
            KEY_INCREASE_GREEN,
            KEY_DECREASE_GREEN,
            KEY_INCREASE_SHIN,
            KEY_DECREASE_SHIN
        };

        void assignKeys();
        std::map<Keys,int> keys;
};

#endif /* end of include guard: GameActionControllerKeyboard_HEAD*/
