#ifndef GameActionControllerOptiTrack_HEAD
#define GameActionControllerOptiTrack_HEAD

#include "GameActionController.hpp"
#include <string>

#include <netinet/in.h>

class FrameListener;
class CommandListener;

class GameActionControllerOptiTrack : public GameActionController
{
    public:
        GameActionControllerOptiTrack(std::string serverAddress, int IDWiimote, int IDNunchuk);
        ~GameActionControllerOptiTrack();
        virtual void update(GameAction& gameAction, Application& application);
    protected:
        void printFrames();
        void connectOptiTrack();

        // Addresses
        uint32_t localAddress, serverAddress;    
        struct sockaddr_in serverCommands;
        // Sockets
        int sdCommand, sdData;
        // Listeners
        CommandListener *commandListener;
        FrameListener *frameListener;
        // Rigid bodies IDs
        int IDWiimote, IDNunchuk;

        bool isViewControlled = false;
        glm::mat4 savedView;
        glm::mat4 optiViewInertiel;
};

#endif /* end of include guard: GameActionControllerOptiTrack_HEAD*/
