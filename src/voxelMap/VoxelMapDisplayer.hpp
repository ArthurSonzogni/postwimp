#ifndef VOXELMAPDISPLAYER_WENJIRSX
#define VOXELMAPDISPLAYER_WENJIRSX

#include "VoxelMap.hpp"
#include <vector>

// display a VoxelMap.
// The voxelMap is subdivided in block of size bufferSize.
// When the voxelMap is updated, the voxelMapDisplayer
// is updating its mesh only when it is needed.
class VoxelMapDisplayer
{
    public:
        // constructor
        VoxelMapDisplayer(VoxelMap& voxelMap,uint32_t bufferSize);

        // draw
        void display();
    private:
        // voxelMap
        VoxelMap& voxelMap;

        // block/buffer dimension
        uint32_t bufferSize;
        uint32_t nbBlockX,nbBlockY,nbBlockZ;

        // mesh
        std::vector< PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> > mesh;
};

#endif /* end of include guard: VOXELMAPDISPLAYER_WENJIRSX */
