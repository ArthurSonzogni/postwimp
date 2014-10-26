#ifndef VOXEL_P271X96W
#define VOXEL_P271X96W
#include "utils/glm.hpp"

//#include "PolyVoxCore/DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type
//#include "PolyVoxCore/DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "PolyVoxCore/MaterialDensityPair.h"

typedef PolyVox::MaterialDensityPair<uint32_t, 8*4, 8> Voxel;


//class Voxel
//{
    //public:
        //Voxel();
        //Voxel(glm::vec4 color, float density);

        ////Density
        //void setDensity(float density);
        //float getDensity();

        ////Color
        //void setColor(const glm::vec4& color);
        //glm::vec4& getColor();

    //private:
        //glm::vec4 color;
        //float density;
//};

#endif /* end of include guard: VOXEL_P271X96W */
