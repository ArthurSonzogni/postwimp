/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __PolyVox_RawVolume_H__
#define __PolyVox_RawVolume_H__

#include "PolyVoxCore/BaseVolume.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Vector.h"

#include <cstdlib> //For abort()
#include <limits>
#include <memory>
#include <stdexcept> //For invalid_argument

namespace PolyVox
{
	template <typename VoxelType>
	class RawVolume : public BaseVolume<VoxelType>
	{
	public:
		#ifndef SWIG
		//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
		//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
		//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
		//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
		//in the future
		//typedef Volume<VoxelType> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences.
		//class Sampler : public VolumeOfVoxelType::template Sampler< RawVolume<VoxelType> >
#if defined(_MSC_VER)
		class Sampler : public BaseVolume<VoxelType>::Sampler< RawVolume<VoxelType> > //This line works on VS2010
#else
                class Sampler : public BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> > //This line works on GCC
#endif
		{
		public:
			Sampler(RawVolume<VoxelType>* volume);
			~Sampler();

			inline VoxelType getVoxel(void) const;			

			void setPosition(const Vector3DInt32& v3dNewPos);
			void setPosition(int32_t xPos, int32_t yPos, int32_t zPos);
			inline bool setVoxel(VoxelType tValue);

			void movePositiveX(void);
			void movePositiveY(void);
			void movePositiveZ(void);

			void moveNegativeX(void);
			void moveNegativeY(void);
			void moveNegativeZ(void);

			inline VoxelType peekVoxel1nx1ny1nz(void) const;
			inline VoxelType peekVoxel1nx1ny0pz(void) const;
			inline VoxelType peekVoxel1nx1ny1pz(void) const;
			inline VoxelType peekVoxel1nx0py1nz(void) const;
			inline VoxelType peekVoxel1nx0py0pz(void) const;
			inline VoxelType peekVoxel1nx0py1pz(void) const;
			inline VoxelType peekVoxel1nx1py1nz(void) const;
			inline VoxelType peekVoxel1nx1py0pz(void) const;
			inline VoxelType peekVoxel1nx1py1pz(void) const;

			inline VoxelType peekVoxel0px1ny1nz(void) const;
			inline VoxelType peekVoxel0px1ny0pz(void) const;
			inline VoxelType peekVoxel0px1ny1pz(void) const;
			inline VoxelType peekVoxel0px0py1nz(void) const;
			inline VoxelType peekVoxel0px0py0pz(void) const;
			inline VoxelType peekVoxel0px0py1pz(void) const;
			inline VoxelType peekVoxel0px1py1nz(void) const;
			inline VoxelType peekVoxel0px1py0pz(void) const;
			inline VoxelType peekVoxel0px1py1pz(void) const;

			inline VoxelType peekVoxel1px1ny1nz(void) const;
			inline VoxelType peekVoxel1px1ny0pz(void) const;
			inline VoxelType peekVoxel1px1ny1pz(void) const;
			inline VoxelType peekVoxel1px0py1nz(void) const;
			inline VoxelType peekVoxel1px0py0pz(void) const;
			inline VoxelType peekVoxel1px0py1pz(void) const;
			inline VoxelType peekVoxel1px1py1nz(void) const;
			inline VoxelType peekVoxel1px1py0pz(void) const;
			inline VoxelType peekVoxel1px1py1pz(void) const;

		private:

			//Other current position information
			VoxelType* mCurrentVoxel;
		};
		#endif

	public:
		/// Constructor for creating a fixed size volume.
		RawVolume(const Region& regValid);

		/// Destructor
		~RawVolume();

		/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
		template <WrapMode eWrapMode>
		VoxelType getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tBorder = VoxelType()) const;
		/// Gets a voxel at the position given by a 3D vector
		template <WrapMode eWrapMode>
		VoxelType getVoxel(const Vector3DInt32& v3dPos, VoxelType tBorder = VoxelType()) const;

		/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
		VoxelType getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapMode eWrapMode = WrapModes::Validate, VoxelType tBorder = VoxelType()) const;
		/// Gets a voxel at the position given by a 3D vector
		VoxelType getVoxel(const Vector3DInt32& v3dPos, WrapMode eWrapMode = WrapModes::Validate, VoxelType tBorder = VoxelType()) const;

		/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
		POLYVOX_DEPRECATED VoxelType getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const;
		/// Gets a voxel at the position given by a 3D vector
		POLYVOX_DEPRECATED VoxelType getVoxelAt(const Vector3DInt32& v3dPos) const;

		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		void setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue, WrapMode eWrapMode = WrapModes::Validate);
		/// Sets the voxel at the position given by a 3D vector
		void setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue, WrapMode eWrapMode = WrapModes::Validate);
		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		bool setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at the position given by a 3D vector
		bool setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue);

		/// Calculates approximatly how many bytes of memory the volume is currently using.
		uint32_t calculateSizeInBytes(void);

	protected:
		/// Copy constructor
		RawVolume(const RawVolume& rhs);

		/// Assignment operator
		RawVolume& operator=(const RawVolume& rhs);

	private:
		void initialise(const Region& regValidRegion);

		// A trick to implement specialization of template member functions in template classes. See http://stackoverflow.com/a/4951057
		template <WrapMode eWrapMode>
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<eWrapMode>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Validate>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Clamp>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Border>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::AssumeValid>, VoxelType tBorder) const;

		//The voxel data
		VoxelType* m_pData;
	};
}

#include "PolyVoxCore/RawVolume.inl"
#include "PolyVoxCore/RawVolumeSampler.inl"

#endif //__PolyVox_RawVolume_H__
