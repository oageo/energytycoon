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

#include "ETPatch.h"
#include "ETIndexHandler.h"
#include <OgreHardwareBufferManager.h>
#include <OgreNode.h>
#include <OgreTagPoint.h>
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreException.h>
#include <OgreMath.h>
#include <iostream>

using namespace Ogre;
using namespace std;


namespace ET
{
  const unsigned short MAIN_BINDING = 0;
  const unsigned short DELTA_BINDING = 1;
  const unsigned int MORPH_CUSTOM_PARAM_ID = 77;


  Patch::Patch(const String& name, TerrainDescription* description, bool autoDelete,
    IndexHandler* indexHandler, unsigned int maxLOD, Real maxError, unsigned int vertexOptions,
    Real lodMorphDistStart)
  : MovableObject(name), mDescription(description), mAutoDelete(autoDelete), mIndexHandler(indexHandler),
    mVertexOptions(vertexOptions), mMaxLOD(maxLOD), mMaxError(maxError), mCurLOD(0),
    mLODMorphDistStart(lodMorphDistStart), mLightListDirty(true)
  {
    if (mDescription->getNumVerticesX() != mIndexHandler->getSizeX() ||
      mDescription->getNumVerticesZ() != mIndexHandler->getSizeZ())
    {
      // sizes don't match
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Sizes of the terrain description and index handler don't match",
        "ET::Patch::Patch");
    }

    // initialise LOD cache
    unsigned int cacheStep = 1 << (mMaxLOD-1);
    unsigned int cacheSizeX = (mDescription->getNumVerticesX()-1) / cacheStep;
    unsigned int cacheSizeZ = (mDescription->getNumVerticesZ()-1) / cacheStep;
    mLODCache.assign(mMaxLOD-1, RealArray2D(cacheSizeX, cacheSizeZ, 0));

    memset(mNeighbours, 0, sizeof(mNeighbours));

    mMaxLOD = min(mMaxLOD, mIndexHandler->getMaxLOD());
    // disable LOD morphing if max LOD is 1
    if (mMaxLOD <= 1)
    {
      mMaxLOD = 1;
      mVertexOptions &= (~VO_LODMORPH);
    }
    mLODChangeMinDistSqr = vector<Real>(mMaxLOD);

    mLastNextLOD = mMaxLOD+1;

    // terrain should not cast shadows by default, too expensive
    mCastShadows = false;

    createVertexData();
    // initial fill of the vertex buffer (=> update all vertices)
    VertexList allVertices;
    for (unsigned int i = 0; i < mDescription->getNumVertices(); ++i)
      allVertices.insert(i);
    updateVertexBuffer(allVertices);

    // register as a listener to the TerrainDescription
    mDescription->addListener(this);
  }


  Patch::~Patch()
  {
    // notify neighbours
    for (int i = 0; i < 4; ++i)
    {
      if (mNeighbours[i])
        mNeighbours[i]->mNeighbours[OPPOSITE_DIR[i]] = 0;
    }
    if (mDescription)
      mDescription->removeListener(this);
    delete mVertexData;
  }


  void Patch::createVertexData()
  {
    mVertexData = new VertexData;
    mVertexData->vertexStart = 0;
    mVertexData->vertexCount = mDescription->getNumVertices();

    VertexDeclaration* decl = mVertexData->vertexDeclaration;
    VertexBufferBinding* bind = mVertexData->vertexBufferBinding;

    // first we need to declare the contents of our vertex buffer
    size_t offset = 0;
    decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    if (mVertexOptions & VO_NORMALS)
    {
      // include vertex normals
      decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_NORMAL);
      offset += VertexElement::getTypeSize(VET_FLOAT3);
    }
    if (mVertexOptions & VO_TANGENTS)
    {
      // include vertex tangents
      decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_TANGENT);
      offset += VertexElement::getTypeSize(VET_FLOAT3);
    }
    if (mVertexOptions & VO_BINORMALS)
    {
      // include vertex binormals
      decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_BINORMAL);
      offset += VertexElement::getTypeSize(VET_FLOAT3);
    }
    // add primary texcoord set
    decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    offset += VertexElement::getTypeSize(VET_FLOAT2);
    if (mVertexOptions & VO_TEXCOORD1)
    {
      // include secondary texcoord set
      decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
      offset += VertexElement::getTypeSize(VET_FLOAT2);
    }

    // create the primary vertex buffer
    mMainBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(
      decl->getVertexSize(MAIN_BINDING), mVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    // bind the buffer
    bind->setBinding(MAIN_BINDING, mMainBuffer);

    // declare and create delta buffers, if requested
    if (mVertexOptions & VO_LODMORPH)
    {
      decl->addElement(DELTA_BINDING, 0, VET_FLOAT1, VES_BLEND_WEIGHTS);
      for (unsigned int i = 0; i < mMaxLOD-1; ++i)
      {
        HardwareVertexBufferSharedPtr buf = HardwareBufferManager::getSingleton().
          createVertexBuffer(VertexElement::getTypeSize(VET_FLOAT1), 
            mDescription->getNumVertices(),
            HardwareBuffer::HBU_STATIC_WRITE_ONLY);
        mDeltaBuffers.push_back(buf);
        emptyBuffer(buf);
      }
    }
  }


  void Patch::updateVertexBuffer(const VertexList& dirtyVertices)
  {
    VertexDeclaration* decl = mVertexData->vertexDeclaration;
    const VertexElement* positionElem = decl->findElementBySemantic(VES_POSITION);
    const VertexElement* normalElem = 0, * tangentElem = 0, * binormalElem = 0;
    const VertexElement* tex0Elem = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);
    const VertexElement* tex1Elem = 0;

    if (mVertexOptions & VO_NORMALS)
      normalElem = decl->findElementBySemantic(VES_NORMAL);
    if (mVertexOptions & VO_TANGENTS)
      tangentElem = decl->findElementBySemantic(VES_TANGENT);
    if (mVertexOptions & VO_BINORMALS)
      binormalElem = decl->findElementBySemantic(VES_BINORMAL);
    if (mVertexOptions & VO_TEXCOORD1)
      tex1Elem = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);

    // lock buffer
    unsigned char* pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_NORMAL));

    // iterate over all vertices to be updated
    for (VertexList::const_iterator vert = dirtyVertices.begin(); vert != dirtyVertices.end(); ++vert)
    {
      unsigned int vertexIndex = *vert;
      unsigned char* pCur = pBase + mMainBuffer->getVertexSize() * vertexIndex;
      // update vertex position
      float* pPosition;
      positionElem->baseVertexPointerToElement(pCur, &pPosition);
      Vector3 pos = mDescription->getVertexPosition(vertexIndex);
      *pPosition++ = pos.x;
      *pPosition++ = pos.y;
      *pPosition   = pos.z;
      // update primary texcoord set
      float* pTex0;
      tex0Elem->baseVertexPointerToElement(pCur, &pTex0);
      Vector2 tex0 = mDescription->getVertexTexcoord0(vertexIndex);
      *pTex0++ = tex0.x;
      *pTex0   = tex0.y;

      if (mVertexOptions & VO_NORMALS)
      {
        // update vertex normal
        float* pNormal;
        normalElem->baseVertexPointerToElement(pCur, &pNormal);
        Vector3 normal = mDescription->getVertexNormal(vertexIndex);
        *pNormal++ = normal.x;
        *pNormal++ = normal.y;
        *pNormal   = normal.z;
      }
      if (mVertexOptions & VO_TANGENTS)
      {
        // update vertex tangent
        float* pTangent;
        tangentElem->baseVertexPointerToElement(pCur, &pTangent);
        Vector3 tangent = mDescription->getVertexTangent(vertexIndex);
        *pTangent++ = tangent.x;
        *pTangent++ = tangent.y;
        *pTangent   = tangent.z;
      }
      if (mVertexOptions & VO_BINORMALS)
      {
        // update vertex binormal
        float* pBinormal;
        binormalElem->baseVertexPointerToElement(pCur, &pBinormal);
        Vector3 binormal = mDescription->getVertexBinormal(vertexIndex);
        *pBinormal++ = binormal.x;
        *pBinormal++ = binormal.y;
        *pBinormal   = binormal.z;
      }
      if (mVertexOptions & VO_TEXCOORD1)
      {
        // update secondary texcoord set
        float* pTex1;
        tex1Elem->baseVertexPointerToElement(pCur, &pTex1);
        Vector2 tex1 = mDescription->getVertexTexcoord1(vertexIndex);
        *pTex1++ = tex1.x;
        *pTex1   = tex1.y;
      }
    }

    // unlock the buffer
    mMainBuffer->unlock();

    // update the bounding box
    updateBoundingBox();

    // update the LOD distances
    VertexList dirtyLODFrames;
    unsigned int lodStep = 1 << (mMaxLOD-1);
    unsigned int lodSizeX = mDescription->getNumVerticesX() / lodStep;
    unsigned int lodSizeZ = mDescription->getNumVerticesZ() / lodStep;
    for (VertexList::const_iterator it = dirtyVertices.begin(); it != dirtyVertices.end(); ++it)
    {
      unsigned int x, z;
      mDescription->getVertexCoord(*it, x, z);
      unsigned int lodX = x / lodStep, lodZ = z / lodStep;
      unsigned int frameIndex = lodZ * lodSizeX + lodX;
      if (lodX != lodSizeX && lodZ != lodSizeZ)
        dirtyLODFrames.insert(frameIndex);
      if (x % lodStep == 0 && x != 0 && lodZ != lodSizeZ)
        dirtyLODFrames.insert(frameIndex-1);
      if (z % lodStep == 0 && z != 0 && lodX != lodSizeX)
        dirtyLODFrames.insert(frameIndex-lodSizeX);
    }
    updateLODChangeDistances(dirtyLODFrames);
  }


  void Patch::updateBoundingBox()
  {
    // iterate over all vertices of the TerrainDescription and find the extents
    Vector3 minimum (1e30, 1e30, 1e30), maximum (-1e30, -1e30, -1e30);
    unsigned int numVertices = mDescription->getNumVertices();
    for (unsigned int i = 0; i < numVertices; ++i)
    {
      Vector3 pos = mDescription->getVertexPosition(i);
      minimum.makeFloor(pos);
      maximum.makeCeil(pos);
    }

    mBoundingBox.setExtents(minimum, maximum);
    mCenter = mBoundingBox.getCenter();
    mBoundingRadius = (mBoundingBox.getMaximum() - mCenter).length();
  }


  void Patch::updateLODChangeDistances(const VertexList& dirtyLODFrames)
  {
    // By switching to higher LOD levels, certain vertices are left out. We calculate
    // the errors in terrain height which are introduced by that. From these errors
    // we calculate the distance at which it's okay to switch to the higher LOD level.
    // We also store the height differences in the delta buffers for geomorphing.

    unsigned int patchSizeX = mDescription->getNumVerticesX();
    unsigned int patchSizeZ = mDescription->getNumVerticesZ();

    // lock delta buffers, if vertex morphing is enabled
    vector<float*> pDeltas (mMaxLOD-1);
    if (mVertexOptions & VO_LODMORPH)
    {
      for (unsigned int l = 0; l < mMaxLOD-1; ++l)
        pDeltas[l] = static_cast<float*>( mDeltaBuffers[l]->lock(HardwareBuffer::HBL_NORMAL) );
    }

    // we are caching values in chunks of the size of the max LOD step
    unsigned int cacheStep = 1 << (mMaxLOD-1);
    unsigned int cacheSizeX = patchSizeX / cacheStep;
    unsigned int cacheSizeZ = patchSizeZ / cacheStep;
    vector<RealArray2D> cache (mMaxLOD-1, RealArray2D(cacheStep+1, cacheStep+1, 0));
    // update all LOD frames which need updating, each frame is cacheStep x cacheStep
    for (VertexList::const_iterator it = dirtyLODFrames.begin(); it != dirtyLODFrames.end(); ++it)
    {
      unsigned int cacheX = (*it % cacheSizeX) * cacheStep;
      unsigned int cacheZ = (*it / cacheSizeX) * cacheStep;
      unsigned int cacheEndX = cacheX + cacheStep, cacheEndZ = cacheZ + cacheStep;

      for (unsigned int level = mMaxLOD-1; level >= 1; --level)
      {
        unsigned int levelStep = 1 << level;
        unsigned int levelHalfStep = levelStep >> 1;
        for (unsigned int levelX = cacheX; levelX < cacheEndX; levelX += levelStep)
        {
          for (unsigned int levelZ = cacheZ; levelZ < cacheEndZ; levelZ += levelStep)
          {
            unsigned int indexX = levelX - cacheX, indexZ = levelZ - cacheZ;
            Real ul = mDescription->getVertexPosition(levelX, levelZ).y;
            Real ur = mDescription->getVertexPosition(levelX+levelStep, levelZ).y;
            Real ll = mDescription->getVertexPosition(levelX, levelZ+levelStep).y;
            Real lr = mDescription->getVertexPosition(levelX+levelStep, levelZ+levelStep).y;
            cache[level-1].at(indexX, indexZ) = 0;
            cache[level-1].at(indexX+levelStep, indexZ) = 0;
            cache[level-1].at(indexX, indexZ+levelStep) = 0;
            cache[level-1].at(indexX+levelStep, indexZ+levelStep) = 0;
            // calculate the height error for the intermediant vertices introduced from this level on
            for (unsigned int l = level; l < mMaxLOD; ++l)
            {
              Real um = -mDescription->getVertexPosition(levelX+levelHalfStep, levelZ).y
                + (ul+ur)/2 - cache[l-1].at(indexX, indexZ) 
                - cache[l-1].at(indexX, indexZ+levelStep);
              Real lm = -mDescription->getVertexPosition(levelX+levelHalfStep, levelZ+levelStep).y 
                + (ll+lr)/2 - cache[l-1].at(indexX, indexZ+levelStep) 
                - cache[l-1].at(indexX+levelStep, indexZ+levelStep);
              Real ml = -mDescription->getVertexPosition(levelX, levelZ+levelHalfStep).y
                + (ul+ll)/2 - cache[l-1].at(indexX, indexZ)
                - cache[l-1].at(indexX+levelStep, indexZ);
              Real mr = -mDescription->getVertexPosition(levelX+levelStep, levelZ+levelHalfStep).y 
                + (ur+lr)/2 - cache[l-1].at(indexX+levelStep, indexZ)
                - cache[l-1].at(indexX+levelStep, indexZ+levelStep);
              Real mm = -mDescription->getVertexPosition(levelX+levelHalfStep, levelZ+levelHalfStep).y 
                + (ll+ur)/2 - cache[l-1].at(indexX, indexZ+levelStep)
                - cache[l-1].at(indexX+levelStep, indexZ);
              cache[l-1].at(indexX+levelHalfStep, indexZ) = um/2;
              cache[l-1].at(indexX+levelHalfStep, indexZ+levelStep) = lm/2;
              cache[l-1].at(indexX, indexZ+levelHalfStep) = ml/2;
              cache[l-1].at(indexX+levelStep, indexZ+levelHalfStep) = mr/2;
              cache[l-1].at(indexX+levelHalfStep, indexZ+levelHalfStep) = mm/2;

              // if vertex morphing is enabled, store values in delta buffers
              if (l == level && (mVertexOptions & VO_LODMORPH))
              {
                if (!isBorderVertex(levelX+levelHalfStep, levelZ))
                  pDeltas[l-1][levelX+levelHalfStep + (levelZ)*patchSizeX] = um;
                if (!isBorderVertex(levelX+levelHalfStep, levelZ+levelStep))
                  pDeltas[l-1][levelX+levelHalfStep + (levelZ+levelStep)*patchSizeX] = lm;
                if (!isBorderVertex(levelX, levelZ+levelHalfStep))
                  pDeltas[l-1][levelX + (levelZ+levelHalfStep)*patchSizeX] = ml;
                if (!isBorderVertex(levelX+levelStep, levelZ+levelHalfStep))
                  pDeltas[l-1][levelX+levelStep + (levelZ+levelHalfStep)*patchSizeX] = mr;
                pDeltas[l-1][levelX+levelHalfStep + (levelZ+levelHalfStep)*patchSizeX] = mm;
              }
            }
          }
        }

        // store into LOD cache
        unsigned int lodCacheX = cacheX / cacheStep, lodCacheZ = cacheZ / cacheStep;
        for (unsigned int l = 0; l < mMaxLOD-1; ++l)
        {
          Real maxAbs = 0;
          for (RealArray2D::iterator it = cache[l].begin(); it != cache[l].end(); ++it)
          {
            if (Math::Abs(*it) > maxAbs)
              maxAbs = Math::Abs(*it);
          }
          mLODCache[l].at(lodCacheX, lodCacheZ) = maxAbs * 2;
        }
      }
    }

    // unlock delta buffers, if vertex morphing is enabled
    if (mVertexOptions & VO_LODMORPH)
    {
      for (unsigned int l = 0; l < mMaxLOD-1; ++l)
        mDeltaBuffers[l]->unlock();
    }

    // C is a factor for the calculation of the min distance.
    Real C = 0.5 / mMaxError;
    // calculate new switch distances
    mLODChangeMinDistSqr[0] = 0;
    for (unsigned int l = 0; l < mMaxLOD-1; ++l)
    {
      Real maxError = *max_element(mLODCache[l].begin(), mLODCache[l].end());
      Real switchDist = maxError*maxError*C*C;
      mLODChangeMinDistSqr[l+1] = max(mLODChangeMinDistSqr[l], switchDist);
    }
  } 

  bool Patch::isBorderVertex(unsigned int x, unsigned int z)
  {
    return (x == 0 || z == 0 || 
        x == mDescription->getNumVerticesX()-1 ||
        z == mDescription->getNumVerticesZ()-1
    );
  }


  void Patch::emptyBuffer(HardwareVertexBufferSharedPtr buf)
  {
    // fills the buffer with 0
    void* pBuf = buf->lock(HardwareBuffer::HBL_DISCARD);
    memset(pBuf, 0, mDescription->getNumVertices() * buf->getVertexSize());
    buf->unlock();
  }


  void Patch::setNeighbour(int direction, Patch* neighbour)
  {
    int opposite = OPPOSITE_DIR[direction];

    if (mNeighbours[direction] != 0)
    {
      // we are no longer neighbour to the previous patch
      mNeighbours[direction]->mNeighbours[opposite] = 0;
    }

    if (neighbour)
    {
      // also adjust the neighbour settings of the neighbour
      if (neighbour->mNeighbours[opposite] != 0)
        neighbour->mNeighbours[opposite]->mNeighbours[direction] = 0;
      neighbour->mNeighbours[opposite] = this;
    }

    mNeighbours[direction] = neighbour;
  }


  void Patch::notifyUpdated(const VertexList& dirtyVertices)
  {
    // update the vertex buffer
    updateVertexBuffer(dirtyVertices);
  }

  void Patch::notifyDestroyed()
  {
    // set the TerrainDescription to 0 so that on destruction we don't try to unregister as a listener
    mDescription = 0;
    // should we automatically delete?
    if (mAutoDelete)
      delete this;
  }



  const String& Patch::getMovableType() const
  {
    static const String type = "EditableTerrainPatch";
    return type;
  }

  const MaterialPtr& Patch::getMaterial() const
  {
    return mMaterial;
  }

  void Patch::setMaterial(const MaterialPtr& material)
  {
    mMaterial = material;
  }

  const AxisAlignedBox& Patch::getBoundingBox() const
  {
    return mBoundingBox;
  }

  Real Patch::getBoundingRadius() const
  {
    return mBoundingRadius;
  }

  void Patch::getWorldTransforms(Matrix4* m) const
  {
    *m = mParentNode->_getFullTransform();
  }

  const Quaternion& Patch::getWorldOrientation() const
  {
    return mParentNode->_getDerivedOrientation();
  }

  const Vector3& Patch::getWorldPosition() const
  {
    return mParentNode->_getDerivedPosition();
  }

  Real Patch::getSquaredViewDepth(const Camera* cam) const
  {
    return (mCenter - cam->getDerivedPosition()).squaredLength();
  }

  const LightList& Patch::getLights() const
  {
    if (mLightListDirty)
    {
      // query the scene manager of the parent node for lights affecting this renderable
      SceneManager* sceneMgr = 0;
      if (mParentIsTagPoint)
      {
        // get the parent entity and obtain its scene manager
        sceneMgr = static_cast<const TagPoint*>(mParentNode)->getParentEntity()->_getManager();
      }
      else
      {
        // get the creator of the parent scene node
        sceneMgr = static_cast<const SceneNode*>(mParentNode)->getCreator();
      }
      sceneMgr->_populateLightList(mCenter, mBoundingRadius, mLightList);
      mLightListDirty = false;
    }

    return mLightList;
  }

  Ogre::uint32 Patch::getTypeFlags() const
  {
    return SceneManager::WORLD_GEOMETRY_TYPE_MASK;
  }


  void Patch::getRenderOperation(RenderOperation& op)
  {
    op.useIndexes = true;
    op.operationType = RenderOperation::OT_TRIANGLE_LIST;
    op.vertexData = mVertexData;

    // determine LOD state of the neighbours
    unsigned int northLOD = (mNeighbours[DIR_NORTH] ? mNeighbours[DIR_NORTH]->getCurrentLOD() : 0);
    unsigned int eastLOD = (mNeighbours[DIR_EAST] ? mNeighbours[DIR_EAST]->getCurrentLOD() : 0);
    unsigned int southLOD = (mNeighbours[DIR_SOUTH] ? mNeighbours[DIR_SOUTH]->getCurrentLOD() : 0);
    unsigned int westLOD = (mNeighbours[DIR_WEST] ? mNeighbours[DIR_WEST]->getCurrentLOD() : 0);
    op.indexData = mIndexHandler->getIndexData(mCurLOD, northLOD, eastLOD, southLOD, westLOD);
  }

  void Patch::_updateRenderQueue(RenderQueue* queue)
  {
    mLightListDirty = true;
    queue->addRenderable(this, mRenderQueueID);
  }


  void Patch::_updateCustomGpuParameter(const GpuProgramParameters::AutoConstantEntry& constEntry,
    GpuProgramParameters* params) const
  {
    if (constEntry.data == MORPH_CUSTOM_PARAM_ID)
      params->_writeRawConstant(constEntry.physicalIndex, mLODMorphFactor);
    else
      Renderable::_updateCustomGpuParameter(constEntry, params);
  }


  void Patch::_notifyCurrentCamera(Camera* cam)
  {
    MovableObject::_notifyCurrentCamera(cam);

    // we need to determine the level of detail to use for the tile and
    // set up the correct delta buffer for LOD morphing

    // get the distance from the camera to the patch
    Vector3 camPos = cam->getDerivedPosition();
    const AxisAlignedBox& aabb = getWorldBoundingBox(true);
    Vector3 diff (0, 0, 0);
    diff.makeFloor(camPos - aabb.getMinimum());
    diff.makeCeil(camPos - aabb.getMaximum());
    Real L = mCurCameraDistanceSqr = diff.squaredLength();

    // determine LOD to use for this patch
    mCurLOD = mMaxLOD - 1;
    for (unsigned int i = 1; i < mMaxLOD; ++i)
    {
      if (mLODChangeMinDistSqr[i] > L)
      {
        mCurLOD = i - 1;
        break;
      }
    }

    // set up LOD morphing, if enabled
    if (mVertexOptions & VO_LODMORPH)
    {
      // we need to find the next LOD after the current one, usually this is mCurLOD+1
      // however, theoretically some LOD might have identical distances, so check anyway to be sure
      unsigned int nextLevel = mCurLOD + 1;
      for (unsigned int i = nextLevel; i < mMaxLOD; ++i)
      {
        if (mLODChangeMinDistSqr[i] > mLODChangeMinDistSqr[mCurLOD])
        {
          nextLevel = i;
          break;
        }
      }

      // determine the LOD morph factor by which to multiply the blendweights
      if (nextLevel == mMaxLOD)
      {
        // there is no next level, so set the morph factor to 0
        mLODMorphFactor = 0;
      }
      else
      {
        // get the distance range between the current and the next level
        Real range = mLODChangeMinDistSqr[nextLevel] - mLODChangeMinDistSqr[mCurLOD];
        Real percent = (L - mLODChangeMinDistSqr[mCurLOD]) / range;
        // scale so that morphFactor == 0 for percent == mLODMorphDistStart and morphFactor == 1
        // for percent == 1, clamp to 0 below
        Real scale = 1.0 / (1.0 - mLODMorphDistStart);
        mLODMorphFactor = max((percent - mLODMorphDistStart) * scale, Real(0));
      }

      // bind the correct delta buffer if changed
      if (mLastNextLOD != nextLevel)
      {
        if (nextLevel != mMaxLOD)
        {
          mVertexData->vertexBufferBinding->setBinding(DELTA_BINDING, mDeltaBuffers[nextLevel-1]);
        }
        else
        {
          // bind a dummy, the morph factor is 0 anyway
          mVertexData->vertexBufferBinding->setBinding(DELTA_BINDING, mDeltaBuffers[0]);
        }
        mLastNextLOD = nextLevel;
      }
    }
  }


  Technique* Patch::getTechnique() const
  {
    // based on the current distance to the camera, select the appropriate LOD technique from the material
    unsigned short lodIndex = mMaterial->getLodIndexSquaredDepth(mCurCameraDistanceSqr);
    return mMaterial->getBestTechnique(lodIndex);
  }
}
