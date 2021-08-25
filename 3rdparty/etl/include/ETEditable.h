#ifndef __ET_EDITABLE_H__
#define __ET_EDITABLE_H__
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

#include <OgreVector2.h>
#include <vector>
#include "ETCommon.h"

namespace ET
{
  class Brush;
  class Editable;

  /**
   * The EditableListener provides an interface for derived classes to
   * get informed about events on an Editable. These include updates of
   * the object as well as its destruction.
   */
  class EditableListener
  {
  public:
    virtual ~EditableListener() = 0;

    /** Get notified when an area of the editable object is changed */
    virtual void notifyChanged(unsigned int startX, unsigned int startZ,
      unsigned int endX, unsigned int endZ) {}

    /** Get notified on the destruction of the Editable */
    virtual void notifyDestroyed(Editable* object) {}

  };

  /**
   * Editable is the base class for each ET class that represents an
   * editable component. It stores basic information about the editing grid of
   * the object and has common functionality to map a brush onto this grid. 
   */
  class Editable
  {
  public:
    virtual ~Editable();

    const Ogre::Vector2 getGridScale() const { return mGridScale; }
    unsigned int getGridWidth() const { return mGridWidth; }
    unsigned int getGridHeight() const { return mGridHeight; }

    /** Register an editable object listener. */
    void addEditableListener(EditableListener* listener);

    /** Remove an editable object listener. */
    void removeEditableListener(EditableListener* listener);

    /** Get the last edited area rect. */
    Rect getLastEditedRect() const { return mLastEditedRect; }

  protected:
    /**
     * Initialises an Editable.
     * @param gridScale The x and z distances of two points on the grid.
     * @param gridWidth The number of points on the grid in x direction.
     * @param gridHeight The number of points on the grid in z direction.
     */
    Editable(const Ogre::Vector2& gridScale, unsigned int gridWidth,
      unsigned int gridHeight);

    /** Determine the area of grid points which may be affected by a given brush. */
    Rect determineEditRect(Ogre::Real x, Ogre::Real z, const Brush* brush);

    Ogre::Vector2 mGridScale;
    unsigned int mGridWidth, mGridHeight;
    Rect mLastEditedRect;

    typedef std::vector<EditableListener*> EditListenerList;
    /** Registered listeners */
    EditListenerList mEditListeners;

  };
}

#endif

