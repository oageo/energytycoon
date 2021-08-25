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

#include "ETDynamicTexture.h"
#include "ETBrush.h"
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreException.h>
#include <OgreRoot.h>

using namespace Ogre;
using namespace std;


namespace ET
{
  DynamicTexture::DynamicTexture(const String& name, const String& group,
      unsigned int width, unsigned int height, PixelFormat format, const Vector2& pixelScale,
      const ColourValue& def)
  : Editable(pixelScale, width, height), mSize(width*height), mFormat(format),
    mData(width, height, def)
  {
    // create the texture
    mTexture = TextureManager::getSingleton().createManual(name, group, TEX_TYPE_2D,
      width, height, 1, 0, mFormat, TU_DEFAULT, this);

    mDirtyX1 = mDirtyZ1 = 0;
    mDirtyX2 = mGridWidth-1;
    mDirtyZ2 = mGridHeight-1;
    update();
  }


  DynamicTexture::DynamicTexture(const String& name, const String& group,
    const ColourArray2D& data, PixelFormat format, const Vector2& pixelScale)
  : Editable(pixelScale, data.sizeX(), data.sizeY()), mSize(mGridWidth*mGridHeight),
    mFormat(format), mData(data)
  {
    // create the texture
    mTexture = TextureManager::getSingleton().createManual(name, group, TEX_TYPE_2D,
      mGridWidth, mGridHeight, 1, 0, mFormat, TU_DEFAULT, this);

    mDirtyX1 = mDirtyZ1 = 0;
    mDirtyX2 = mGridWidth-1;
    mDirtyZ2 = mGridHeight-1;
    update();
  }


  DynamicTexture::~DynamicTexture()
  {
    // delete the texture
    TextureManager::getSingleton().remove(mTexture->getName());
  }


  void DynamicTexture::loadResource(Resource*)
  {
    // request to (re)load texture. Create internal resources and copy our array
    // contents to the texture
    mTexture->createInternalResources();
    mDirtyX1 = mDirtyZ1 = 0;
    mDirtyX2 = mGridWidth-1;
    mDirtyZ2 = mGridHeight-1;
    update();
  }

  void DynamicTexture::update()
  {
    if (mDirtyX1 > mDirtyX2 || mDirtyZ1 > mDirtyZ2)
      return;

    HardwarePixelBufferSharedPtr buffer = mTexture->getBuffer();

    // prepare a temporary buffer with the contents of the dirty region
    unsigned int dirtyWidth = mDirtyX2 - mDirtyX1 + 1;
    unsigned int dirtyHeight = mDirtyZ2 - mDirtyZ1 + 1;
    unsigned char* buf = new unsigned char [dirtyWidth*dirtyHeight*PixelUtil::getNumElemBytes(mFormat)];
    unsigned char* pData = buf;
    for (unsigned int j = mDirtyZ1; j <= mDirtyZ2; ++j)
    {
      for (unsigned int i = mDirtyX1; i <= mDirtyX2; ++i)
      {
        // write the colour value to the texture
        ColourValue& col = mData.at(i, j);
        PixelUtil::packColour(col, mFormat, pData);
        pData += PixelUtil::getNumElemBytes(mFormat);
      }
    }
    buffer->blitFromMemory(PixelBox(dirtyWidth, dirtyHeight, 1, mFormat, buf),
      Image::Box(mDirtyX1, mDirtyZ1, mDirtyX2+1, mDirtyZ2+1));
    delete[] buf;

    // reset dirty rect
    mDirtyX1 = mGridWidth+1;
    mDirtyZ1 = mGridWidth+1;
    mDirtyX2 = 0;
    mDirtyZ2 = 0;
  }


  void DynamicTexture::setColourValue(unsigned int x, unsigned int z, const ColourValue& colour)
  {
    mData.at(x,z) = colour;
    // update dirty rect
    mDirtyX1 = min(mDirtyX1, x);
    mDirtyZ1 = min(mDirtyZ1, z);
    mDirtyX2 = max(mDirtyX2, x);
    mDirtyZ2 = max(mDirtyZ2, z);
  }


  void DynamicTexture::setColourArray(const ColourArray2D& array)
  {
    if (array.sizeX() != mData.sizeX() || array.sizeY() != mData.sizeY())
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "New colour array must have the same dimensions as the old one", __FUNCTION__);

    mData = array;
    // update whole texture
    mDirtyX1 = mDirtyZ1 = 0;
    mDirtyX2 = mGridWidth-1;
    mDirtyZ2 = mGridHeight-1;
  }


  void DynamicTexture::paint(Real x, Real z, const Brush* brush, const ColourValue& colour, Real intensity)
  {
    Rect editRect = determineEditRect(x, z, brush);
    // do modification
    for (unsigned int iz = editRect.y1; iz <= editRect.y2; ++iz)
    {
      for (unsigned int ix = editRect.x1; ix <= editRect.x2; ++ix)
      {
        // get position of the pixel
        Vector2 pos (mGridScale.x * ix, mGridScale.y * iz);
        pair<bool, Vector2> brushQry = brush->getBrushPosition(Vector2(x,z), pos);
        if (!brushQry.first)
          continue;
        Real pct = intensity * brush->getIntensityAt(brushQry.second);
        mData.at(ix, iz) = pct * colour + (1-pct) * mData.at(ix,iz);
        mData.at(ix, iz).saturate();
      }
    }

    // update dirty rect
    mDirtyX1 = min(mDirtyX1, editRect.x1);
    mDirtyX2 = max(mDirtyX2, editRect.x2);
    mDirtyZ1 = min(mDirtyZ1, editRect.y1);
    mDirtyZ2 = max(mDirtyZ2, editRect.y2);
  }


  void DynamicTexture::paint(const Vector3& pos, const Brush* brush, const ColourValue& colour, Real intensity)
  {
    // get position in local space
    Vector3 localPos = transformPositionW2L(pos);
    paint(localPos.x, localPos.z, brush, colour, intensity);
  }

}
