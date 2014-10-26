#ifndef SHADERLIB_IJ0DKRT
#define SHADERLIB_IJ0DKRT

#include "graphic/Shader.hpp"

class ShaderLib
{
    public:
        static ShaderProgram* geometry;
        static ShaderProgram* geometryColor;
        static ShaderProgram* compose;

        ShaderLib() 
        {
            geometry = & ShaderProgram::loadFromFile(
                "shader/geometryPass.vert",
                "shader/geometryPass.frag"
            );

            geometryColor = & ShaderProgram::loadFromFile(
                "shader/geometryPassColor.vert",
                "shader/geometryPassColor.frag"
            );

            compose = & ShaderProgram::loadFromFile(
                "shader/compose.vert",
                "shader/compose.frag"
            );
        }
};

#endif /* end of include guard: SHADERLIB_IJ0DKRT */
