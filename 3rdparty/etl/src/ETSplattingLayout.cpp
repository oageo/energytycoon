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

#include "ETSplattingLayout.h"
#include "ETDynamicTexture.h"
#include "ETBrush.h"
#include <OgreException.h>
#include <OgrePixelFormat.h>
#include <OgreStringConverter.h>
#include <OgreAxisAlignedBox.h>

using namespace Ogre;


namespace ET
{
  namespace
  {
    /** Get the number of channels of the given pixel format */
    unsigned int getNumChans(PixelFormat format)
    {
      switch (format)
      {
      case PF_BYTE_L: case PF_SHORT_L: case PF_FLOAT16_R: case PF_FLOAT32_R: case PF_DEPTH:
        return 1;
      case PF_BYTE_LA: case PF_A4L4: case PF_FLOAT16_GR: case PF_FLOAT32_GR: case PF_SHORT_GR:
        return 2;
      case PF_BYTE_RGB: case PF_BYTE_BGR: case PF_R5G6B5: case PF_B5G6R5: case PF_SHORT_RGB:
      case PF_FLOAT16_RGB: case PF_FLOAT32_RGB:
        return 3;
      case PF_A4R4G4B4: case PF_BYTE_RGBA: case PF_BYTE_BGRA: case PF_SHORT_RGBA: case PF_FLOAT16_RGBA:
      case PF_FLOAT32_RGBA: case PF_X8R8G8B8: case PF_X8B8G8R8:
        return 4;
      default:
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Unsupported pixel format", "ET::SplattingLayout");
      }
    }

    /** Get the number of splatting maps needed to get the given number of channels */
    unsigned int getRequiredMapNum(PixelFormat format, unsigned int numChannels)
    {
      unsigned int chanPerMap = getNumChans(format);
      return (numChannels + chanPerMap-1) / chanPerMap;
    }

    /** Get the colour value to use for the given map to paint the given channel */
    ColourValue getColourValueForMap(PixelFormat format, unsigned int map, unsigned int channel)
    {
      ColourValue col = ColourValue::Black;
      unsigned int chanPerMap = getNumChans(format);
      int chan = int(channel) - chanPerMap*map;
      if (chan < 0 || chan >= (int)chanPerMap)
        return col;

      // fill the right channel in col
      if (chan == 0)
      {
        col.r = 1;
      }
      else if (chan == 1)
      {
        switch (format)
        {
        case PF_BYTE_LA: case PF_A4L4:
          col.a = 1;
        default:
          col.g = 1;
        }
      }
      else if (chan == 2)
      {
        col.b = 1;
      }
      else
      {
        col.a = 1;
      }
      return col;
    }

    Real getChannel(const ColourValue& col, PixelFormat format, unsigned int channel)
    {
      if (channel == 0)
      {
        return col.r;
      }
      else if (channel == 1)
      {
        switch (format)
        {
          case PF_BYTE_LA: case PF_A4L4:
            return col.a;
          default:
            return col.g;
        }
      }
      else if (channel == 2)
      {
        return col.b;
      }
      else
      {
        return col.a;
      }
    }
  }



  SplattingLayout::SplattingLayout(const String& baseName, const String& group,
    unsigned int width, unsigned int height, PixelFormat format,
    const Vector2& gridScale, unsigned int numChannels)
  : Editable(gridScale, width, height), mBaseName(baseName), mGroup(group), 
    mFormat(format), mNumChannels(numChannels)
  {
    setNumChannels(mNumChannels);
  }

  SplattingLayout::~SplattingLayout()
  {
    for (DynTexList::iterator it = mSplattingMaps.begin(); it != mSplattingMaps.end(); ++it)
      delete *it;
  }


  void SplattingLayout::setNumChannels(unsigned int numChannels)
  {
    bool renormalise = (numChannels < mNumChannels);

    unsigned int numMaps = getRequiredMapNum(mFormat, numChannels);
    // remove any unnecessary splatting maps
    while (mSplattingMaps.size() > numMaps)
    {
      DynamicTexture* tex = mSplattingMaps.back();
      mSplattingMaps.pop_back();
      mMapNames.pop_back();
      delete tex;
    }
    // add new maps, if required
    while (mSplattingMaps.size() < numMaps)
    {
      ColourValue colour = ColourValue::Black;
      if (mSplattingMaps.empty())
      {
        // begin the first texture with a fully lit first channel
        colour = getColourValueForMap(mFormat, 0, 0);
      }
      String newMapName = mBaseName+StringConverter::toString(mSplattingMaps.size());
      DynamicTexture* tex = new DynamicTexture(newMapName,
        mGroup, mGridWidth, mGridHeight, mFormat, mGridScale, colour);
      mSplattingMaps.push_back(tex);
      mMapNames.push_back(newMapName);
    }

    // in case that we removed maps, we need to renormalise the rest
    if (renormalise)
      normalise();
  }

  Real SplattingLayout::getChannelContent(unsigned int x, unsigned int z, unsigned int channel)
  {
    unsigned int map = channel / getNumChans(mFormat);
    DynamicTexture* tex = mSplattingMaps[map];
    const ColourValue& col = tex->getColourValue(x, z);
    return getChannel(col, mFormat, channel);
  }

  void SplattingLayout::normalise()
  {
    // TODO!
  }


  void SplattingLayout::update()
  {
    // update the underlying textures
    for (DynTexList::iterator it = mSplattingMaps.begin(); it != mSplattingMaps.end(); ++it)
      (*it)->update();
  }


  void SplattingLayout::splat(Real x, Real z, const Brush* brush, unsigned int channel, Real intensity)
  {
    determineEditRect(x, z, brush);
    // iterate over all maps and paint with the correct colour value
    for (unsigned int map = 0; map < mSplattingMaps.size(); ++map)
    {
      ColourValue col = getColourValueForMap(mFormat, map, channel);
      mSplattingMaps[map]->paint(x, z, brush, col, intensity);
    }
  }


  void SplattingLayout::splat(const Vector3& pos, const Brush* brush, unsigned int channel, Real intensity)
  {
    Vector3 localPos = transformPositionW2L(pos);
    splat(localPos.x, localPos.z, brush, channel, intensity);
  }


  const String& SplattingLayout::getMovableType() const
  {
    static const String type = "ETSplattingLayout";
    return type;
  }


  const AxisAlignedBox& SplattingLayout::getBoundingBox() const
  {
    static const AxisAlignedBox box (0, 0, 0, 0, 0, 0);
    return box;
  }
}
