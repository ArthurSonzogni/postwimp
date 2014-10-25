#ifndef VOXELMAP_SEH0LAHJ
#define VOXELMAP_SEH0LAHJ

#include "Voxel.hpp"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

class VoxelMap
{
    public:
        VoxelMap(int sizeX, int sizeY, int sizeZ);
    private:
        int sizeX, sizeY, sizeZ;

        glm::ivec3 needUpdateMin,needUpdateMax;

        PolyVox::SimpleVolume<Voxel> volume;
};

#endif /* end of include guard: VOXELMAP_SEH0LAHJ */
