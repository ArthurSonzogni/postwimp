#ifndef GameActionControllerOptiTrack_HEAD
#define GameActionControllerOptiTrack_HEAD

#include "GameActionController.hpp"
#include <map>

class FrameListener;
class CommandListener;

class GameActionControllerOptiTrack : public GameActionController
{
    public:
        GameActionControllerOptiTrack();
        ~GameActionControllerOptiTrack();
        virtual void update(GameAction& gameAction, Application& application);
    protected:
        FrameListener* frameListener;
        CommandListener* commandListener;
        int sdData;
        int sdCommand;
};

#endif /* end of include guard: GameActionControllerOptiTrack_HEAD*/
