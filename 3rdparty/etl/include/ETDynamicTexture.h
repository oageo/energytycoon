#ifndef __ET_DYNAMICTEXTURE_H__
#define __ET_DYNAMICTEXTURE_H__
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

#include "ETCommon.h"
#include "ETEditable.h"
#include "ETUpdatable.h"
#include "ETMovable.h"
#include <OgreResource.h>
#include <OgreTexture.h>
#include <OgreVector2.h>
#include <OgreFrameListener.h>


namespace ET
{
  class Brush;


  /**
   * This class manages an in-memory texture and allows modifications to it. Just like
   * Terrain it can be attached to a scene node so that world positions can be used
   * directly for the modifications.
   */
  class DynamicTexture : public Ogre::ManualResourceLoader, public Editable, public Movable, public Updatable
  {
  public:
    /**
     * Create a new dynamic texture.
     * @param name Texture name
     * @param group Resource group name
     * @param width Width of the texture
     * @param height Height of the texture
     * @param format The pixel format of the texture
     * @param pixelScale The scale of a pixel in 2D space (used for modifications)
     * @param def Initial colour of the texture
     */
    DynamicTexture(const Ogre::String& name, const Ogre::String& group, unsigned int width,
      unsigned int height, Ogre::PixelFormat format, const Ogre::Vector2& pixelScale,
      const Ogre::ColourValue& def = Ogre::ColourValue::White);

    /**
     * Create a new dynamic texture from a ColourArray2D
     * @param name Texture name
     * @param group Resource group name
     * @param data The array from which to create the texture
     * @param format The pixel format of the texture
     * @param pixelScale The scale of a pixel in 2D space (used for modifications)
     */
    DynamicTexture(const Ogre::String& name, const Ogre::String& group, const ColourArray2D& data,
      Ogre::PixelFormat format, const Ogre::Vector2& pixelScale);

    ~DynamicTexture();


    /** derived from ManualResourceLoader */
    void loadResource(Ogre::Resource*);

    /**
     * Update the texture contents. This function must be called to reflect
     * changes you made to the visual representation.
     */
    void update();

    /** Get the colour value at the specified position */
    const Ogre::ColourValue& getColourValue(unsigned int x, unsigned int z) const { return mData.at(x,z); }

    /** Set the colour value at the specified position */
    void setColourValue(unsigned int x, unsigned int z, const Ogre::ColourValue& colour);

    /** Get the 2D array of colour values */
    const ColourArray2D& getColourArray() const { return mData; }

    /** Set the 2D array of colour values */
    void setColourArray(const ColourArray2D& array);


    /**
     * Use a brush to paint on the texture at the given local position.
     * @param x Local position, x coordinate
     * @param z Local position, z coordinate
     * @param brush Brush to use for painting
     * @param colour The colour value to paint on the texture
     * @param intensity The intensity of the painting
     */
    void paint(Ogre::Real x, Ogre::Real z, const Brush* brush, const Ogre::ColourValue& colour,
      Ogre::Real intensity = 1);

    /**
     * Use a brush to paint on the texture at the given world position.
     * @param pos World position
     * @param brush Brush to use for painting
     * @param colour The colour value to paint on the texture
     * @param intensity The intensity of the painting
     */
    void paint(const Ogre::Vector3& pos, const Brush* brush, const Ogre::ColourValue& colour,
      Ogre::Real intensity = 1);


  private:
    unsigned int mSize;
    Ogre::PixelFormat mFormat;
    ColourArray2D mData;
    Ogre::TexturePtr mTexture;
    /** Dirty rect for content updates */
    unsigned int mDirtyX1, mDirtyZ1, mDirtyX2, mDirtyZ2;
  };
}

#endif
