#ifndef POSTWIMPAPPLICATION_7WD7L3VK
#define POSTWIMPAPPLICATION_7WD7L3VK

#include <wiicpp.h>
#include "system/Application.hpp"
#include "utils/glm.hpp"
#include "voxelMap/VoxelMap.hpp"
#include "voxelMap/VoxelMapDisplayer.hpp"


class PostWIMPApplication : public Application
{
    public:
        PostWIMPApplication();

    private:
        void connectToWiimotes(int numWiimotes, int timeout);
        void handleWiimoteStatus(CWiimote &wm);
        void handleWiimoteDisconnect(CWiimote &wm);
        void handleWiimoteReadData(CWiimote &wm);
        void getWiimoteUpdates();

        virtual void loop();
        void step();
        void draw();

        glm::mat4 projection;
        glm::mat4 view;

        VoxelMap voxelMap;
        VoxelMapDisplayer voxelMapDisplayer;

        CWii *wii;
        std::vector<CWiimote>& wiimotes;

        void cameraEvent();
};

#endif /* end of include guard: POSTWIMPAPPLICATION_7WD7L3VK */
