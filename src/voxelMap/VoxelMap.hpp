#ifndef VOXELMAP_SEH0LAHJ
#define VOXELMAP_SEH0LAHJ

#include "Voxel.hpp"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

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

        // getMesh
        void extract(PolyVox::Region region, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh);
    private:

        // dimension
        uint32_t sizeX, sizeY, sizeZ;


        // attributes
        glm::ivec3 needUpdateMin,needUpdateMax;
        PolyVox::SimpleVolume<Voxel> volume;
};

#endif /* end of include guard: VOXELMAP_SEH0LAHJ */
