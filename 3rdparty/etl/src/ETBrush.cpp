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

#include "ETBrush.h"
#include "ETUtility.h"
#include <OgreMath.h>
#include <OgreStringConverter.h>
#include <cassert>
#include <iostream>

using namespace Ogre;
using namespace std;


namespace ET
{
  Brush::Brush(Real width, Real height, Radian rotation)
  : mWidth(width), mHeight(height), mRotation(rotation)
  {
    calcBoundingRect();
  }

  Vector2 Brush::transformL2W(const Vector2& point) const
  {
    // first, move the origin to the center of the rectangle
    Vector2 tmp (point.x - 0.5, point.y - 0.5);
    // then, multiply with the given size extents
    tmp.x *= mWidth; tmp.y *= mHeight;
    // finally, rotate with given angle
    return Vector2 (tmp.x*mCos - tmp.y*mSin, tmp.x*mSin + tmp.y*mCos);
  }

  Vector2 Brush::transformW2L(const Vector2& point) const
  {
    // rotate vector with -mRotation
    Vector2 res (point.x*mCos + point.y*mSin, -point.x*mSin + point.y*mCos);
    // shrink by size extents
    res.x /= mWidth; res.y /= mHeight;
    // shift by origin
    res.x += 0.5; res.y += 0.5;
    return res;
  }


  void Brush::calcBoundingRect()
  {
    // cache sine and cosine calculations
    mSin = Math::Sin(mRotation);
    mCos = Math::Cos(mRotation);
    
    // transform the four local rectangle corners to grid space
    // and fit them into a bounding rect
    Vector2 c1 = transformL2W(Vector2(0,0));
    Vector2 c2 = transformL2W(Vector2(0,1));
    Vector2 c3 = transformL2W(Vector2(1,0));
    Vector2 c4 = transformL2W(Vector2(1,1));
    mBoundingRect.left = min(c1.x, min(c2.x, min(c3.x, c4.x)));
    mBoundingRect.right = max(c1.x, max(c2.x, max(c3.x, c4.x)));
    mBoundingRect.bottom = max(c1.y, max(c2.y, max(c3.y, c4.y)));
    mBoundingRect.top = min(c1.y, min(c2.y, min(c3.y, c4.y)));    
  }


  pair<bool, Vector2> Brush::getBrushPosition(const Vector2& center, const Vector2& pos) const
  {
    pair<bool, Vector2> res;
    res.second = transformW2L(pos - center);
    res.first = (res.second.x >= 0 && res.second.y >= 0 && res.second.x <= 1 && res.second.y <= 1);
    return res;
  }


  ArrayBrush::ArrayBrush(const RealArray2D& array, const Vector2& size, Radian rotation)
  : Brush(size.x, size.y, rotation), mIntensities(array)
  {
  }

  Real ArrayBrush::getIntensityAt(const Vector2& pos) const
  {
    assert(pos.x >= 0 && pos.y >= 0 && pos.x <= 1 && pos.y <= 1);

    return interpolate2D(mIntensities, pos.x * (mIntensities.sizeX()-1), 
        pos.y * (mIntensities.sizeY()-1));
  }
}
