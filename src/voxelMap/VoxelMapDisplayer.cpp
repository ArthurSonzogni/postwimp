#include "VoxelMapDisplayer.hpp"
VoxelMapDisplayer::VoxelMapDisplayer(VoxelMap& voxelMap,uint32_t bufferSize):
    voxelMap(voxelMap),
    bufferSize(bufferSize)
{
    nbBlockX = (voxelMap.getSizeX()+bufferSize-1)/bufferSize;
    nbBlockY = (voxelMap.getSizeY()+bufferSize-1)/bufferSize;
    nbBlockZ = (voxelMap.getSizeZ()+bufferSize-1)/bufferSize;
}
