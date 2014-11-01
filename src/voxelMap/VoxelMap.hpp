#ifndef VOXELMAP_SEH0LAHJ
#define VOXELMAP_SEH0LAHJ

#include "Voxel.hpp"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/Mesh.h"
#include "PolyVoxCore/PagedVolume.h"

class VoxelMap
{
    public:
        // constructor
        VoxelMap(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ);

        // dimension
        uint32_t getSizeX();
        uint32_t getSizeY();
        uint32_t getSizeZ();

        // block access
        Voxel get(uint32_t x, uint32_t y, uint32_t z);
        void set(uint32_t x, uint32_t y, uint32_t z,const Voxel& voxel);

        // block modifier
        void lerp(uint32_t x, uint32_t y, uint32_t z, Voxel target, float lambda);

        // getMesh
        Mesh extract(PolyVox::Region region);

    private:

        // dimension
        uint32_t sizeX, sizeY, sizeZ;


        // attributes
        glm::ivec3 needUpdateMin,needUpdateMax;
        PolyVox::PagedVolume<Voxel> volume;
};

#endif /* end of include guard: VOXELMAP_SEH0LAHJ */
