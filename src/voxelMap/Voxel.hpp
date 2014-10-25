#ifndef VOXEL_P271X96W
#define VOXEL_P271X96W
#include "utils/glm.hpp"

//#include "PolyVoxCore/DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type
//#include "PolyVoxCore/DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "utils/glm.hpp"

class Voxel
{
    public:
        Voxel();
        Voxel(glm::vec4 color, float density);
    private:
        glm::vec4 color;
        float density;
};

#endif /* end of include guard: VOXEL_P271X96W */
