#ifndef __ET_SPLATTINGMANAGER_H__
#define __ET_SPLATTINGMANAGER_H__
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
#include "ETEditable.h"
#include "ETMovable.h"
#include "ETUpdatable.h"
#include <OgreString.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgrePixelFormat.h>
#include <map>
#include <vector>



namespace ET
{
  class SplattingLayout;
  class Brush;


  class SplattingListener
  {
  public:
    virtual void notifySplattingTextures(const TextureList& textures) = 0;
    virtual void notifySplattingMaps(const TextureList& maps) = 0;
  };


  /**
   * The SplattingManager offers advanced features for splatting terrain. It
   * builds on the SplattingLayout, but instead of relying on a fixed given
   * number of channels, it manages splatting textures dynamically, adding
   * channels as you splat with new textures.
   */
  class SplattingManager : public Editable, public Movable, public Updatable
  {
  public:
    /**
     * Create a SplattingManager.
     * @param baseName Base name for the splatting map textures.
     * @param group Resource group for the splatting map textures.
     * @param texWidth Width of the map textures.
     * @param texHeight Height of the map textures.
     * @param format The pixel format of the map textures.
     * @param gridScale scale of a single map pixel. (used for editing)
     * @param baseTexture The initial splatting texture to use for channel 1.
     */
    SplattingManager(const Ogre::String& baseName, const Ogre::String& group,
      unsigned int texWidth, unsigned int texHeight, Ogre::PixelFormat format,
      const Ogre::Vector2& gridScale, const Ogre::String& baseTexture);
    ~SplattingManager();


    /** Get width of the splatting map textures */
    unsigned int getTexWidth() const;
    /** Get height of the splatting map textures */
    unsigned int getTexHeight() const;
    /** Get pixel format of the splatting map textures */
    Ogre::PixelFormat getPixelFormat() const;
    /** Get the pixel scale */
    const Ogre::Vector2& getPixelScale() const;

    /** Retrieve the underlying SplattingLayout */
    SplattingLayout* getSplattingLayout() const;

    /**
     * Reset the splatting layout to a single texture.
     * Warning! This will erase your layout so far!
     */
    void setBaseTexture(const Ogre::String& texture);

    /**
     * Remove a texture from the splatting manager. This will empty
     * the channel associated with the given texture and renormalise
     * the other channels.
     * @remarks This call will be ignored if there's only one texture
     * active.
     */
    void removeTexture(const Ogre::String& texture);

    /**
     * Splat the given texture onto the maps at the given local coordinates.
     * @param x Local x coordinate.
     * @param z Local z coordinate.
     * @param brush The brush to use for splatting.
     * @param texture The name of the texture to splat.
     * @param intensity Intensity with which to apply the brush.
     */
    void splat(Ogre::Real x, Ogre::Real z, const Brush* brush,
      const Ogre::String& texture, Ogre::Real intensity = 1.0);

    /**
     * Splat the given texture onto the maps at the given world coordinates.
     * @param pos World coordinates at which to splat.
     * @param brush The brush to use for splatting.
     * @param texture The name of the texture to splat.
     * @param intensity Intensity with which to apply the brush.
     */
    void splat(const Ogre::Vector3& pos, const Brush* brush,
      const Ogre::String& texture, Ogre::Real intensity = 1.0);


    /** Update contents of the SplattingLayouts */
    virtual void update();


    void addListener(SplattingListener* listener);
    void removeListener(SplattingListener* listener);

  private:
    void addTexture(const Ogre::String& texture);
    void checkChannelsUsed();

    SplattingLayout* mSplattingLayout;
    std::map<Ogre::String, unsigned int> mTexChanMap;
    TextureList mTextures;
    unsigned int mNumUsedChannels;
    std::vector<SplattingListener*> mListeners;
    std::vector<Rect> mDirtyRects;
  };
}
#endif

