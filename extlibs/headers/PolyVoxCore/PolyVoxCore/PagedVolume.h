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

#ifndef __PolyVox_PagedVolume_H__
#define __PolyVox_PagedVolume_H__

#include "PolyVoxCore/BaseVolume.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Vector.h"

#include <limits>
#include <cstdlib> //For abort()
#include <cstring> //For memcpy
#include <unordered_map>
#include <list>
#include <map>
#include <memory>
#include <stdexcept> //For invalid_argument
#include <vector>

namespace PolyVox
{
	/// The PagedVolume class provides a memory efficient method of storing voxel data while also allowing fast access and modification.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// A PagedVolume is essentially a 3D array in which each element (or <i>voxel</i>) is identified by a three dimensional (x,y,z) coordinate.
	/// We use the PagedVolume class to store our data in an efficient way, and it is the input to many of the algorithms (such as the surface
	/// extractors) which form the heart of PolyVox. The PagedVolume class is templatised so that different types of data can be stored within each voxel.
	///
	/// Basic usage
	/// -----------
	///
	/// The following code snippet shows how to construct a volume and demonstrates basic usage:
	///
	/// \code
	/// PagedVolume<int> volume(Region(Vector3DInt32(0,0,0), Vector3DInt32(63,127,255)));
	/// volume.setVoxel(15, 90, 42, int(5));
	/// std::cout << "Voxel at (15, 90, 42) has value: " << volume.getVoxel(15, 90, 42) << std::endl;
	/// std::cout << "Width = " << volume.getWidth() << ", Height = " << volume.getHeight() << ", Depth = " << volume.getDepth() << std::endl;
	/// \endcode
	/// 
	/// The PagedVolume constructor takes a Region as a parameter. This specifies the valid range of voxels which can be held in the volume, so in this
	/// particular case the valid voxel positions are (0,0,0) to (63, 127, 255). The result of attempts to access voxels outside this range will result
	/// are defined by the WrapMode). PolyVox also has support for near infinite volumes which will be discussed later.
	/// 
	/// Access to individual voxels is provided via the setVoxel() and getVoxel() member functions. Advanced users may also be interested in
	/// the Sampler nested class for faster read-only access to a large number of voxels.
	/// 
	/// Lastly the example prints out some properties of the PagedVolume. Note that the dimentsions getWidth(), getHeight(), and getDepth() are inclusive, such
	/// that the width is 64 when the range of valid x coordinates goes from 0 to 63.
	/// 
	/// Data Representaion
	/// ------------------
	/// If stored carelessly, volume data can take up a huge amount of memory. For example, a volume of dimensions 1024x1024x1024 with
	/// 1 byte per voxel will require 1GB of memory if stored in an uncompressed form. Natuarally our PagedVolume class is much more efficient
	/// than this and it is worth understanding (at least at a high level) the approach which is used.
	///
	/// Essentially, the PagedVolume class stores its data as a collection of chunks. Each of these chunk is much smaller than the whole volume,
	/// for example a typical size might be 32x32x32 voxels (though is is configurable by the user). In this case, a 256x512x1024 volume
	/// would contain 8x16x32 = 4096 chunks. Typically these chunks do not need to all be in memory all the time, and the Pager class can 
	/// be used to control how they are loaded and unloaded. This mechanism allows a
	/// potentially unlimited amount of data to be loaded, provided the user is able to take responsibility for storing any data which PolyVox
	/// cannot fit in memory, and then returning it back to PolyVox on demand. For example, the user might choose to temporarily store this data
	/// on disk or stream it to a remote database.
	///
	/// Essentially you are providing an extension to the PagedVolume class - a way for data to be stored once PolyVox has run out of memory for it. Note
	/// that you don't actually have to do anything with the data - you could simply decide that once it gets removed from memory it doesn't matter
	/// anymore.
	///
	/// Cache-aware traversal
	/// ---------------------
	/// *NOTE: This needs updating for PagedVolume rather than the old LargeVolume*
	/// You might be suprised at just how many cache misses can occur when you traverse the volume in a naive manner. Consider a 1024x1024x1024 volume
	/// with chunks of size 32x32x32. And imagine you iterate over this volume with a simple three-level for loop which iterates over x, the y, then z.
	/// If you start at position (0,0,0) then ny the time you reach position (1023,0,0) you have touched 1024 voxels along one edge of the volume and
	/// have pulled 32 chunks into the cache. By the time you reach (1023,1023,0) you have hit 1024x1024 voxels and pulled 32x32 chunks into the cache.
	/// You are now ready to touch voxel (0,0,1) which is right next to where you started, but unless your cache is at least 32x32 chunks large then this
	/// initial chunk has already been cleared from the cache.
	///
	/// Ensuring you have a large enough cache size can obviously help the above situation, but you might also consider iterating over the voxels in a
	/// different order. For example, if you replace your three-level loop with a six-level loop then you can first process all the voxels between (0,0,0)
	/// and (31,31,31), then process all the voxels between (32,0,0) and (63,0,0), and so forth. Using this approach you will have no cache misses even
	/// is your cache size is only one. Of course the logic is more complex, but writing code in such a cache-aware manner may be beneficial in some situations.
	///
	/// Threading
	/// ---------
	/// The PagedVolume class does not make any guarentees about thread safety. You should ensure that all accesses are performed from the same thread.
	/// This is true even if you are only reading data from the volume, as concurrently reading from different threads can invalidate the contents
	/// of the chunk cache (amoung other problems).
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	class PagedVolume : public BaseVolume<VoxelType>
	{
	public:
		/// The PagedVolume stores it data as a set of Chunk instances which can be loaded and unloaded as memory requirements dictate.
		class Chunk;
		/// The Pager class is responsible for the loading and unloading of Chunks, and can be overridden by the user.
		class Pager;

		class Chunk
		{
			friend class PagedVolume;

		public:
			Chunk(Vector3DInt32 v3dPosition, uint16_t uSideLength, Pager* pPager = nullptr);
			~Chunk();

			VoxelType* getData(void) const;
			uint32_t getDataSizeInBytes(void) const;

			VoxelType getVoxel(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
			VoxelType getVoxel(const Vector3DUint16& v3dPos) const;

			void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
			void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		private:
			/// Private copy constructor to prevent accisdental copying
			Chunk(const Chunk& /*rhs*/) {};

			/// Private assignment operator to prevent accisdental copying
			Chunk& operator=(const Chunk& /*rhs*/) {};

			// This is updated by the PagedVolume and used to discard the least recently used chunks.
			uint32_t m_uChunkLastAccessed;

			// This is so we can tell whether a uncompressed chunk has to be recompressed and whether
			// a compressed chunk has to be paged back to disk, or whether they can just be discarded.
			bool m_bDataModified;

			uint32_t calculateSizeInBytes(void);
			static uint32_t calculateSizeInBytes(uint32_t uSideLength);

			VoxelType* m_tData;
			uint16_t m_uSideLength;
			uint8_t m_uSideLengthPower;
			Pager* m_pPager;

			// Note: Do we really need to store this position here as well as in the block maps?
			Vector3DInt32 m_v3dChunkSpacePosition;
		};

		/**
		* Users can override this class and provide an instance of the derived class to the PagedVolume constructor. This derived class
		* could then perform tasks such as compression and decompression of the data, and read/writing it to a file, database, network,
		* or other storage as appropriate. See FilePager for a simple example of such a derived class.
		*/
		class Pager
		{
		public:
			/// Constructor
			Pager() {};
			/// Destructor
			virtual ~Pager() {};

			virtual void pageIn(const Region& region, Chunk* pChunk) = 0;
			virtual void pageOut(const Region& region, Chunk* pChunk) = 0;
		};

		//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
		//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
		//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
		//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
		//in the future
		//typedef Volume<VoxelType> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences.
		//class Sampler : public VolumeOfVoxelType::template Sampler< PagedVolume<VoxelType> >
		#ifndef SWIG
#if defined(_MSC_VER)
		class Sampler : public BaseVolume<VoxelType>::Sampler< PagedVolume<VoxelType> > //This line works on VS2010
#else
                class Sampler : public BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> > //This line works on GCC
#endif
		{
		public:
			Sampler(PagedVolume<VoxelType>* volume);
			~Sampler();

			/// \deprecated
			POLYVOX_DEPRECATED VoxelType getSubSampledVoxel(uint8_t uLevel) const;
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
		PagedVolume
		(
			const Region& regValid,	
			Pager* pPager = nullptr,	
			uint16_t uChunkSideLength = 32
		);
		/// Destructor
		~PagedVolume();

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

		/// Sets the number of chunks for which uncompressed data is stored
		void setMemoryUsageLimit(uint32_t uMemoryUsageInBytes);
		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		void setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue, WrapMode eWrapMode = WrapModes::Validate);
		/// Sets the voxel at the position given by a 3D vector
		void setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue, WrapMode eWrapMode = WrapModes::Validate);
		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		bool setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at the position given by a 3D vector
		bool setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue);
		/// Tries to ensure that the voxels within the specified Region are loaded into memory.
		void prefetch(Region regPrefetch);
		/// Ensures that any voxels within the specified Region are removed from memory.
		void flush(Region regFlush);
		/// Removes all voxels from memory
		void flushAll();

		/// Calculates approximatly how many bytes of memory the volume is currently using.
		uint32_t calculateSizeInBytes(void);

	protected:
		/// Copy constructor
		PagedVolume(const PagedVolume& rhs);

		/// Assignment operator
		PagedVolume& operator=(const PagedVolume& rhs);

	private:

		// FIXME - We can probably ove this into the constructor
		void initialise();

		// A trick to implement specialization of template member functions in template classes. See http://stackoverflow.com/a/4951057
		template <WrapMode eWrapMode>
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<eWrapMode>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Validate>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Clamp>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Border>, VoxelType tBorder) const;
		VoxelType getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::AssumeValid>, VoxelType tBorder) const;
	
		std::shared_ptr<Chunk> getChunk(int32_t uChunkX, int32_t uChunkY, int32_t uChunkZ) const;

		void purgeNullPtrsFromAllChunks(void) const;

		// The chunk data is stored in the pair of maps below. This will often hold the same set of chunks but there are occasions
		// when they can differ (more on that in a moment). The main store is the set of 'recently used chunks' which holds shared_ptr's
		// to the chunk data. When memory is low chunks can be removed from this list and, assuming there are no more references to
		// them, they will be deleted. However, it is also possible that a Sampler is pointing at a given chunk, and in this case the
		// reference counting will ensure that the chunk survives until the sampler has finished with it.
		//
		// However, this leaves us open to a subtle bug. If a chunk is removed from the recently used set, continues to exist due to a
		// sampler using it, and then the user tries to access it through the volume interface, then the volume will page the chunk
		// back in (not knowing the sampler still has it). This would result in two chunks in memory with the same position is space.
		// To avoid this, the 'all chunks' set tracks chunks with are used by the volume *and/or* the samplers. It holds weak pointers
		// so does not cause chunks to persist.
		//
		// TODO: Do we really need maps here? It means we are storing the chunk positions in the map, but they are also stored in the
		// chunks themselves (so they can be passed to the pager from the chunks destructor). Can we use a set here? What is a better approach?
		typedef std::unordered_map<Vector3DInt32, std::weak_ptr< Chunk > > WeakPtrChunkMap;
		mutable WeakPtrChunkMap m_pAllChunks;
		typedef std::unordered_map<Vector3DInt32, std::shared_ptr< Chunk > > SharedPtrChunkMap;
		mutable SharedPtrChunkMap m_pRecentlyUsedChunks;

		mutable uint32_t m_uTimestamper;
		mutable Vector3DInt32 m_v3dLastAccessedChunkPos;
		mutable std::shared_ptr<Chunk> m_pLastAccessedChunk;
		uint32_t m_uChunkCountLimit;

		// The size of the volume
		Region m_regValidRegionInChunks;

		// The size of the chunks
		uint16_t m_uChunkSideLength;
		uint8_t m_uChunkSideLengthPower;

		Pager* m_pPager;

		// Enough to make sure a chunks and it's neighbours can be loaded, with a few to spare.
		const uint32_t uMinPracticalNoOfChunks = 32;
		// Should prevent multi-gigabyte volumes when chunk sizes are reasonable.
		const uint32_t uMaxPracticalNoOfChunks = 32768;
	};
}

#include "PolyVoxCore/PagedVolume.inl"
#include "PolyVoxCore/PagedVolumeChunk.inl"
#include "PolyVoxCore/PagedVolumeSampler.inl"

#endif //__PolyVox_PagedVolume_H__
