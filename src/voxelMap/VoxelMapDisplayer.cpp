#include "VoxelMapDisplayer.hpp"
#include "ShaderLib.hpp"
#include <iostream>
using namespace std;

VoxelMapDisplayer::VoxelMapDisplayer(VoxelMap& voxelMap,uint32_t bufferSize):
    voxelMap(voxelMap),
    bufferSize(bufferSize)
{
    nbBlockX = (voxelMap.getSizeX()+bufferSize-1)/bufferSize;
    nbBlockY = (voxelMap.getSizeY()+bufferSize-1)/bufferSize;
    nbBlockZ = (voxelMap.getSizeZ()+bufferSize-1)/bufferSize;
    nbBlock = nbBlockX * nbBlockY * nbBlockZ;

    // allocate MeshGL
    mesh.reserve(nbBlock);
    for(uint32_t i = 0 ; i<nbBlock ; ++i)
    {
        mesh[i] = {0,0,0,0};
    }
}

void VoxelMapDisplayer::display()
{
    ShaderLib::voxel->use();
    for(uint32_t i = 0 ; i < nbBlock ; ++i)
    {
        MeshGL& m = mesh[i]; 
        if (m.size)
        {
            glBindVertexArray(m.vao);
            glDrawElements(
                 GL_TRIANGLES,      // mode
                 m.size,            // count
                 GL_UNSIGNED_INT,   // type
                 (void*)0           // element array buffer offset
             );
        }
    }
    glBindVertexArray(0);
    ShaderLib::voxel->unuse();
}

void VoxelMapDisplayer::update(PolyVox::Region region)
{
    const PolyVox::Vector3DInt32& lowerCorner = region.getLowerCorner();
    const PolyVox::Vector3DInt32& upperCorner = region.getUpperCorner();

    uint32_t xmin,xmax;
    uint32_t ymin,ymax;
    uint32_t zmin,zmax;

    xmin = lowerCorner.getX() / nbBlockX;
    ymin = lowerCorner.getY() / nbBlockY;
    zmin = lowerCorner.getZ() / nbBlockZ;

    xmax = ( upperCorner.getX() + nbBlockX -1 ) / nbBlockX;
    ymax = ( upperCorner.getY() + nbBlockX -1 ) / nbBlockY;
    zmax = ( upperCorner.getZ() + nbBlockX -1 ) / nbBlockZ;

    if (xmin>nbBlockX) xmin = 0;
    if (ymin>nbBlockY) ymin = 0;
    if (zmin>nbBlockZ) zmin = 0;
    if (xmax>nbBlockX) xmax = nbBlockX;
    if (ymax>nbBlockY) ymax = nbBlockY;
    if (zmax>nbBlockZ) zmax = nbBlockZ;

    for(uint32_t z = zmin; z<zmax; ++z)
    for(uint32_t y = ymin; y<ymax; ++y)
    for(uint32_t x = xmin; x<xmax; ++x)
        updateBox(x,y,z);
}

void VoxelMapDisplayer::updateBox(uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t pos =
        x+nbBlockX*(
        y+nbBlockY*(
        z));

    MeshGL& meshGL = mesh[pos];
    
    PolyVox::Region region(
        PolyVox::Vector3DInt32((x+0)*bufferSize,(y+0)*bufferSize,(z+0)*bufferSize),
        PolyVox::Vector3DInt32((x+1)*bufferSize,(y+1)*bufferSize,(z+1)*bufferSize)
    );

    PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> m;

    voxelMap.extract(region, m);

	const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = m.getVertices();
	const std::vector<uint32_t>& vecIndices = m.getIndices();


    if (meshGL.size != 0)
    {
        glDeleteVertexArrays(1,&meshGL.vao);
        glDeleteBuffers(1,&meshGL.vbo);
        glDeleteBuffers(1,&meshGL.veo);
        meshGL = {0,0,0,0};
    }

    if (vecVertices.size() > 0)
    {

        // building vecVBO
        std::vector<GLVertice> vecVBO;
        vecVBO.reserve(vecVertices.size());
        for(vector<PolyVox::PositionMaterialNormal>::const_iterator iterVertex = vecVertices.begin(); iterVertex != vecVertices.end(); ++iterVertex)
        {
            const PolyVox::PositionMaterialNormal& vertex = *iterVertex;
            const PolyVox::Vector3DFloat& pos = vertex.getPosition();
            const PolyVox::Vector3DFloat& normal = vertex.getNormal();
            uint32_t material = vertex.getMaterial();

            float px = pos.getX() + x*bufferSize ;
            float py = pos.getY() + y*bufferSize ;
            float pz = pos.getZ() + z*bufferSize ;

            float nx = normal.getX();
            float ny = normal.getY();
            float nz = normal.getZ();

            float a = (material&0xFF)/255.0; material >>= 8;
            float b = (material&0xFF)/255.0; material >>= 8;
            float g = (material&0xFF)/255.0; material >>= 8;
            float r = (material&0xFF)/255.0; material >>= 8;

            vecVBO.push_back({{px,py,pz},{nx,ny,nz},{r,g,b,a}});
        }


        // building vecVEO (simple alias in this case)
        const std::vector<uint32_t>& vecVEO = vecIndices;
        

        // vbo
        glGenBuffers( 1, &meshGL.vbo );
        glBindBuffer(GL_ARRAY_BUFFER, meshGL.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLVertice)*vecVBO.size(), &vecVBO[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // veo
        glGenBuffers(1, &meshGL.veo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGL.veo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecVEO.size() * sizeof(uint32_t), &vecVEO[0], GL_STATIC_DRAW);

        // vao
        glGenVertexArrays( 1, &meshGL.vao);
        glBindVertexArray(meshGL.vao);

            // vbo
            glBindBuffer(GL_ARRAY_BUFFER,meshGL.vbo);
            ShaderLib::voxel -> setAttribute("position", 3, GL_FALSE, 10, 0);
            ShaderLib::voxel -> setAttribute("normal", 3, GL_FALSE, 10, 3);
            ShaderLib::voxel -> setAttribute("color"  , 4, GL_FALSE, 10, 6);

            // veo
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGL.veo);

        glBindVertexArray(0);

        meshGL.size = vecIndices.size();
    }
}
