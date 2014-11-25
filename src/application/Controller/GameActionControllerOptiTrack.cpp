#include "GameActionControllerOptiTrack.hpp"
#include "GameAction.hpp"
#include "system/Application.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <NatNetLinux/NatNet.h>
#include <NatNetLinux/CommandListener.h>
#include <NatNetLinux/FrameListener.h>

#include <time.h>


class Globals
{
    public:

        // Parameters read from the command line
        static uint32_t localAddress;
        static uint32_t serverAddress;

        // State of the main() thread.
        static bool run;
};

uint32_t Globals::localAddress = 0;
uint32_t Globals::serverAddress = 0;

// This thread loop just prints frames as they arrive.
void printFrames(FrameListener& frameListener)
{
    bool valid;
    MocapFrame frame;
    while( true )
    {
        // Try to get a new frame from the listener.
        MocapFrame frame(frameListener.pop(&valid).first);
        // Quit if the listener has no more frames.
        if( !valid )
            break;
        std::cout << frame << std::endl;
    }
}

void connect(FrameListener** frameListener, CommandListener** commandListener, int sdCommand, int sdData, struct sockaddr_in serverCommands)
{
    // Start the CommandListener in a new thread.
    (*commandListener) = new CommandListener(sdCommand);
    (*commandListener)->start();

    // Send a ping packet to the server so that it sends us the NatNet version
    // in its response to commandListener.
    NatNetPacket ping = NatNetPacket::pingPacket();
    ping.send(sdCommand, serverCommands);
    // Version number of the NatNet protocol, as reported by the server.
    unsigned char natNetMajor;
    unsigned char natNetMinor;

    // Wait here for ping response to give us the NatNet version.
    (*commandListener)->getNatNetVersion(natNetMajor, natNetMinor);

    // Start up a FrameListener in a new thread.
    (*frameListener) = new FrameListener(sdData, natNetMajor, natNetMinor);
    (*frameListener)->start();
}

GameActionControllerOptiTrack::GameActionControllerOptiTrack()
{
    frameListener = NULL;
    commandListener = NULL;
    // Catch ctrl-c and terminate gracefully.
    //signal(SIGINT, terminate);

    // Set addresses
    Globals::localAddress = inet_addr( "127.0.0.1");
    Globals::serverAddress = inet_addr( "127.0.0.1");
    // Use this socket address to send commands to the server.
    struct sockaddr_in serverCommands = NatNet::createAddress(Globals::serverAddress, NatNet::commandPort);

    // Create sockets
    sdCommand = NatNet::createCommandSocket( Globals::localAddress );
    sdData = NatNet::createDataSocket( Globals::localAddress );

    //connect(&frameListener,&commandListener,sdCommand,sdData, serverCommands);
    //boost::thread* t = new boost::thread(connect,&frameListener,&commandListener,sdCommand,sdData, serverCommands);
}


void GameActionControllerOptiTrack::update(GameAction& gameAction, Application& application)
{
    if (frameListener)
        printFrames(*frameListener);
}

GameActionControllerOptiTrack::~GameActionControllerOptiTrack()
{
    if (frameListener)
        frameListener->stop();
    if (commandListener)
        commandListener->stop();
    if (frameListener)
        frameListener->join();
    if (commandListener)
        commandListener->join();

    close(sdData);
    close(sdCommand);

    if (frameListener)
        delete frameListener;
    if (commandListener)
        delete commandListener;
}
