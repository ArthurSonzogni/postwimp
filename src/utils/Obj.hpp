// created by Arthur Sonzogni

#ifndef OBJ_YFJJB6N0
#define OBJ_YFJJB6N0

#include <GL/glew.h>
#include "utils/glError.hpp"

class Obj
{
    public:
        Obj(const char * filename);
        ~Obj();
        GLuint getVBO() const;
        unsigned int getNbFace() const;

        struct Dimension
        {
            float xmin,ymin,zmin;
            float xmax,ymax,zmax;
        };
        
        Dimension getDimension();

    private:
        GLuint vbo;
        unsigned int faceNb;
        Dimension dimension;
        
};

#endif /* end of include guard: OBJ_YFJJB6N0 */
