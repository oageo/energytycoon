#ifndef __ET_AUTOUPDATER_H__
#define __ET_AUTOUPDATER_H__
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

#include <OgreFrameListener.h>
#include <vector>

namespace ET
{
  class Updatable;

  /**
   * AutoUpdater is a convenience class which automatically updates registered
   * objects with a limited update rate. For example, you can have your terrain
   * updated twenty times per second. This is less than what you would get if
   * you would update the terrain after each call to deform and will therefore
   * increase your framerate considerably during editing.
   */
  class AutoUpdater : public Ogre::FrameListener
  {
  public:
    /** 
     * Upon creation, the AutoUpdater will automatically register as a FrameListener. 
     * @param updateInterval - Time in seconds between two update calls
     */
    AutoUpdater(Ogre::Real updateInterval = 0.05);
    ~AutoUpdater();

    /** Add an object to be updated automatically. */
    void addUpdatable(Updatable* object);

    /** Remove an object from auto-updates. */
    void removeUpdatable(Updatable* object);

    /** Inherited from FrameListener */
    virtual bool frameStarted(const Ogre::FrameEvent& evt);

  private:
    typedef std::vector<Updatable*> UpdatableList;
    UpdatableList mObjects;
    Ogre::Real mUpdateInterval;
    Ogre::Real mTimeSinceLastUpdate;
  };
}

#endif

