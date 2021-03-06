#ifndef POSTWIMPAPPLICATION_7WD7L3VK
#define POSTWIMPAPPLICATION_7WD7L3VK

#include "system/Application.hpp"
#include "utils/glm.hpp"
#include "voxelMap/VoxelMap.hpp"
#include "voxelMap/VoxelMapDisplayer.hpp"
#include "Controller/GameAction.hpp"
#include "graphic/ModelObj.hpp"
#include "graphic/Axes.hpp"


class PostWIMPApplication : public Application
{
    public:
        PostWIMPApplication();

    private:

        virtual void loop();
        void step();
        void draw();

        VoxelMap voxelMap;
        VoxelMapDisplayer voxelMapDisplayer;

        void cameraEvent();

        GameAction gameAction;

        // cursorSphere
        ModelObj sphereObj;
        ModelObj repereObj;

        // axes
        Axes axes;
        
};

#endif /* end of include guard: POSTWIMPAPPLICATION_7WD7L3VK */
