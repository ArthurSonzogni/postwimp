#ifndef VOXEL_P271X96W
#define VOXEL_P271X96W
#include "utils/glm.hpp"

//#include "PolyVoxCore/DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type
//#include "PolyVoxCore/DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Mesh.h"
#include "PolyVoxCore/Vertex.h"

typedef PolyVox::MaterialDensityPair<uint32_t, 8*4, 8> Voxel;
typedef PolyVox::Mesh<PolyVox::Vertex<Voxel>,uint32_t> Mesh;

namespace PolyVox
{
    template<>
    class DefaultMarchingCubesController<Voxel>
    {
    public:
        typedef uint8_t DensityType;
        typedef uint32_t MaterialType;

        DefaultMarchingCubesController(void)
        {	
        }
        DensityType convertToDensity(Voxel voxel)
        {
            return voxel.getDensity();
        }
        MaterialType convertToMaterial(Voxel voxel)
        {
            return voxel.getMaterial();
        }

        Voxel blendMaterials(Voxel a, Voxel b, float weight)
        {
            MaterialType m1 = a.getMaterial();
            MaterialType m2 = b.getMaterial();

            float a1 = m1&0xFF; m1>>=8;
            float b1 = m1&0xFF; m1>>=8;
            float g1 = m1&0xFF; m1>>=8;
            float r1 = m1&0xFF; m1>>=8;

            float a2 = m2&0xFF; m2>>=8;
            float b2 = m2&0xFF; m2>>=8;
            float g2 = m2&0xFF; m2>>=8;
            float r2 = m2&0xFF; m2>>=8;

            uint8_t cr = r1*(1.0-weight)+r2*weight;            
            uint8_t cg = g1*(1.0-weight)+g2*weight;            
            uint8_t cb = b1*(1.0-weight)+b2*weight;            
            uint8_t ca = a1*(1.0-weight)+a2*weight;            

            MaterialType m = ca + (cb<<8) + (cg<<16) + (cr<<24);

            return Voxel(
                    m,
                    a.getDensity()*(1.0-weight)+
                    b.getDensity()*weight
            );
        }

        DensityType getThreshold(void)
        {
            return 128;
        }

    private:
        uint8_t m_tThreshold;
    };
}



#endif /* end of include guard: VOXEL_P271X96W */
