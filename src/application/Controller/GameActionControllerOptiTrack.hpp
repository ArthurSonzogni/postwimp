#ifndef GameActionControllerOptiTrack_HEAD
#define GameActionControllerOptiTrack_HEAD

#include "GameActionController.hpp"
#include <string>

class FrameListener;
class CommandListener;

class GameActionControllerOptiTrack : public GameActionController
{
    public:
        GameActionControllerOptiTrack(std::string localAddress, std::string serverAddress);
        ~GameActionControllerOptiTrack();
        virtual void update(GameAction& gameAction, Application& application);
    protected:
        void printFrames();
        void connectOptiTrack(struct sockaddr_in serverCommands);

        // Addresses
        uint32_t localAddress, serverAddress;    
        // Sockets
        int sdCommand, sdData;
        // Listeners
        CommandListener *commandListener;
        FrameListener *frameListener;
};

#endif /* end of include guard: GameActionControllerOptiTrack_HEAD*/
