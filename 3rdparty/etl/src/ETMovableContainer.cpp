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
#include <OgreNode.h>

using namespace Ogre;

namespace ET
{
  MovableContainer::~MovableContainer()
  {
    for (MovableList::iterator it = mMovables.begin(); it != mMovables.end(); ++it)
      (*it)->_notifyAddedToContainer(0);
  }

  void MovableContainer::addMovable(Movable* movable)
  {
    movable->_notifyAddedToContainer(this);
    mMovables.push_back(movable);
    // set current position
    update(movable);
  }

  void MovableContainer::removeMovable(Movable* movable)
  {
    MovableList::iterator i = find(mMovables.begin(), mMovables.end(), movable);
    if (i != mMovables.end())
    {
      mMovables.erase(i);
      movable->_notifyAddedToContainer(0);
    }
  }

  void MovableContainer::updateAll()
  {
    for (MovableList::iterator it = mMovables.begin(); it != mMovables.end(); ++it)
      update(*it);
  }

  void MovableContainer::update(Movable* movable)
  {
    // copy the attached node's position and orientation to the movable
    if (isAttached())
    {
      Node* node = getParentNode();
      movable->setPosition(node->_getDerivedPosition());
      movable->setOrientation(node->_getDerivedOrientation());
      movable->setScale(node->_getDerivedScale());
    }
    else
    {
      // assume origin
      movable->setPosition(Vector3(0,0,0));
      movable->setOrientation(Quaternion::IDENTITY);
      movable->setScale(Vector3(1,1,1));
    }
  }


  const String& MovableContainer::getMovableType() const
  {
    static String type = "ET::MovableContainer";
    return type;
  }

  void MovableContainer::_notifyAttached(Node* parent, bool isTagPoint)
  {
    updateAll();
  }

  void MovableContainer::_notifyMoved()
  {
    updateAll();
  }

  const AxisAlignedBox& MovableContainer::getBoundingBox() const
  {
    static AxisAlignedBox box (0, 0, 0, 0, 0, 0);
    return box;
  }

}

