#ifndef __ET_MOVABLECONTAINER_H__
#define __ET_MOVABLECONTAINER_H__
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

#include <OgreMovableObject.h>
#include <vector>

namespace ET
{
  class Movable;

  /**
   * MovableContainer is a convenience class to synchronously move several
   * editable objects. Attach the MovableContainer to a scene node, then
   * register your editable objects with the container, and they will
   * automatically be updated to the scene node's position.
   * A Movable can only be registered to one container at a time.
   * Adding it to a new container will remove it from the old.
   */
  class MovableContainer : public Ogre::MovableObject
  {
  public:
    ~MovableContainer();

    void addMovable(Movable* movable);
    void removeMovable(Movable* movable);

    // MovableObject interface
    virtual const Ogre::String& getMovableType() const;
    virtual void _notifyAttached(Ogre::Node* parent, bool isTagPoint=false);
    virtual void _notifyMoved();
    virtual const Ogre::AxisAlignedBox& getBoundingBox() const;
    virtual Ogre::Real getBoundingRadius() const { return 0; }
    virtual void _updateRenderQueue(Ogre::RenderQueue* queue) {}

  private:
    void updateAll();
    void update(Movable* movable);

    typedef std::vector<Movable*> MovableList;
    MovableList mMovables;
  };
}

#endif

