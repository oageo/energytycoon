#ifndef __ET_PATCH_H__
#define __ET_PATCH_H__
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

#include "ETTerrainDescription.h"
#include "ETCompatibility.h"

#include <OgreMovableObject.h>
#include <OgreRenderable.h>

namespace ET
{
  class IndexHandler;

  /**
   * A Patch is the rendering primitive for the terrain. It represents a square piece of
   * terrain with 2^n + 1 vertices in x and z direction. Level of detail is determined
   * based on the camera distance, and Patches can be connected with each other so that
   * LOD is stitched correctly at the borders.
   *
   * @remarks You'll usually not use this class directly; see Page instead.
   */
  class Patch : public Ogre::MovableObject, public Ogre::Renderable, public TerrainListener
  {
  public:
    /**
     * Create a new Patch from a given terrain description.
     * @param name MovableObject name
     * @param description TerrainDescription to create the Patch from
     * @param autoDelete Delete automatically when the description is destroyed?
     * @param indexHandler The index handler from which to get the required index buffers
     * @param maxLOD How many levels of detail to use
     * @param maxError The maximal error margin for determining the level of detail
     * @param vertexOptions Bitmask specifying additional info to include in the vertex buffers
     * @param lodMorphDistStart Percentage of distance at which to start LOD morphing
     */
    Patch(const Ogre::String& name, TerrainDescription* description, bool autoDelete,
      IndexHandler* indexHandler, unsigned int maxLOD, Ogre::Real maxError,
      unsigned int vertexOptions = VO_DEFAULT, Ogre::Real lodMorphDistStart = 0.5f);

    ~Patch();

    /** Set material to use for this patch */
    void setMaterial(const Ogre::MaterialPtr& material);

    /** Set a neighbouring patch in the given direction */
    void setNeighbour(int direction, Patch* neighbour);

    /** Return current level of detail */
    int getCurrentLOD() const { return mCurLOD; }

    /** Get the TerrainDescription from which this Patch is created */
    TerrainDescription* getTerrainDescription() const { return mDescription; }


    //**********************
    // RENDERABLE INTERFACE
    //**********************
    const Ogre::MaterialPtr& getMaterial() const;
    Ogre::Technique* getTechnique() const;
    void getRenderOperation(Ogre::RenderOperation& op);
    void getWorldTransforms(Ogre::Matrix4* xform) const;
    const Ogre::Quaternion& getWorldOrientation() const;
    const Ogre::Vector3& getWorldPosition() const;
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
    const Ogre::LightList& getLights() const;
    void _updateCustomGpuParameter(const Ogre::GpuProgramParameters::AutoConstantEntry& constantEntry,
      Ogre::GpuProgramParameters* params) const;

    //*********************
    // MOVABLE INTERFACE
    //*********************
    const Ogre::String& getMovableType() const;
    void _notifyCurrentCamera(Ogre::Camera* cam);
    const Ogre::AxisAlignedBox& getBoundingBox() const;
    Ogre::Real getBoundingRadius() const;
    void _updateRenderQueue(Ogre::RenderQueue* queue);
    Ogre::uint32 getTypeFlags() const;

    //*********************
    // LISTENER INTERFACE
    //*********************
    void notifyUpdated(const VertexList& dirtyVertices);
    void notifyDestroyed();


    void visitRenderables(OgreRenderableVisitor* visitor, bool) { visitor->visit(this, 0, false); }


  private:
    /** Initialise the vertex description */
    void createVertexData();
    /** Update the data of the given vertices in the vertex buffer */
    void updateVertexBuffer(const VertexList& dirtyVertices);
    /** Update the extents of the Patch */
    void updateBoundingBox();
    /** Update the LOD switching distances and delta buffers */
    void updateLODChangeDistances(const VertexList& dirtyLODFrames);
    /** Check if a vertex lies at the border */
    bool isBorderVertex(unsigned int x, unsigned int z);

    /** Helper function to clear the delta buffers */
    void emptyBuffer(Ogre::HardwareVertexBufferSharedPtr buf);

    TerrainDescription* mDescription;
    bool mAutoDelete;
    IndexHandler* mIndexHandler;
    unsigned int mVertexOptions;

    /** Max level of detail */
    unsigned int mMaxLOD;
    /** Max LOD error margin */
    Ogre::Real mMaxError;
    /** Current level of detail */
    unsigned int mCurLOD;
    /** LOD change distances */
    std::vector<Ogre::Real> mLODChangeMinDistSqr;
    /** LOD after the current one at the last render operation. Stored to keep track
     *  if we need to change the delta buffers */
    unsigned int mLastNextLOD;
    /** Cache for LOD distance calculations */
    std::vector<RealArray2D> mLODCache;

    Ogre::AxisAlignedBox mBoundingBox;
    Ogre::Real mBoundingRadius;
    Ogre::Vector3 mCenter;

    /** Vertex data description */
    Ogre::VertexData* mVertexData;
    /** Vertex buffer */
    Ogre::HardwareVertexBufferSharedPtr mMainBuffer;
    /** LOD morphing delta buffers */
    std::vector<Ogre::HardwareVertexBufferSharedPtr> mDeltaBuffers;
    /** Percentage of distance at which to start LOD morphing */
    Ogre::Real mLODMorphDistStart;
    /** Factor with which to multiply the blendweights for LOD morphing */
    Ogre::Real mLODMorphFactor;

    /** Material to use for the patch */
    Ogre::MaterialPtr mMaterial;

    /** Array of neighbours */
    Patch* mNeighbours[4];

    /** Distance to the current camera */
    Ogre::Real mCurCameraDistanceSqr;

    /** Do we need to update light list? */
    mutable bool mLightListDirty;
  };
}
#endif
