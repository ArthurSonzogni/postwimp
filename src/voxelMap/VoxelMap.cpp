#include "VoxelMap.hpp"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

VoxelMap::VoxelMap(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ):
    sizeX(sizeX),
    sizeY(sizeZ),
    sizeZ(sizeZ),
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

uint32_t VoxelMap::getSizeX() { return sizeX; }
uint32_t VoxelMap::getSizeY() { return sizeY; }
uint32_t VoxelMap::getSizeZ() { return sizeZ; }


Mesh VoxelMap::extract(PolyVox::Region region)
{
	auto mesh = extractMarchingCubesMesh(&volume, region);
    return decodeMesh(mesh);
}

void VoxelMap::lerp(uint32_t x, uint32_t y, uint32_t z, Voxel target, float lambda)
{
    PolyVox::DefaultMarchingCubesController<Voxel> c;
    volume.setVoxelAt(x,y,z,c.blendMaterials(volume.getVoxelAt(x,y,z),target,lambda));
}
void VoxelMap::lerpDensity(uint32_t x, uint32_t y, uint32_t z, Voxel target, float lambda)
{
    Voxel v = volume.getVoxelAt(x,y,z);
    v.setDensity(v.getDensity() * (1.0-lambda) + target.getDensity() * lambda);
    volume.setVoxelAt(x,y,z,v);
}
void VoxelMap::lerpColor(uint32_t /*x*/, uint32_t /*y*/, uint32_t /*z*/, Voxel /*target*/, float /*lambda*/)
{
    
}
