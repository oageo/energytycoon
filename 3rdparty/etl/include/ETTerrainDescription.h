#ifndef __ET_TERRAINDESCRIPTION_H__
#define __ET_TERRAINDESCRIPTION_H__
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
#include <OgreVector3.h>
#include <OgreVector2.h>

namespace Ogre
{
  class Vector3;
  class Vector2;
}

namespace ET
{
  class TerrainListener;

  /**
   * This is an abstract interface which represents a vertex description of a
   * heightmapped piece of terrain.
   */
  class TerrainDescription
  {
  public:
    virtual ~TerrainDescription() { }

    /** Get the number of vertices of this terrain */
    unsigned int getNumVertices() const { return mNumVertices; }
    /** Get the number of vertices in X direction */
    unsigned int getNumVerticesX() const { return mSizeX; }
    /** Get the number of vertices in Z direction */
    unsigned int getNumVerticesZ() const { return mSizeZ; }
    /** Get the vertex scale in Ogre units */
    virtual Ogre::Vector3 getVertexScale() const = 0;
    /** Get the vertex index from 2D coordinates */
    unsigned int getVertexIndex(unsigned int x, unsigned int z) const { return x+z*mSizeX; }
    /** Part a vertex index into its position in the array */
    void getVertexCoord(unsigned int i, unsigned int& x, unsigned int& z) const
    {
      x = i % mSizeX;
      z = i / mSizeX;
    }

    virtual Ogre::Vector3 getVertexPosition(unsigned int x, unsigned int z) const = 0;
    virtual Ogre::Vector3 getVertexNormal(unsigned int x, unsigned int z) const = 0;
    virtual Ogre::Vector3 getVertexTangent(unsigned int x, unsigned int z) const = 0;
    virtual Ogre::Vector3 getVertexBinormal(unsigned int x, unsigned int z) const = 0;
    virtual Ogre::Vector2 getVertexTexcoord0(unsigned int x, unsigned int z) const = 0;
    virtual Ogre::Vector2 getVertexTexcoord1(unsigned int x, unsigned int z) const = 0;

    /** Get the position of the i'th vertex */
    Ogre::Vector3 getVertexPosition(unsigned int i) const
    {
      return getVertexPosition(i % mSizeX, i / mSizeX);
    }
    /** Get the normal of the i'th vertex */
    Ogre::Vector3 getVertexNormal(unsigned int i) const
    {
      return getVertexNormal(i % mSizeX, i / mSizeX);
    }
    /** Get the tangent of the i'th vertex */
    Ogre::Vector3 getVertexTangent(unsigned int i) const
    {
      return getVertexTangent(i % mSizeX, i / mSizeX);
    }
    /** Get the binormal of the i'th vertex */
    Ogre::Vector3 getVertexBinormal(unsigned int i) const
    {
      return getVertexBinormal(i % mSizeX, i / mSizeX);
    }
    /** Get the first texture coordinate of the i'th vertex */
    Ogre::Vector2 getVertexTexcoord0(unsigned int i) const
    {
      return getVertexTexcoord0(i % mSizeX, i / mSizeX);
    }
    /** Get the second texture coordinate of the i'th vertex */
    Ogre::Vector2 getVertexTexcoord1(unsigned int i) const
    {
      return getVertexTexcoord1(i % mSizeX, i / mSizeX);
    }

    /** Get the size of the triangle list of the index buffer (at full detail, no stitching) */
    virtual unsigned int getTriListSize() const = 0;
    /** Get the vertex index for the i'th position in the trilist */
    virtual unsigned int getTriListIndex(unsigned int i) const = 0;

    /** Register a TerrainListener with this description */
    void addListener(TerrainListener* listener) { mListeners.push_back(listener); onNewListener(listener); }
    /** Remove a TerrainListener from this description */
    void removeListener(TerrainListener* listener)
    {
      mListeners.erase(std::find(mListeners.begin(), mListeners.end(), listener));
    }

  protected:
    virtual void onNewListener(TerrainListener* listener) { }

    unsigned int mNumVertices, mSizeX, mSizeZ;
    std::vector<TerrainListener*> mListeners;
  };


  /**
   * A TerrainListener can be registered with a TerrainDescription to receive events
   * when the TerrainDescription is updated or destroyed.
   */
  class TerrainListener
  {
  public:
    virtual ~TerrainListener() { }

    /**
     * Called when the terrain description changes.
     * @param dirtyVertices A list of vertices which have changed.
     */
    virtual void notifyUpdated(const VertexList& dirtyVertices) = 0;

    /** Called when the terrain description is being destroyed */
    virtual void notifyDestroyed() = 0;
  };
}

#endif
