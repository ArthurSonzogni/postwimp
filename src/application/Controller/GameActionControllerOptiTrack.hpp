#ifndef GameActionControllerOptiTrack_HEAD
#define GameActionControllerOptiTrack_HEAD

#include "GameActionController.hpp"
#include <map>

class GameActionControllerOptiTrack : public GameActionController
{
    public:
        GameActionControllerOptiTrack();
        virtual void update(GameAction& gameAction, Application& application);
    protected:
};

#endif /* end of include guard: GameActionControllerOptiTrack_HEAD*/
