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

#include "ETMovable.h"
#include "ETMovableContainer.h"
#include <cassert>

using namespace Ogre;


namespace ET
{
  Movable::Movable() : mPosition(0,0,0), mScale(1,1,1), mOrientation(Quaternion::IDENTITY),
    mContainer(0)
  {
  }

  Movable::~Movable()
  {
    if (mContainer)
      mContainer->removeMovable(this);
  }

  Vector3 Movable::transformPositionW2L(const Vector3& worldPos) const
  {
    Quaternion invOrient = mOrientation.UnitInverse();
    return (invOrient * (worldPos - mPosition)) / mScale;
  }


  Vector3 Movable::transformPositionL2W(const Vector3& localPos) const
  {
    return (mOrientation*localPos + mPosition) * mScale;
  }


  Vector3 Movable::transformVectorW2L(const Vector3& worldVec) const
  {
    return (mOrientation.UnitInverse() * worldVec);
  }


  Vector3 Movable::transformVectorL2W(const Vector3& localVec) const
  {
    return (mOrientation * localVec);
  }

  void Movable::_notifyAddedToContainer(MovableContainer* container)
  {
    // unregister from previous
    if (mContainer != 0 && container != 0)
      mContainer->removeMovable(this);
    mContainer = container;
  }
}
