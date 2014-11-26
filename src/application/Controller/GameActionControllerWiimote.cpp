#include "GameActionControllerWiimote.hpp"
#include "GameAction.hpp"
#include "system/Application.hpp"
#include <wiicpp.h>
#include <iostream>
#include <unistd.h>

static const int WIIMOTE_LED_MAP[4] = {CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4};

static const uint32_t colorMap[]=
    {
        0xFF0000FF,
        0x00FF00FF,
        0x0000FFFF,
        0x00FFFFFF,
        0xFF00FFFF,
        0xFFFF00FF,
    };
static int colorIndex = 0;


GameActionControllerWiimote::GameActionControllerWiimote() :
    wiimotes(0)
{
    // Connect to the wiimote(s)
    connectToWiimotes(1, 2);
}

void GameActionControllerWiimote::update(GameAction& gameAction, Application& application)
{
    float tDelta = 40.0 * application.getFrameDeltaTime();
    float rDelta = 2.0 * application.getFrameDeltaTime();

    // Handle Wiimote / Nunchuk
    for(auto i = wiimotes.begin(); i != wiimotes.end(); ++i)
    {
        CWiimote & wm = *i;
        int exType = wm.ExpansionDevice.GetType();
        if(exType == wm.ExpansionDevice.TYPE_NUNCHUK)
        {
            CNunchuk &nc = wm.ExpansionDevice.Nunchuk;

            //if(nc.Buttons.isPressed(CNunchukButtons::BUTTON_C))
            //TODO: remove buttons and use tracker
            /*if(nc.Buttons.isPressed(CNunchukButtons::BUTTON_Z))
            {
                if (wm.Buttons.isHeld(CButtons::BUTTON_LEFT))
                    gameAction.view = glm::translate(glm::mat4(1.0),glm::vec3(+tDelta,0.0,0.0))*gameAction.view;
                if (wm.Buttons.isHeld(CButtons::BUTTON_RIGHT))
                    gameAction.view = glm::translate(glm::mat4(1.0),glm::vec3(-tDelta,0.0,0.0))*gameAction.view;
                if (wm.Buttons.isHeld(CButtons::BUTTON_UP))
                    gameAction.view = glm::translate(glm::mat4(1.0),glm::vec3(0.0,0.0,+tDelta))*gameAction.view;
                if (wm.Buttons.isHeld(CButtons::BUTTON_DOWN))
                    gameAction.view = glm::translate(glm::mat4(1.0),glm::vec3(0.0,-tDelta,0.0))*gameAction.view;
            }
            else
            {
                if (wm.Buttons.isHeld(CButtons::BUTTON_LEFT))
                    gameAction.view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(-1.0,0.0,0.0))*gameAction.view;
                if (wm.Buttons.isHeld(CButtons::BUTTON_RIGHT))
                    gameAction.view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(+1.0,0.0,0.0))*gameAction.view;
                if (wm.Buttons.isHeld(CButtons::BUTTON_UP))
                    gameAction.view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,0.0,+1.0))*gameAction.view;
                if (wm.Buttons.isHeld(CButtons::BUTTON_DOWN))
                    gameAction.view = glm::rotate(glm::mat4(1.0),rDelta,glm::vec3(0.0,0.0,-1.0))*gameAction.view;
            }*/

            //TODO: tester les valeurs
            if (wm.Buttons.isPressed(CButtons::BUTTON_PLUS))
                gameAction.brush.size += 0.1;
            if (wm.Buttons.isPressed(CButtons::BUTTON_MINUS))
                gameAction.brush.size -= 0.1;

            //TODO: tester les valeurs
            if (wm.Buttons.isPressed(CButtons::BUTTON_UP))
                gameAction.brush.strength += 0.1;
            if (wm.Buttons.isPressed(CButtons::BUTTON_DOWN))
                gameAction.brush.strength -= 0.1;
           
            //TODO: tester si ça marche bien 
            if (wm.Buttons.isPressed(CButtons::BUTTON_RIGHT))
                gameAction.brush.color = colorMap[++colorIndex%6];
            if (wm.Buttons.isPressed(CButtons::BUTTON_LEFT))
            {
                --colorIndex;
                if (colorIndex < 0) colorIndex = 5;
                gameAction.brush.color = colorMap[colorIndex];
            }
        }
    }
}



void GameActionControllerWiimote::connectToWiimotes(int numWiimotes, int timeout)
{
    wii = new CWii(numWiimotes);
    std::vector<CWiimote>::iterator i;
    int numFound;
    int index;

    std::cout << "Searching for wiimotes... Turn them on!" << endl;

    //Find the wiimote
    numFound = wii->Find(timeout);

    // Search for up to five seconds;

    std::cout << "Found " << numFound << " wiimotes" << endl;
    std::cout << "Connecting to wiimotes..." << endl;

    // Connect to the wiimote
    wiimotes = wii->Connect();

	std::cout << "Connected to " << (unsigned int)wiimotes.size() << " wiimotes" << endl;

    // Setup the wiimotes
    for(index = 0, i = wiimotes.begin(); i != wiimotes.end(); ++i, ++index)
    {
        // Use a reference to make working with the iterator handy.
        CWiimote & wiimote = *i;

        //Set Leds
        wiimote.SetLEDs(WIIMOTE_LED_MAP[index]);

        //Rumble for 0.2 seconds as a connection ack
        wiimote.SetRumbleMode(CWiimote::ON);
        usleep(200000);
        wiimote.SetRumbleMode(CWiimote::OFF);
    }
}

void GameActionControllerWiimote::handleWiimoteStatus(CWiimote &wm)
{
    printf("\n");
    printf("--- CONTROLLER STATUS [wiimote id %i] ---\n\n", wm.GetID());

    printf("attachment: %i\n", wm.ExpansionDevice.GetType());
    printf("speaker: %i\n", wm.isUsingSpeaker());
    printf("ir: %i\n", wm.isUsingIR());
    printf("leds: %i %i %i %i\n", wm.isLEDSet(1), wm.isLEDSet(2), wm.isLEDSet(3), wm.isLEDSet(4));
    printf("battery: %f %%\n", wm.GetBatteryLevel());
}

void GameActionControllerWiimote::handleWiimoteDisconnect(CWiimote &wm)
{
    printf("\n");
    printf("--- DISCONNECTED [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void GameActionControllerWiimote::handleWiimoteReadData(CWiimote &wm)
{
    printf("\n");
    printf("--- DATA READ [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void GameActionControllerWiimote::getWiimoteUpdates()
{
    static bool reloadWiimotes = false;
    if(reloadWiimotes)
    {
        // Regenerate the list of wiimotes
        wiimotes = wii->GetWiimotes();
        reloadWiimotes = 0;
    }

    //Poll the wiimotes to get the status like pitch or roll
    if(wii->Poll())
    {
        for(auto i = wiimotes.begin(); i != wiimotes.end(); ++i)
        {
            // Use a reference to make working with the iterator handy.
            CWiimote & wiimote = *i;
            switch(wiimote.GetEvent())
            {

                case CWiimote::EVENT_EVENT:
                    break;

                case CWiimote::EVENT_STATUS:
                    handleWiimoteStatus(wiimote);
                    break;

                case CWiimote::EVENT_DISCONNECT:
                case CWiimote::EVENT_UNEXPECTED_DISCONNECT:
                    handleWiimoteDisconnect(wiimote);
                    reloadWiimotes = true;
                    break;

                case CWiimote::EVENT_READ_DATA:
                    handleWiimoteReadData(wiimote);
                    break;

                case CWiimote::EVENT_NUNCHUK_INSERTED:
                    std::cout << "Nunchuk inserted on controller " << wiimote.GetID() << endl;
                    reloadWiimotes = true;
                    break;

                case CWiimote::EVENT_CLASSIC_CTRL_INSERTED:
                    std::cout << "Classic controller inserted on controller " << wiimote.GetID() << endl;
                    reloadWiimotes = true;
                    break;

                case CWiimote::EVENT_GUITAR_HERO_3_CTRL_INSERTED:
                    std::cout << "GH3 inserted on controller " << wiimote.GetID() << endl;
                    reloadWiimotes = true;
                    break;

                case CWiimote::EVENT_MOTION_PLUS_INSERTED:
                    std::cout << "Motion Plus inserted." << endl;
                    break;

                case CWiimote::EVENT_BALANCE_BOARD_INSERTED:
                    std::cout << "Balance Board connected."  << endl;
                    break;

                case CWiimote::EVENT_BALANCE_BOARD_REMOVED:
                    std::cout << "Balance Board disconnected."  << endl;
                    break;

                case CWiimote::EVENT_NUNCHUK_REMOVED:
                case CWiimote::EVENT_CLASSIC_CTRL_REMOVED:
                case CWiimote::EVENT_GUITAR_HERO_3_CTRL_REMOVED:
                case CWiimote::EVENT_MOTION_PLUS_REMOVED:
                    std::cout << "An expansion was removed." << endl;
                    handleWiimoteStatus(wiimote);
                    reloadWiimotes = true;
                    break;

                default:
                    break;
            }
        }
    }
}
