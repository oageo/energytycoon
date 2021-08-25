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

#include "ETSubDescription.h"
#include <OgreException.h>
#include <OgreVector3.h>
#include <OgreVector2.h>

using namespace Ogre;
using namespace std;


namespace ET
{
  SubDescription::SubDescription(TerrainDescription* description, unsigned int startX,
    unsigned int startZ, unsigned int sizeX, unsigned int sizeZ, bool autoDelete)
  : mDescription(description), mStartX(startX), mStartZ(startZ), mAutoDelete(autoDelete), mParentNode(0),
    mIsTagPoint(false), mIsVisible(true)
  {
    mSizeX = sizeX; mSizeZ = sizeZ;
    mNumVertices = sizeX*sizeZ;

    // sanity check
    if (startX+sizeX > description->getNumVerticesX() || startZ+sizeZ > description->getNumVerticesZ())
    {
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Subarea exceeds parent.", "Et::SubDescription::SubDescription");
    }

    mDescription->addListener(this);
  }

  SubDescription::~SubDescription()
  {
    if (mDescription)
    {
      // the parent is still active, so unregister as listener and inform own listeners of destruction
      mDescription->removeListener(this);
      for (vector<TerrainListener*>::iterator it = mListeners.begin(); it != mListeners.end(); ++it)
        (*it)->notifyDestroyed();
    }
  }


  Vector3 SubDescription::getVertexScale() const
  {
    return mDescription->getVertexScale();
  }


  Vector3 SubDescription::getVertexPosition(unsigned int x, unsigned int z) const
  {
    x += mStartX;
    z += mStartZ;
    return mDescription->getVertexPosition(x, z);
  }

  Vector3 SubDescription::getVertexNormal(unsigned int x, unsigned int z) const
  {
    x += mStartX;
    z += mStartZ;
    return mDescription->getVertexNormal(x, z);
  }

  Vector3 SubDescription::getVertexTangent(unsigned int x, unsigned int z) const
  {
    x += mStartX;
    z += mStartZ;
    return mDescription->getVertexTangent(x, z);
  }

  Vector3 SubDescription::getVertexBinormal(unsigned int x, unsigned int z) const
  {
    x += mStartX;
    z += mStartZ;
    return mDescription->getVertexBinormal(x, z);
  }

  Vector2 SubDescription::getVertexTexcoord0(unsigned int x, unsigned int z) const
  {
    x += mStartX;
    z += mStartZ;
    return mDescription->getVertexTexcoord0(x, z);
  }

  Vector2 SubDescription::getVertexTexcoord1(unsigned int x, unsigned int z) const
  {
    // texcoord1 is relative to our own dimensions
    return Vector2(Real(x)/(mSizeX-1), Real(z)/(mSizeZ-1));
  }

  unsigned int SubDescription::getTriListSize() const
  {
    // there are 2 triangles per quad, each triangle requires 3 vertices.
    return (mSizeX-1)*(mSizeZ-1)*6;
  }

  unsigned int SubDescription::getTriListIndex(unsigned int i) const
  {
    unsigned int quad = i / 6;
    unsigned int inQuad = i % 6;
    unsigned int quadX = quad % (mSizeX-1);
    unsigned int quadZ = quad / (mSizeX-1);

    // decide which of the 4 vertices we need to return
    switch (inQuad)
    {
    case 0:
      return getVertexIndex(quadX, quadZ);
    case 1: case 3:
      return getVertexIndex(quadX, quadZ+1);
    case 2: case 5:
      return getVertexIndex(quadX+1, quadZ);
    case 4:
      return getVertexIndex(quadX+1, quadZ+1);
    default:
      return 0;
    }
  }


  void SubDescription::notifyUpdated(const VertexList& dirtyVertices)
  {
    // scan the given list for all vertices concerning this subarea
    VertexList subList;

    for (VertexList::const_iterator it = dirtyVertices.begin(); it != dirtyVertices.end(); ++it)
    {
      unsigned int x, z;
      mDescription->getVertexCoord(*it, x, z);
      if (x >= mStartX && x < mStartX+mSizeX && z >= mStartZ && z < mStartZ+mSizeZ)
        subList.insert(getVertexIndex(x-mStartX, z-mStartZ));
    }

    if (subList.empty())
      return;
    // notify listeners
    for (vector<TerrainListener*>::iterator it = mListeners.begin(); it != mListeners.end(); ++it)
      (*it)->notifyUpdated(subList);
  }


  void SubDescription::notifyDestroyed()
  {
    // notify listeners
    for (vector<TerrainListener*>::iterator it = mListeners.begin(); it != mListeners.end(); ++it)
      (*it)->notifyDestroyed();

    // reset the parent description
    mDescription = 0;
    if (mAutoDelete)
      delete this;
  }

}
