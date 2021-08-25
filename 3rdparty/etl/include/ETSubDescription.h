#ifndef __ET_SUBDESCRIPTION_H__
#define __ET_SUBDESCRIPTION_H__
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

namespace ET
{
  /**
   * SubDescription is a wrapper for a TerrainDescription which exposes a subarea
   * of the full TerrainDescription.
   */
  class SubDescription : public TerrainDescription, public TerrainListener
  {
  public:
    /**
     * Create a new sub description.
     * @param description The full TerrainDescription to wrap
     * @param startX X position from which to start the subarea
     * @param startZ Z position from which to start the subarea
     * @param sizeX Num vertices in X direction to include in the subarea
     * @param sizeZ Num vertices in Z direction to include in the subarea
     * @param autoDelete Automatically delete the subdescription with the full TerrainDescription?
     */
    SubDescription(TerrainDescription* description, unsigned int startX, unsigned int startZ,
      unsigned int sizeX, unsigned int sizeZ, bool autoDelete);
    ~SubDescription();


    Ogre::Vector3 getVertexScale() const;
    Ogre::Vector3 getVertexPosition(unsigned int x, unsigned int z) const;
    Ogre::Vector3 getVertexNormal(unsigned int x, unsigned int z) const;
    Ogre::Vector3 getVertexTangent(unsigned int x, unsigned int z) const;
    Ogre::Vector3 getVertexBinormal(unsigned int x, unsigned int z) const;
    Ogre::Vector2 getVertexTexcoord0(unsigned int x, unsigned int z) const;
    Ogre::Vector2 getVertexTexcoord1(unsigned int x, unsigned int z) const;
    unsigned int getTriListSize() const;
    unsigned int getTriListIndex(unsigned int i) const;

    void notifyUpdated(const VertexList& dirtyVertices);
    void notifyDestroyed();


  private:
    TerrainDescription* mDescription;
    unsigned int mStartX, mStartZ;
    bool mAutoDelete;

    Ogre::Node* mParentNode;
    bool mIsTagPoint;
    bool mIsVisible;
  };
}

#endif
