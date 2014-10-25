#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <iostream>
using namespace std;

//Use the PolyVox namespace
using namespace PolyVox;

float max(float a, float b)
{
    return (a<b)?a:b;
}

float sqr(float a)
{
    return a*a;
}


void createSphereInVolume(SimpleVolume<uint8_t>& volData, float fRadius)
{
	//This vector hold the position of the center of the volume
	Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
                

                float xx = x-32.0;
                float yy = y-32.0;
                float zz = z-32.0;

                float c = cos(zz*0.1);
                float s = sin(zz*0.1);

                float targetX1 = 20*c;
                float targetY1 = 20*s;

                float targetX2 = -20*c;
                float targetY2 = -20*s;

                float xxx1 = xx-targetX1;
                float yyy1 = yy-targetY1;

                float xxx2 = xx-targetX2;
                float yyy2 = yy-targetY2;

                float xxxx1 = + xxx1 * c + yyy1 * s;
                float yyyy1 = - xxx1 * s + yyy1 * c;

                float xxxx2 = + xxx2 * c + yyy2 * s;
                float yyyy2 = - xxx2 * s + yyy2 * c;

                float iso1 = 2.0*xxxx1*xxxx1 + yyyy1*yyyy1 - 200;
                float iso2 = 2.0*xxxx2*xxxx2 + yyyy2*yyyy2 - 200;
                float level = 128+1.0*max(iso1,iso2);
                if (level<0.0) level = 0.0;
                if (level>255.0) level = 255.0;
                uint8_t uVoxelValue = 255-level;


				//Wrte the voxel value into the volume	
				volData.setVoxelAt(x, y, z, uVoxelValue);
			}
		}
	}
}
#include <ctime>

  using namespace std;

int main()
{
    int H = 64;

	//Create an empty volume and then place a sphere in it
	SimpleVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(H-1,H-1,H-1)));
	createSphereInVolume(volData, 30);

  clock_t begin = clock();
	//A mesh object to hold the result of surface extraction
	SurfaceMesh<PositionMaterialNormal> mesh;


	//Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
    //CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
    MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
    //MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(&volData, PolyVox::Region(
        //Vector3DInt32(0,0,0),
        //Vector3DInt32(32,32,32)
    //), &mesh);

	//Execute the surface extractor.
	surfaceExtractor.execute();

  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<elapsed_secs<<endl;

	return EXIT_SUCCESS;
}
