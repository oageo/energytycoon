#ifndef __ET_BRUSH_H__
#define __ET_BRUSH_H__
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
#include "ETCommon.h"
#include <OgreRectangle.h>
#include <OgreVector2.h>


namespace ET
{
  /**
   * A Brush is a rectangular object that can be mapped onto the grid of an
   * editable object to modify a selected area of that object. Brushes can
   * be rotated and resized. This class is an abstract base class, use one
   * of the concrete implementations below, eg. ArrayBrush.
   * Brushes use an internal local space in which the rectangular area from
   * (0, 0) to (1, 1) contains the contents of the brush. Any brush must
   * implement a method which returns a value in [0, 1] for any point in that
   * local rectangle. The local space is then mapped onto the grid space
   * of the object to be edited, which involves stretching the rectangle to
   * the given size, rotating it around the given angle as seen from its mid
   * point, and finally moving it by a given positional offset which marks
   * the edit position on the editable object.
   */
  class Brush
  {
  public:
    virtual ~Brush() {}

    /** Set the extents of the brush in Ogre units. */
    void setSize(const Ogre::Vector2& size) { mWidth = size.x; mHeight = size.y; calcBoundingRect(); }

    /** Set the brush extents in x direction. */
    void setWidth(Ogre::Real width) { mWidth = width; calcBoundingRect(); }
    /** Set the brush extents in z direction. */
    void setHeight(Ogre::Real height) { mHeight = height; calcBoundingRect(); }
    /** Get the brush extents in x direction. */
    Ogre::Real getWidth() const { return mWidth; }
    /** Get the brush extents in z direction. */
    Ogre::Real getHeight() const { return mHeight; }

    /** Rotate the brush around the given angle. */
    void setRotation(Ogre::Radian rotation) { mRotation = rotation; calcBoundingRect(); }
    /** Get the brush's rotation angle. */
    Ogre::Radian getRotation() const { return mRotation; }

    /** 
     * Returns whether a given grid position lies inside the brush rectangle. 
     * If so, the local brush position is given as the second return argument.
     * @param center The center of the brush in grid space
     * @param pos The grid position to test
     */
    std::pair<bool, Ogre::Vector2> getBrushPosition(const Ogre::Vector2& center, 
        const Ogre::Vector2& pos) const;

    /** Get the brush intensity at the given local brush position. */
    virtual Ogre::Real getIntensityAt(const Ogre::Vector2& pos) const = 0;
    
    /** Get a bounding rectangle for the brush */
    const Ogre::Rectangle& getBoundingRect() const { return mBoundingRect; }

  protected:
    Brush(Ogre::Real width, Ogre::Real height, Ogre::Radian rotation);
    /** Transform a point from local brush space to grid space. */
    Ogre::Vector2 transformL2W(const Ogre::Vector2& point) const;
    /** Transform a point from grid space to local brush space. */
    Ogre::Vector2 transformW2L(const Ogre::Vector2& point) const;

  private:
    /** Calculates a bounding rectangle for the brush dependant on the current rotation. */
    void calcBoundingRect();

    Ogre::Real mWidth, mHeight;
    Ogre::Radian mRotation;
    Ogre::Rectangle mBoundingRect;
    Ogre::Real mSin, mCos;
  };



  /**
   * ArrayBrush is a concrete Brush implementation which uses a 2D array of intensity
   * values. The intensity value at a specific brush position is interpolated from
   * the given array.
   */
  class ArrayBrush : public Brush
  {
  public:
    /**
     * Create a Brush from a 2D array.
     * @param array 2D array of intensity values
     * @param size Size of the brush on an editable grid
     * @param rotation Angle of rotation around the brush's normal
     */
    ArrayBrush(const RealArray2D& array, const Ogre::Vector2& size,
        Ogre::Radian rotation = Ogre::Radian(0));

    /** Set the intensity array */
    void setIntensityArray(const RealArray2D& array) { mIntensities = array; }
    /** Get the intensity array */
    RealArray2D& getIntensityArray() { return mIntensities; }
    /** Get the intensity array (const) */
    const RealArray2D& getIntensityArray() const { return mIntensities; }

    size_t getSizeX() const { return mIntensities.sizeX(); }
    size_t getSizeZ() const { return mIntensities.sizeY(); }

    virtual Ogre::Real getIntensityAt(const Ogre::Vector2& pos) const;

  private:
    RealArray2D mIntensities;
  };
}

#endif
