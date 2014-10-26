#ifndef SHADERLIB_IJ0DKRT
#define SHADERLIB_IJ0DKRT

#include "graphic/Shader.hpp"

class ShaderLib
{
    public:
        static ShaderProgram* voxel;
        ShaderLib() 
        {
            voxel = & ShaderProgram::loadFromFile(
                "shader/voxel.vert",
                "shader/voxel.frag"
            );
        }
};

#endif /* end of include guard: SHADERLIB_IJ0DKRT */
