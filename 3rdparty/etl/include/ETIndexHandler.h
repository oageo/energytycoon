#ifndef __ET_INDEXHANDLER_H__
#define __ET_INDEXHANDLER_H__
/********************************************************************************
EDITABLE TERRAIN LIBRARY v3 for Ogre

Copyright (c) 2008 Holger Frydrych <frydrych@oddbeat.de>

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
********************************************************************************/

#include <vector>
#include <map>

namespace Ogre
{
  class IndexData;
}


namespace ET
{
  /**
   * The IndexHandler creates and manages index buffers for different LOD
   * for Terrain Patches of a given size.
   */
  class IndexHandler
  {
  public:
    /**
     * Creates a new IndexHandler for the given size and maximal number of LODs.
     * @param sizeX Number of vertices in X dimension
     * @param sizeZ Number of vertices in Z dimension
     */
    IndexHandler(unsigned int sizeX, unsigned int sizeZ);
    ~IndexHandler();

    /**
     * Request index buffer for a given state of LOD.
     * @param ownLOD level of detail of the requesting patch
     * @param northLOD Neighbouring patch's LOD to the north
     * @param eastLOD Neighbouring patch's LOD to the east
     * @param southLOD Neighbouring patch's LOD to the south
     * @param westLOD Neighbouring patch's LOD to the west
     */
    Ogre::IndexData* getIndexData(unsigned int ownLOD, unsigned int northLOD,
      unsigned int eastLOD, unsigned int southLOD, unsigned int westLOD);

    /** Return the size in X dimension */
    unsigned int getSizeX() const { return mSizeX; }
    /** Return the size in Z dimension */
    unsigned int getSizeZ() const { return mSizeZ; }
    /** Return maximal level of detail possible */
    unsigned int getMaxLOD() const { return mMaxLOD; }

  private:
    /** Determine maximal level of detail from given sizes */
    void findMaxLOD();
    /** Create new index data to the given LOD state */
    template<typename IndexType>
    Ogre::IndexData* createIndexData(unsigned int ownLOD, unsigned int northLOD,
      unsigned int eastLOD, unsigned int southLOD, unsigned int westLOD);
    template<typename IndexType>
    int stitchBorder(int direction, unsigned int hiLOD, unsigned int loLOD, bool omitFirstTri,
      bool omitLastTri, IndexType** ppIdx);
    template<typename IndexType>
    IndexType index(size_t x, size_t z) const;

    unsigned int mSizeX, mSizeZ;
    unsigned int mMaxLOD;
    typedef std::map<unsigned int, Ogre::IndexData*> IndexMap;
    typedef std::vector<IndexMap> LODArray;
    LODArray mIndexBuffers;
    bool mUse32Bit;
  };
}

#endif
