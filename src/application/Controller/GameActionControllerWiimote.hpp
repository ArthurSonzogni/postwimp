#ifndef GameActionControllerWiimote_HEAD
#define GameActionControllerWiimote_HEAD

#include "GameActionController.hpp"
#include <wiicpp.h>

class GameActionControllerWiimote : public GameActionController
{
    public:
        GameActionControllerWiimote();
        virtual void update(GameAction& gameAction, Application& application);
    
    protected:
        void connectToWiimotes(int numWiimotes, int timeout);
        void handleWiimoteStatus(CWiimote &wm);
        void handleWiimoteDisconnect(CWiimote &wm);
        void handleWiimoteReadData(CWiimote &wm);
        void getWiimoteUpdates();

        std::vector<CWiimote> wiimotes;
        CWii *wii;
};

#endif /* end of include guard: GameActionControllerWiimote_HEAD*/
