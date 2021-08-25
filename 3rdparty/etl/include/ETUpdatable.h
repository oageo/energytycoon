#ifndef __ET_UPDATABLE_H__
#define __ET_UPDATABLE_H__
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

namespace ET
{
  /**
   * Updatable provides an interface for objects which need to be updated 
   * to visually reflect changes to their internal representation.
   */
  class Updatable
  {
  public:
    virtual ~Updatable() {}

    /**
     * To be called after the internal state of an Updatable has changed.
     * The change will then be mapped to the visual representation.
     */
    virtual void update() = 0;
  };
}

#endif

