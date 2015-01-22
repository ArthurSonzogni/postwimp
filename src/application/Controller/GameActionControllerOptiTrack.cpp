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
#include "utils/glm.hpp"


bool checkServerConnectivity(int sock, struct sockaddr_in addr)
{
    if ((sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return false;
    }

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        return false;
    }
    return true;
}

struct in_addr getLocalAddress(int sock)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    getsockname(sock, (struct sockaddr *) &addr, &len);
    std::cout << "Local IP address used to communicate with Opti Track : " << inet_ntoa(addr.sin_addr) << std::endl; 
    //printf("IP used to communicate = %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    return addr.sin_addr;
}



GameActionControllerOptiTrack::GameActionControllerOptiTrack(std::string serverAddress, int IDWiimote, int IDNunchuk) :
    serverAddress(inet_addr(serverAddress.c_str())), IDWiimote(IDWiimote), IDNunchuk(IDNunchuk)
{
    controllerName = "Opti Track";
    frameListener = NULL;
    commandListener = NULL;

    // Use this socket address to send commands to the server.
    serverCommands = NatNet::createAddress(this->serverAddress, NatNet::commandPort);

    // Check server reachability and get corresponding local address
    int testSocket = -1;
    if (! checkServerConnectivity(testSocket, serverCommands))
    {
        std::cerr << "Can't connect to Opti Track server !" << std::endl;
    }
    else
    {
        this->localAddress = getLocalAddress(testSocket).s_addr; 

        // Create sockets
        sdCommand = NatNet::createCommandSocket(this->localAddress);
        sdData = NatNet::createDataSocket(this->localAddress);

        // TODO handle killing t and check if connection is OK
        boost::thread* t = new boost::thread(&GameActionControllerOptiTrack::connectOptiTrack,this);
    }
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
    const glm::mat4 frameToWorld = {
        {-64.0,0.0,0.0,0.0},
        {0.0,64.0,0.0,0.0},
        {0.0,0.0,-64.0,0.0},
        {0.0,0.0,-64.0,1.0},
    };

    if (frameListener)
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

            //std::cout << frame << std::endl;
            auto rigid = frame.rigidBodies();
            for(auto it = rigid.begin(); it!=rigid.end(); ++it)
            {
                if (it->trackingValid())
                {
                    if (it->id() == IDWiimote)
                    {
                        auto pos = (*it).location();
                        //gameAction.brush.position.x = -pos.x;
                        //gameAction.brush.position.y = +pos.y;
                        //gameAction.brush.position.z = -pos.z - 1.0;
                        //gameAction.brush.position *= 64.f;
                        //gameAction.brush.position = glm::vec3(glm::inverse(gameAction.view) * glm::vec4(gameAction.brush.position,1.0));
                        gameAction.brush.position = glm::vec3(glm::inverse(gameAction.view) * frameToWorld * glm::vec4(pos.x,pos.y,pos.z,1.0));
                    }
                    else if (it->id() == IDNunchuk)
                    {
                        auto rot = (*it).orientation();
                        auto pos = (*it).location();

                        glm::quat q(rot.qw, rot.qx, rot.qy, rot.qz);
                        //glm::mat4 r = glm::inverse(glm::mat4_cast(q));
                        glm::mat4 r = glm::mat4_cast(q);

                        handPosition = glm::vec3(frameToWorld * glm::vec4(pos.x,pos.y,pos.z,1.f));
                        //r = glm::inverse(frameToWorld) * r * frameToWorld;
                        r = frameToWorld * r * glm::inverse(frameToWorld);
                        r[3] = glm::vec4(0.0,0.0,0.0,1.0);
                        handRotation = r;

                        glm::mat4 optiView = gameAction.handPosition;
                        
                        

                        if (gameAction.isViewControlled) 
                        {
                            if (!isViewControlled)
                            {
                                isViewControlled = true;
                                savedView = glm::inverse(optiView) * gameAction.view;
                                optiViewInertiel = optiView;

                            }
                            optiViewInertiel = optiViewInertiel*0.9f+optiView*0.1f;
                            optiViewInertiel = optiView;
                            gameAction.view = optiViewInertiel * savedView;
                        }
                        else
                        {
                            isViewControlled = false;
                        }
                    }
                }
                /*else
                {
                    std::cout << "invalid" << std::endl;
                }*/
            }
        }
    }
    gameAction.handPosition = glm::mat4(1.0);
    gameAction.handPosition = glm::translate(gameAction.handPosition,handPosition);
    gameAction.handPosition = gameAction.handPosition * handRotation;
    
}

// This thread loop just prints frames as they arrive.
void GameActionControllerOptiTrack::printFrames()
{
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
    std::cout  << "GameActionControllerOptiTrack::connectOptiTrackEnd" << std::endl;
}

