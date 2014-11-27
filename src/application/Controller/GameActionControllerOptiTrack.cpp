#include "GameActionControllerOptiTrack.hpp"
#include "GameAction.hpp"
#include "system/Application.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


GameActionControllerOptiTrack::GameActionControllerOptiTrack(std::string localAddress, std::string serverAddress) :
    localAddress(inet_addr(localAddress.c_str())), serverAddress(inet_addr(serverAddress.c_str()))
{
    controllerName = "Opti Track";
    frameListener = NULL;
    commandListener = NULL;

    // Use this socket address to send commands to the server.
    serverCommands = NatNet::createAddress(this->serverAddress, NatNet::commandPort);

    // Create sockets
    sdCommand = NatNet::createCommandSocket(this->localAddress);
    sdData = NatNet::createDataSocket(this->localAddress);

    // Y U NO COMPILE?
    boost::thread* t = new boost::thread(&GameActionControllerOptiTrack::connectOptiTrack,this);
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

void GameActionControllerOptiTrack::update(GameAction& gameAction, Application& application)
{
    if (frameListener)
        printFrames();
}

// This thread loop just prints frames as they arrive.
void GameActionControllerOptiTrack::printFrames()
{
    bool valid;
    MocapFrame frame;
    while( true )
    {
        // Try to get a new frame from the listener.
        MocapFrame frame(frameListener->pop(&valid).first);
        // Quit if the listener has no more frames.
        if( !valid )
            break;
        std::cout << frame << std::endl;
    }
}

void GameActionControllerOptiTrack::connectOptiTrack()
{
    // Start the CommandListener in a new thread.
    commandListener = new CommandListener(sdCommand);
    commandListener->start();

    // Send a ping packet to the server so that it sends us the NatNet version
    // in its response to commandListener.
    NatNetPacket ping = NatNetPacket::pingPacket();
    ping.send(sdCommand, serverCommands);
    // Version number of the NatNet protocol, as reported by the server.
    unsigned char natNetMajor;
    unsigned char natNetMinor;

    // Wait here for ping response to give us the NatNet version.
    commandListener->getNatNetVersion(natNetMajor, natNetMinor);

    // Start up a FrameListener in a new thread.
    frameListener = new FrameListener(sdData, natNetMajor, natNetMinor);
    frameListener->start();
}

