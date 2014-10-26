#include "VoxelMap.hpp"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

VoxelMap::VoxelMap(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ):
    volume(PolyVox::Region(PolyVox::Vector3DInt32(0,0,0),PolyVox::Vector3DInt32(sizeX-1,sizeY-1,sizeZ-1)))
{
 
}

Voxel VoxelMap::get(uint32_t x, uint32_t y, uint32_t z)
{
    return volume.getVoxelAt(x,y,z);
}

void VoxelMap::set(uint32_t x, uint32_t y, uint32_t z,const Voxel& voxel)
{
    volume.setVoxelAt(x,y,z,voxel);
}

//MarchingCubesSurfaceExtractor< SimpleVolume<Voxel> > surfaceExtractor(
        //&volData, PolyVox::Region(
        //Vector3DInt32(0,0,0),
        //Vector3DInt32(32,32,32)
        //), &mesh
    //);
    //
uint32_t VoxelMap::getSizeX() { return sizeX; }
uint32_t VoxelMap::getSizeY() { return sizeY; }
uint32_t VoxelMap::getSizeZ() { return sizeZ; }


void VoxelMap::extract(PolyVox::Region region, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh)
{
    PolyVox::MarchingCubesSurfaceExtractor<PolyVox::SimpleVolume<Voxel>>
        surfaceExtractor(&volume,region,&mesh);
    surfaceExtractor.execute();
}
