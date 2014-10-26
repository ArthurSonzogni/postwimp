#include "VoxelMapDisplayer.hpp"

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
        mesh[i] = {0,0,0};
    }
}

void VoxelMapDisplayer::display()
{
    for(uint32_t i = 0 ; i < nbBlockZ ; ++i)
    {
        MeshGL& m = mesh[i]; 
        if (m.size)
        {
            glBindVertexArray(m.vao);
            glDrawArrays(
                GL_TRIANGLES,      // mode
                0,                 // first
                m.size             // count
            );
        }
    }
    glBindVertexArray(0);
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
        PolyVox::Vector3DInt32((x+0)*nbBlockX,(y+0)*nbBlockY,(z+0)*nbBlockZ),
        PolyVox::Vector3DInt32((x+1)*nbBlockX,(y+1)*nbBlockY,(z+1)*nbBlockZ)
    );

    PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> m;

    voxelMap.extract(region, m);

	const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = m.getVertices();
	const std::vector<uint32_t>& vecIndices = m.getIndices();

    /*
    if (meshGL.size() != 0)
    {
        glDeleteVertexArrays(1,&meshGL.vao);
        glDeleteBuffers(1,&meshGL.vbo);
        meshGL = {0,0,0};
    }

    if (vecVertices.size() > 0)
    {
        // vbo
        glGenBuffers( 1, &meshGL.vbo );
        glBindBuffer(GL_ARRAY_BUFFER, meshGL.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLVertice)*vecVBO.size(), &vecVBO[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vao
        glGenVertexArrays( 1, &meshGL.vao);
        glBindVertexArray(vaoGhost);

            // vbo
            glBindBuffer(GL_ARRAY_BUFFER,meshGL.vbo);
            ShaderLib::geometry -> setAttribute("position", 3, GL_FALSE, 10, 0);
            ShaderLib::geometry -> setAttribute("normal", 3, GL_FALSE, 10, 3);
            ShaderLib::geometry -> setAttribute("color"  , 4, GL_FALSE, 10, 6);

        glBindVertexArray(0);
    }
    */
}
