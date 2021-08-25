#ifndef __ET_COMPATIBILITY_H__
#define __ET_COMPATIBILITY_H__
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

#include <OgrePrerequisites.h>
#include <OgreRenderable.h>

namespace Ogre
{
  class Any;
}

namespace ET
{
  /**
   * Ogre Eihort and Ogre Shoggoth possess some small differences which make it
   * difficult to make a lib compatible to both. This file defines some macros
   * and typedefs to ease the job.
   */

#if OGRE_VERSION_MINOR == 4
  // Eihort support
  #define ET_OGRE_ALLOC(type, count) new type [count]
  #define ET_OGRE_FREE(var) delete[] var
  class FakeRenderableVisitor
  {
  public:
    void visit(Ogre::Renderable*, Ogre::ushort, bool, Ogre::Any* = 0) {}
  };
  typedef FakeRenderableVisitor OgreRenderableVisitor;
#else
  #if OGRE_VERSION_MINOR != 6
    #warning Unknown OGRE version. Assuming Shoggoth compatibility.
  #endif
  #define ET_OGRE_ALLOC(type, count) OGRE_ALLOC_T(type, count, MEMCATEGORY_GENERAL)
  #define ET_OGRE_FREE(var) OGRE_FREE(var, MEMCATEGORY_GENERAL)
  typedef Ogre::Renderable::Visitor OgreRenderableVisitor;
#endif
}

#endif
