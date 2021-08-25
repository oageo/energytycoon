#ifndef __ET_SPLATTINGLAYOUT_H__
#define __ET_SPLATTINGLAYOUT_H__
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

#include "ETEditable.h"
#include "ETMovable.h"
#include "ETUpdatable.h"
#include "ETCommon.h"
#include <OgrePixelFormat.h>
#include <OgreVector2.h>
#include <vector>

namespace ET
{
  class DynamicTexture;
  class Brush;

  /**
   * A SplattingLayout manages a couple of DynamicTextures and treats them as splatting maps.
   * You can set the number of channels (i. e. number of different splatting textures) you
   * want to use and modify each channel via brushes.
   * @remarks This is a very basic setup, if you need more advanced features, look at the
   * SplattingManager.
   */
  class SplattingLayout : public Editable, public Movable, public Updatable
  {
  public:
    /**
     * Create a new SplattingLayout.
     * @param baseName Base name for the splatting map textures
     * @param group Resource group for the textures
     * @param texWidth Width of the textures
     * @param texHeight Height of the textures
     * @param format Pixel format of the textures
     * @param gridScale Scale of a pixel in 2D grid space (used for modifications)
     * @param numChannels Number of splatting channels you need
     */
    SplattingLayout(const Ogre::String& baseName, const Ogre::String& group,
      unsigned int texWidth, unsigned int texHeight, Ogre::PixelFormat format,
      const Ogre::Vector2& gridScale, unsigned int numChannels);

    ~SplattingLayout();

    /** Get width */
    unsigned int getTexWidth() const { return mGridWidth; }
    /** Get height */
    unsigned int getTexHeight() const { return mGridHeight; }
    /** Get pixel format */
    Ogre::PixelFormat getPixelFormat() const { return mFormat; }
    /** Get the pixel scale */
    const Ogre::Vector2& getPixelScale() const { return mGridScale; }

    /** Get number of splatting channels */
    unsigned int getNumChannels() const { return mNumChannels; }
    /** Get number of splatting maps */
    unsigned int getNumSplattingMaps() const { return mSplattingMaps.size(); }
    /** Get one of the splatting maps */
    DynamicTexture* getSplattingMap(unsigned int mapIndex) const { return mSplattingMaps[mapIndex]; }
    const TextureList& getSplattingMapNames() const { return mMapNames; }

    /** Reset the number of channels */
    void setNumChannels(unsigned int numChannels);

    /** Retrieve the content of the given channel at coordinates x,z. */
    Ogre::Real getChannelContent(unsigned int x, unsigned int z, unsigned int channel);


    /**
     * Normalise the splatting maps so that all channels add up to 1. You shouldn't normally
     * need to call this.
     */
    void normalise();


    /** Update the texture contents */
    void update();


    /**
     * Splat a channel at the given position in local space.
     * @param x X position in local space
     * @param z Z position in local space
     * @param brush The brush to use
     * @param channel The channel to splat
     * @param intensity The intensity of the splatting
     */
    void splat(Ogre::Real x, Ogre::Real z, const Brush* brush,
      unsigned int channel, Ogre::Real intensity = 1);

    /**
     * Splat a channel at the given position in world space.
     * @param pos Position in world space
     * @param brush The brush to use
     * @param channel The channel to splat
     * @param intensity The intensity of the splatting
     */
    void splat(const Ogre::Vector3& pos, const Brush* brush,
      unsigned int channel, Ogre::Real intensity = 1);

    //**************************
    // MovableObject interface
    //**************************

    const Ogre::String& getMovableType() const;
    void _updateRenderQueue(Ogre::RenderQueue* queue) { }
    const Ogre::AxisAlignedBox& getBoundingBox() const;
    Ogre::Real getBoundingRadius() const { return 0; }


  private:
    Ogre::String mBaseName, mGroup;
    Ogre::PixelFormat mFormat;
    unsigned int mNumChannels;

    typedef std::vector<DynamicTexture*> DynTexList;
    DynTexList mSplattingMaps;
    TextureList mMapNames;
  };
}

#endif

