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

#include "ETCommon.h"
#include "ETIndexHandler.h"
#include <OgreVertexIndexData.h>
#include <OgreHardwareBufferManager.h>
#include <OgreException.h>
#include <cassert>

using namespace Ogre;
using namespace std;


namespace ET
{
  IndexHandler::IndexHandler(unsigned int sizeX, unsigned int sizeZ)
  : mSizeX(sizeX), mSizeZ(sizeZ), mUse32Bit(false)
  {
    // determine maximal level of detail possible with the given sizes
    findMaxLOD();
    // determine whether our data can fit into 16bit index buffers or not
    unsigned int numVertices = sizeX*sizeZ;
    if (numVertices > 65536)
      mUse32Bit = true;    
  }

  IndexHandler::~IndexHandler()
  {
    for (size_t i = 0; i < mIndexBuffers.size(); ++i)
    {
      for (IndexMap::iterator it = mIndexBuffers[i].begin(); it != mIndexBuffers[i].end(); ++it)
        delete it->second;
    }
  }

  void IndexHandler::findMaxLOD()
  {
    // determine max LOD in X dimension
    unsigned int maxLODX = (mSizeX != 1 ? 32 : 0);
    for (unsigned int n = 1; n < 32; ++n)
    {
      unsigned int size = (1 << n) + 1;
      if (size == mSizeX)
      {
        maxLODX = n;
        break;
      }
    }

    // determine max LOD in Z dimension
    unsigned int maxLODZ = (mSizeZ != 1 ? 32 : 0);
    for (unsigned int n = 1; n < 32; ++n)
    {
      unsigned int size = (1 << n) + 1;
      if (size == mSizeZ)
      {
        maxLODZ = n;
        break;
      }
    }

    if (maxLODX == 32 || maxLODZ == 32)
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Dimensions must satisfy (2^n)+1", "ET::IndexHandler::findMaxLOD");

    mMaxLOD = min(maxLODX, maxLODZ);
    mIndexBuffers = LODArray(mMaxLOD);
  }


  IndexData* IndexHandler::getIndexData(unsigned int ownLOD, unsigned int northLOD,
    unsigned int eastLOD, unsigned int southLOD, unsigned int westLOD)
  {
    assert(ownLOD < mIndexBuffers.size() && "Requested unexpected LOD");

    // derive map index for the neighbour's LOD constitution
    // only LODs higher than the requesting patch's LOD are of interest
    if (northLOD <= ownLOD)
      northLOD = 0;
    if (eastLOD <= ownLOD)
      eastLOD = 0;
    if (southLOD <= ownLOD)
      southLOD = 0;
    if (westLOD <= ownLOD)
      westLOD = 0;
    unsigned int mapIndex = (northLOD<<24) | (eastLOD<<16) | (southLOD<<8) | westLOD;

    IndexData* data = 0;
    IndexMap::iterator it = mIndexBuffers[ownLOD].find(mapIndex);
    if (it != mIndexBuffers[ownLOD].end())
    {
      // already created index data for this occasion
      data = it->second;
    }
    else
    {
      if (mUse32Bit)
        data = createIndexData<Ogre::uint32>(ownLOD, northLOD, eastLOD, southLOD, westLOD);
      else
        data = createIndexData<Ogre::uint16>(ownLOD, northLOD, eastLOD, southLOD, westLOD);
      mIndexBuffers[ownLOD].insert(IndexMap::value_type(mapIndex, data));
    }

    return data;
  }


  template<typename IndexType>
  IndexData* IndexHandler::createIndexData(unsigned int ownLOD, unsigned int northLOD,
    unsigned int eastLOD, unsigned int southLOD, unsigned int westLOD)
  {
    // for a chosen LOD n, only the (2^n)th vertices are included in the index buffer
    unsigned int step = 1 << ownLOD;
    // determine begin and end points in X and Z direction
    unsigned int startX = (westLOD ? step : 0);
    unsigned int startZ = (northLOD ? step : 0);
    unsigned int endX = mSizeX - 1 - (eastLOD ? step : 0);
    unsigned int endZ = mSizeZ - 1 - (southLOD ? step : 0);

    // determine the necessary precision of the index buffers
    HardwareIndexBuffer::IndexType it = 
      (mUse32Bit ? HardwareIndexBuffer::IT_32BIT : HardwareIndexBuffer::IT_16BIT);

    // estimate index buffer length
    size_t length = (mSizeX/step) * (mSizeZ/step) * 8;
    // create index data
    IndexData* indexData = new IndexData;
    indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
      it, length, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    IndexType* pIdx = static_cast<IndexType*>(indexData->indexBuffer->lock(
      0, indexData->indexBuffer->getSizeInBytes(), HardwareBuffer::HBL_DISCARD));

    unsigned int numIndexes = 0;
    // go over all the vertices and combine them to triangles in trilist format.
    // skip the borders if stitching is necessary
    for (unsigned int j = startZ; j < endZ; j += step)
    {
      for (unsigned int i = startX; i < endX; i += step)
      {
        *pIdx++ = index<IndexType>(i, j);
        *pIdx++ = index<IndexType>(i, j+step);
        *pIdx++ = index<IndexType>(i+step, j);

        *pIdx++ = index<IndexType>(i, j+step);
        *pIdx++ = index<IndexType>(i+step, j+step);
        *pIdx++ = index<IndexType>(i+step, j);

        numIndexes += 6;
      }
    }

    // stitch edges to neighbours with higher lod where needed
    if (northLOD > 0)
      numIndexes += stitchBorder<IndexType>(DIR_NORTH, ownLOD, northLOD, westLOD > 0, eastLOD > 0, &pIdx);
    if (eastLOD > 0)
      numIndexes += stitchBorder<IndexType>(DIR_EAST, ownLOD, eastLOD, northLOD > 0, southLOD > 0, &pIdx);
    if (southLOD > 0)
      numIndexes += stitchBorder<IndexType>(DIR_SOUTH, ownLOD, southLOD, eastLOD > 0, westLOD > 0, &pIdx);
    if (westLOD > 0)
      numIndexes += stitchBorder<IndexType>(DIR_WEST, ownLOD, westLOD, southLOD > 0, northLOD > 0, &pIdx);

    indexData->indexBuffer->unlock();
    indexData->indexCount = numIndexes;
    indexData->indexStart = 0;

    return indexData;
  }


  template<typename IndexType>
  int IndexHandler::stitchBorder(int direction, unsigned int hiLOD, unsigned int loLOD, bool omitFirstTri,
    bool omitLastTri, IndexType** ppIdx)
  {
    assert(loLOD > hiLOD);

    // code taken from Ogre's TSM
    // TODO: Rewrite this!!

    IndexType* pIdx = *ppIdx;

    int step = 1 << hiLOD;
    int superstep = 1 << loLOD;
    int halfsuperstep = superstep >> 1;
    int rowstep = 0;
    size_t startx = 0, starty = 0, endx = 0;
    bool horizontal = false;
    switch (direction)
    {
    case DIR_NORTH:
      startx = starty = 0;
      endx = mSizeX - 1;
      rowstep = step;
      horizontal = true;
      break;

    case DIR_SOUTH:
      startx = mSizeX-1;
      starty = mSizeZ-1;
      endx = 0;
      rowstep = -step;
      step = -step;
      superstep = -superstep;
      halfsuperstep = -halfsuperstep;
      horizontal = true;
      break;

    case DIR_EAST:
      startx = 0;
      endx = mSizeZ-1;
      starty = mSizeX-1;
      rowstep = -step;
      horizontal = false;
      break;

    case DIR_WEST:
      startx = mSizeZ-1;
      endx = 0;
      starty = 0;
      rowstep = step;
      step = -step;
      superstep = -superstep;
      halfsuperstep = -halfsuperstep;
      horizontal = false;
      break;
    }

    unsigned int numIndexes = 0;

    for (size_t j = startx; j != endx; j += superstep)
    {
      int k;
      for (k = 0; k != halfsuperstep; k += step)
      {
        size_t jk = j + k;
        if (j != startx || k != 0 || !omitFirstTri)
        {
          if (horizontal)
          {
            *pIdx++ = index<IndexType>(j, starty);
            *pIdx++ = index<IndexType>(jk, starty + rowstep);
            *pIdx++ = index<IndexType>(jk + step, starty + rowstep);
          }
          else
          {
            *pIdx++ = index<IndexType>(starty, j);
            *pIdx++ = index<IndexType>(starty+rowstep, jk);
            *pIdx++ = index<IndexType>(starty+rowstep, jk+step);
          }
          numIndexes += 3;
        }
      }

      if (horizontal)
      {
        *pIdx++ = index<IndexType>(j, starty);
        *pIdx++ = index<IndexType>(j+halfsuperstep, starty+rowstep);
        *pIdx++ = index<IndexType>(j+superstep, starty);
      }
      else
      {
        *pIdx++ = index<IndexType>(starty, j);
        *pIdx++ = index<IndexType>(starty+rowstep, j+halfsuperstep);
        *pIdx++ = index<IndexType>(starty, j+superstep);
      }
      numIndexes += 3;

      for (k = halfsuperstep; k != superstep; k += step)
      {
        size_t jk = j + k;
        if (j != endx - superstep || k != superstep - step || !omitLastTri)
        {
          if (horizontal)
          {
            *pIdx++ = index<IndexType>(j+superstep, starty);
            *pIdx++ = index<IndexType>(jk, starty+rowstep);
            *pIdx++ = index<IndexType>(jk+step, starty+rowstep);
          }
          else
          {
            *pIdx++ = index<IndexType>(starty, j+superstep);
            *pIdx++ = index<IndexType>(starty+rowstep, jk);
            *pIdx++ = index<IndexType>(starty+rowstep, jk+step);
          }
          numIndexes += 3;
        }
      }
    }

    *ppIdx = pIdx;

    return numIndexes;
  }


  template <typename IndexType>
  IndexType IndexHandler::index(size_t x, size_t z) const
  {
    return (IndexType) (x + z * mSizeX);
  }

}
