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
        mesh[i] = {0,0,0,0,{0,0,0}};
    }
}

void VoxelMapDisplayer::display()
{
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable( GL_BLEND );
    //glDisable( GL_DEPTH_TEST);
    ShaderLib::voxel->use();
    static float j = 0.0;
    j+=0.1;
    for(int32_t i = 0 ; i < nbBlock ; ++i)
    {
        MeshGL& m = mesh[i]; 
        if (m.size)
        {
            ShaderLib::voxel -> setUniform("model", glm::translate(glm::mat4(1.0),m.translation*1.01f));
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


    auto m = voxelMap.extract(region);
    //voxelMap.extract(region, m);

    const auto& vecVertices = m.getVertices();
    const auto& vecIndices= m.getIndices();

    if (meshGL.size != 0)
    {
        glDeleteVertexArrays(1,&meshGL.vao);
        glDeleteBuffers(1,&meshGL.vbo);
        glDeleteBuffers(1,&meshGL.veo);
        meshGL = {0,0,0,0,{0,0,0}};
    }

    if (vecVertices.size() > 0)
    {
		// vao
        glGenVertexArrays( 1, &meshGL.vao);
        glBindVertexArray(meshGL.vao);

            // vbo
            glGenBuffers( 1, &meshGL.vbo );
            glBindBuffer(GL_ARRAY_BUFFER, meshGL.vbo);
            glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(Mesh::VertexType), vecVertices.data(), GL_STATIC_DRAW);

            // veo
            glGenBuffers(1, &meshGL.veo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGL.veo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(Mesh::IndexType), vecIndices.data(), GL_STATIC_DRAW);

            GLuint positionLoc = ShaderLib::voxel -> attribLocation("position");
            GLuint   normalLoc = ShaderLib::voxel -> attribLocation("normal");
            GLuint    colorLoc = ShaderLib::voxel -> attribLocation("color");

            // vbo name mapping
            // Position
            glEnableVertexAttribArray(positionLoc);
            glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::VertexType), (GLvoid*)(offsetof(Mesh::VertexType, position))); 
            // Normal
            glEnableVertexAttribArray(normalLoc);
            glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::VertexType), (GLvoid*)(offsetof(Mesh::VertexType, normal)));
            glEnableVertexAttribArray(colorLoc); 

            // Color
            glEnableVertexAttribArray(colorLoc);
            GLint size = (std::min)(sizeof(Mesh::VertexType::DataType), size_t(4));
            glVertexAttribIPointer(colorLoc, size, GL_UNSIGNED_BYTE, sizeof(Mesh::VertexType), (GLvoid*)(offsetof(Mesh::VertexType, data)));

            //cout<<"----"<<endl;
            //cout<<positionLoc<<endl;
            //cout<<normalLoc<<endl;
            //cout<<colorLoc<<endl;
            //cout<<sizeof(Mesh::VertexType)<<endl;

            //ShaderLib::voxel -> setAttribute("position", 3, GL_FALSE, sizeof(Mesh::VertexType)/sizeof(GL_FLOAT),offsetof(Mesh::VertexType, position)/sizeof(GL_FLOAT));
            //ShaderLib::voxel -> setAttribute("normal", 3, GL_FALSE, sizeof(Mesh::VertexType)/sizeof(GL_FLOAT),  offsetof(Mesh::VertexType, normal)/sizeof(GL_FLOAT));
            //ShaderLib::voxel -> setAttribute("color"  , 4, GL_FALSE, sizeof(Mesh::VertexType), offsetof(Mesh::VertexType, data));

		// vao end
		glBindVertexArray(0);

        meshGL.size = vecIndices.size();
        meshGL.translation = {xmin,ymin,zmin};
    }
}
