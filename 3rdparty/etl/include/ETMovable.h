#ifndef __ET_MOVABLE_H__
#define __ET_MOVABLE_H__
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

#include <OgreVector3.h>
#include <OgreQuaternion.h>

namespace ET
{
  class MovableContainer;

  /**
   * Movable is a base class for all ET classes which have a position and
   * orientation in 3D space.
   * It provides the shared functionality to transform positions and vectors
   * from world to local space and vice versa.
   */
  class Movable
  {
  public:
    virtual ~Movable();

    void setPosition(const Ogre::Vector3& pos) { mPosition = pos; }
    const Ogre::Vector3& getPosition() const { return mPosition; }

    void setOrientation(const Ogre::Quaternion& orient) { mOrientation = orient; }
    const Ogre::Quaternion& getOrientation() const { return mOrientation; }

    void setScale(const Ogre::Vector3& scale) { mScale = scale; }
    const Ogre::Vector3& getScale() const { return mScale; }

    /* Called by MovableContainer when added to a container. Internal */
    void _notifyAddedToContainer(MovableContainer* container);

  protected:
    Movable();

    Ogre::Vector3 mPosition;
    Ogre::Vector3 mScale;
    Ogre::Quaternion mOrientation;

    /** Transforms a world position into local space */
    Ogre::Vector3 transformPositionW2L(const Ogre::Vector3& worldPos) const;
    /** Transforms a local position into world space */
    Ogre::Vector3 transformPositionL2W(const Ogre::Vector3& localPos) const;
    /** Transforms a world vector into local space (vector being something like a normal, tangent etc.) */
    Ogre::Vector3 transformVectorW2L(const Ogre::Vector3& worldVec) const;
    /** Transforms a local vector into world space (vector being something like a normal, tangent etc.) */
    Ogre::Vector3 transformVectorL2W(const Ogre::Vector3& localVec) const;

  private:
    MovableContainer* mContainer;
  };
}

#endif
