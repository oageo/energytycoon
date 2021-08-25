#ifndef __ET_COMMON_H__
#define __ET_COMMON_H__
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

#include "ETArray2D.h"
#include <OgreCommon.h>
#include <OgreColourValue.h>
#include <set>
#include <vector>
#include <string>

namespace ET
{
  typedef Array2D<Ogre::Real> RealArray2D;
  typedef Array2D<Ogre::ColourValue> ColourArray2D;
  typedef std::set<unsigned int> VertexList;
  typedef std::vector<std::string> TextureList;

  /** Constant for big endian format */
  const bool ENDIAN_BIG = false;
  /** Constant for little endian format */
  const bool ENDIAN_LITTLE = true;
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
  const bool ENDIAN_NATIVE = ENDIAN_BIG;
#else
  const bool ENDIAN_NATIVE = ENDIAN_LITTLE;
#endif


  /** Flags for additional vertex buffer elements */
  enum
  {
    VO_NORMALS   = 0x01,
    VO_TANGENTS  = 0x02,
    VO_BINORMALS = 0x04,
    VO_TEXCOORD1 = 0x08,
    VO_LODMORPH  = 0x10
  };
  const unsigned int VO_DEFAULT = VO_NORMALS|VO_TEXCOORD1|VO_LODMORPH;

  /**
   * Direction for connecting neighbouring patches and choosing index buffers.
   * North is considered to be in negative Z direction.
   * East is considered to be in positive X direction.
   */
  enum
  {
    DIR_NORTH = 0,
    DIR_EAST  = 1,
    DIR_SOUTH = 2,
    DIR_WEST  = 3
  };

  /** Opposite directions */
  const int OPPOSITE_DIR[4] = { DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST };

  
  struct Rect
  {
    unsigned int x1, y1, x2, y2;
    Rect(unsigned int ax, unsigned int ay, unsigned int bx, unsigned int by)
    : x1(ax), y1(ay), x2(bx), y2(by)
    {
    }
    Rect() {}
  };
}

#endif
