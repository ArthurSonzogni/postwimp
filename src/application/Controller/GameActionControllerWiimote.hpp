#ifndef GameActionControllerWiimote_HEAD
#define GameActionControllerWiimote_HEAD

#include "GameActionController.hpp"
#include <wiicpp.h>

// Idée pour avoir plusieurs wiimotes : connectToWiimotes public static et qui retourne un vecteur de controllers
class GameActionControllerWiimote : public GameActionController
{
    public:
        GameActionControllerWiimote();
        virtual bool reconnect() override;
        virtual void update(GameAction& gameAction, Application& application) override;
        virtual void processEvents(GameAction& gameAction, Application& application, std::list<GameAction::Event> events) override;

    protected:
        bool connectToWiimotes(int numWiimotes, int timeout);
        void handleWiimoteStatus(CWiimote &wm);
        void handleWiimoteDisconnect(CWiimote &wm);
        void handleWiimoteReadData(CWiimote &wm);
        void getWiimoteUpdates();

        std::vector<CWiimote> wiimotes;
        CWii *wii;
};

#endif /* end of include guard: GameActionControllerWiimote_HEAD*/
