#include "VoxelMapDisplayer.hpp"
#include "ShaderLib.hpp"
#include <iostream>
using namespace std;

VoxelMapDisplayer::VoxelMapDisplayer(VoxelMap& voxelMap,uint32_t bufferSize):
    voxelMap(voxelMap),
    bufferSize(bufferSize)
{
    sizeX = voxelMap.getSizeX();
    sizeY = voxelMap.getSizeY();
    sizeZ = voxelMap.getSizeZ();
    nbBlockX = (sizeX+bufferSize-1)/bufferSize;
    nbBlockY = (sizeY+bufferSize-1)/bufferSize;
    nbBlockZ = (sizeZ+bufferSize-1)/bufferSize;
    nbBlock = nbBlockX * nbBlockY * nbBlockZ;

    // allocate MeshGL
    mesh.reserve(nbBlock);
    for(int32_t i = 0 ; i<nbBlock ; ++i)
    {
        mesh[i] = {0,0,0,0};
    }
}

void VoxelMapDisplayer::display()
{
    ShaderLib::voxel->use();
    for(int32_t i = 0 ; i < nbBlock ; ++i)
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

    int32_t xmin,xmax;
    int32_t ymin,ymax;
    int32_t zmin,zmax;

    xmin = ( lowerCorner.getX() - 1 )/ bufferSize;
    ymin = ( lowerCorner.getY() - 1 )/ bufferSize;
    zmin = ( lowerCorner.getZ() - 1 )/ bufferSize;

    xmax = ( upperCorner.getX() + 1 )/ bufferSize;
    ymax = ( upperCorner.getY() + 1 )/ bufferSize;
    zmax = ( upperCorner.getZ() + 1 )/ bufferSize;

    if (xmin<0) xmin = 0;
    if (ymin<0) ymin = 0;
    if (zmin<0) zmin = 0;

    if (xmax>=nbBlockX) xmax = nbBlockX-1;
    if (ymax>=nbBlockY) ymax = nbBlockY-1;
    if (zmax>=nbBlockZ) zmax = nbBlockZ-1;

    for(int32_t z = zmin; z<=zmax; ++z)
    for(int32_t y = ymin; y<=ymax; ++y)
    for(int32_t x = xmin; x<=xmax; ++x)
        updateBox(x,y,z);
}

void VoxelMapDisplayer::updateBox(uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t pos =
        x+nbBlockX*(
        y+nbBlockY*(
        z));

    MeshGL& meshGL = mesh[pos];
    
    int32_t xmin = x*bufferSize;
    int32_t ymin = y*bufferSize;
    int32_t zmin = z*bufferSize;
    int32_t xmax = xmin+bufferSize;
    int32_t ymax = ymin+bufferSize;
    int32_t zmax = zmin+bufferSize;

    if (xmax>sizeX) xmax=sizeX;
    if (ymax>sizeY) ymax=sizeY;
    if (zmax>sizeZ) zmax=sizeZ;

    if (xmax<xmin) return;
    if (ymax<ymin) return;
    if (zmax<zmin) return;
    
    PolyVox::Region region(
        PolyVox::Vector3DInt32(xmin,ymin,zmin),
        PolyVox::Vector3DInt32(xmax,ymax,zmax)
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
