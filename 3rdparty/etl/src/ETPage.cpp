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

#include "ETPage.h"
#include "ETIndexHandler.h"
#include "ETSubDescription.h"
#include "ETPatch.h"
#include <OgreException.h>
#include <OgreSceneNode.h>
#include <OgreStringConverter.h>
#include <OgreTagPoint.h>
#include <OgreEntity.h>

using namespace Ogre;
using namespace std;


namespace ET
{
  Page::Page(const String& name, TerrainDescription* description, unsigned int patchSizeX,
    unsigned int patchSizeZ, bool autoDelete, unsigned int maxLOD, Real maxError,
    unsigned int vertexOptions, Real lodMorphDistStart, IndexHandler* indexHandler)
  : MovableObject(name), mDescription(description), mPatchSizeX(patchSizeX), mPatchSizeZ(patchSizeZ),
    mAutoDelete(autoDelete), mIndexHandler(indexHandler), mDeleteIndexHandler(false)
  {
    // sanity check
    if ((description->getNumVerticesX()-1) % (patchSizeX-1) != 0 ||
      (description->getNumVerticesZ()-1) % (patchSizeZ-1) != 0)
    {
      // tile size doesn't fit to dimensions
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Given tile size does not fit the terrain dimensions.",
        "ET::Page::Page");
    }

    // do we need to create our own index handler?
    if (!mIndexHandler)
    {
      mDeleteIndexHandler = true;
      mIndexHandler = new IndexHandler(mPatchSizeX, mPatchSizeZ);
    }
    else
    {
      // check dimensions of the provided index handler
      if (mIndexHandler->getSizeX() != mPatchSizeX || mIndexHandler->getSizeZ() != mPatchSizeZ)
      {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Given index handler does not match the tile size.",
          "ET::Page::Page");
      }
    }

    createPatches(name, maxLOD, maxError, vertexOptions, lodMorphDistStart);

    mDescription->addListener(this);
  }

  Page::~Page()
  {
    if (mDescription)
      mDescription->removeListener(this);


    if (!mAutoDelete)
    {
      for (Array2D<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++it)
        delete *it;
      for (Array2D<SubDescription*>::iterator it = mAreas.begin(); it != mAreas.end(); ++it)
        delete *it;
    }

    if (mDeleteIndexHandler)
      delete mIndexHandler;
  }


  void Page::createPatches(const String& name, unsigned int maxLOD, Real maxError,
    unsigned int vertexOptions, Real lodMorphDistStart)
  {
    // determine how many patches we need
    unsigned int patchesX = (mDescription->getNumVerticesX()-1) / (mPatchSizeX-1);
    unsigned int patchesZ = (mDescription->getNumVerticesZ()-1) / (mPatchSizeZ-1);
    mAreas = Array2D<SubDescription*>(patchesX, patchesZ);
    mPatches = Array2D<Patch*>(patchesX, patchesZ);

    // create subareas and patches
    for (unsigned int x = 0; x < patchesX; ++x)
    {
      for (unsigned int z = 0; z < patchesZ; ++z)
      {
        unsigned int startX = x*(mPatchSizeX-1), startZ = z*(mPatchSizeZ-1);
        mAreas.at(x, z) = new SubDescription(mDescription, startX, startZ, mPatchSizeX, mPatchSizeZ, mAutoDelete);

        String patchName = name + ":" + StringConverter::toString(x) + "_" + StringConverter::toString(z);
        mPatches.at(x, z) = new Patch(patchName, mAreas.at(x, z), mAutoDelete, mIndexHandler,
          maxLOD, maxError, vertexOptions, lodMorphDistStart);
        // connect patches
        if (x > 0)
          mPatches.at(x,z)->setNeighbour(DIR_WEST, mPatches.at(x-1,z));
        if (z > 0)
          mPatches.at(x,z)->setNeighbour(DIR_NORTH, mPatches.at(x,z-1));
      }
    }
  }

  void Page::setNeighbour(int direction, Page* neighbour)
  {
    assert(mPatches.sizeX() == neighbour->mPatches.sizeX() && mPatches.sizeY() == neighbour->mPatches.sizeY()
      && "Neighbouring pages must have same patch layout");

    if (direction == DIR_NORTH)
    {
      for (unsigned int i = 0; i < mPatches.sizeX(); ++i)
        mPatches.at(i, 0)->setNeighbour(DIR_NORTH, neighbour->mPatches.at(i, mPatches.sizeY()-1));
    }
    else if (direction == DIR_SOUTH)
    {
      for (unsigned int i = 0; i < mPatches.sizeX(); ++i)
        mPatches.at(i, mPatches.sizeY()-1)->setNeighbour(DIR_SOUTH, neighbour->mPatches.at(i, 0));
    }
    else if (direction == DIR_EAST)
    {
      for (unsigned int i = 0; i < mPatches.sizeY(); ++i)
        mPatches.at(mPatches.sizeX()-1, i)->setNeighbour(DIR_EAST, neighbour->mPatches.at(0, i));
    }
    else
    {
      for (unsigned int i = 0; i < mPatches.sizeY(); ++i)
        mPatches.at(0, i)->setNeighbour(DIR_WEST, neighbour->mPatches.at(mPatches.sizeX()-1, i));
    }
  }


  void Page::notifyDestroyed()
  {
    mDescription = 0;
    if (mAutoDelete)
      delete this;
  }


  void Page::setMaterial(const MaterialPtr& material)
  {
    // set material on all patches
    for (Array2D<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++it)
      (*it)->setMaterial(material);
  }


  unsigned int Page::getNumPatchesX() const
  {
    return mPatches.sizeX();
  }

  unsigned int Page::getNumPatchesZ() const
  {
    return mPatches.sizeY();
  }

  Patch* Page::getPatch(unsigned int x, unsigned int z) const
  {
    return mPatches.at(x, z);
  }


  const String& Page::getMovableType() const
  {
    static const String TYPE = "ETPage";
    return TYPE;
  }

  const AxisAlignedBox& Page::getBoundingBox() const
  {
    static const AxisAlignedBox BOX (0,0,0,0,0,0);
    return BOX;
  }


  void Page::_notifyAttached(Node* parent, bool isTagPoint)
  {
    MovableObject::_notifyAttached(parent, isTagPoint);
    
    if (parent == 0)
    {
      // detach patches from their current node
      for (Array2D<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++it)
      {
        Patch* patch = *it;
        if (patch->isAttached())
        {
          SceneNode* sceneNode = patch->getParentSceneNode();
          if (sceneNode)
            sceneNode->detachObject(patch);
          else
          {
            TagPoint* tagPoint = static_cast<TagPoint*>(patch->getParentNode());
            tagPoint->getParentEntity()->detachObjectFromBone(patch);
          }
        }
      }
    }
    else
    {
      // attach Patches to the Node
      if (isTagPoint)
      {
        TagPoint* tagPoint = static_cast<TagPoint*>(parent);
        for (Array2D<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++it)
          tagPoint->getParentEntity()->attachObjectToBone(tagPoint->getName(), *it);
      }
      else
      {
        SceneNode* sceneNode = static_cast<SceneNode*>(parent);
        for (Array2D<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++it)
          sceneNode->attachObject(*it);
      }
    }
  }
}
