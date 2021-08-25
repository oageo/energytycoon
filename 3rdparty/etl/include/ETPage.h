#ifndef __ET_PAGE_H__
#define __ET_PAGE_H__
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
#include "ETArray2D.h"
#include "ETCompatibility.h"
#include <OgreMovableObject.h>

namespace ET
{
  class IndexHandler;
  class Patch;
  class SubDescription;

  /**
   * A Page is a collection of Patches of a given tile size. You'll normally use this
   * class to visually represent your terrain.
   */
  class Page : public TerrainListener, public Ogre::MovableObject
  {
  public:
    /**
     * Create a Page from a TerrainDescription.
     * @param name Base name for the Patches (MovableObject names)
     * @param description Description of the terrain to build the Page from
     * @param patchSizeX Number of vertices of the Patches in X direction
     * @param patchSizeZ Number of vertices of the Patches in X direction
     * @param autoDelete Delete Page automatically when the description is deleted?
     * @param maxLOD Maximal number of different levels of details
     * @param maxError Error margin for choice of level of detail
     * @param vertexOptions Specify additional information to include in the vertices (normals etc.)
     * @param lodMorphDistStart Relative distance between two LODs at which to start morphing
     * @param indexHandler Supply a custom index handler, if 0, the Page will create its own
     */
    Page(const Ogre::String& name, TerrainDescription* description, unsigned int patchSizeX,
      unsigned int patchSizeZ, bool autoDelete, unsigned int maxLOD = 255, Ogre::Real maxError = 0.005,
      unsigned int vertexOptions = VO_DEFAULT, Ogre::Real lodMorphDistStart = 0.5,
      IndexHandler* indexHandler = 0);

    ~Page();

    /** Set neighbouring Pages */
    void setNeighbour(int direction, Page* neighbour);

    /** Set material for all Patches */
    void setMaterial(const Ogre::MaterialPtr& material);

    /** Get number of patches in x direction */
    unsigned int getNumPatchesX() const;
    /** Get number of patches in z direction */
    unsigned int getNumPatchesZ() const;
    /** Get access to the underlying patches */
    Patch* getPatch(unsigned int x, unsigned int z) const;

    /** Get the TerrainDescription from which this page is created */
    TerrainDescription* getTerrainDescription() const { return mDescription; }


    void notifyUpdated(const VertexList& dirtyVertices) { }
    void notifyDestroyed();


    //**************************
    // MovableObject interface
    //**************************

    const Ogre::String& getMovableType() const;
    void _updateRenderQueue(Ogre::RenderQueue* queue) { }
    const Ogre::AxisAlignedBox& getBoundingBox() const;
    Ogre::Real getBoundingRadius() const { return 0; }
    void _notifyAttached(Ogre::Node* parent, bool isTagPoint=false);


    void visitRenderables(OgreRenderableVisitor*, bool) { }


  private:
    void createPatches(const Ogre::String& name, unsigned int maxLOD, Ogre::Real maxError,
      unsigned int vertexOptions, Ogre::Real lodMorphDistStart);

    TerrainDescription* mDescription;
    unsigned int mPatchSizeX, mPatchSizeZ;
    bool mAutoDelete;
    IndexHandler* mIndexHandler;
    bool mDeleteIndexHandler;

    Array2D<Patch*> mPatches;
    Array2D<SubDescription*> mAreas;
  };
}

#endif
