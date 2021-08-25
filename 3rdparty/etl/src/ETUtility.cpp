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

#include "ETUtility.h"
#include "ETTerrainDescription.h"
#include "ETFlatTerrain.h"
#include "ETCompatibility.h"
#include <OgreException.h>
#include <OgrePixelFormat.h>
#include <OgreImage.h>
#include <iostream>

using namespace Ogre;
using namespace std;


namespace ET
{
  using Ogre::uint8;
  using Ogre::uint16;
  using Ogre::uint32;

  namespace
  {
    /** Helper function to flip endianess of an integer value */
    template <typename int_t>
    void flipEndian(int_t& val)
    {
      // reinterpret as a char array
      char* array = reinterpret_cast<char*>(&val);
      // flip
      for (int i = 0; i < (int)sizeof(int_t)/2; ++i)
        std::swap(array[i], array[sizeof(int_t)-i]);
    }

    void flipEndian24(uint32& val)
    {
      // reinterpret as char array
      char* array = reinterpret_cast<char*>(&val);
      if (ENDIAN_NATIVE == ENDIAN_LITTLE)
        std::swap(array[0], array[2]);
      else
        std::swap(array[1], array[3]);
    }


    /** Helper function to construct a height array from byte stream */
    template <typename int_t>
    RealArray2D createHeightsArray(const char* data, size_t width, size_t height, bool endianess)
    {
      RealArray2D array (width, height);
      // get maximal value for this integer type
      int_t maxValue = std::numeric_limits<int_t>::max();

      for (size_t j = 0; j < height; ++j)
      {
        for (size_t i = 0; i < width; ++i)
        {
          // read value from data
          int_t val = 0;
          memcpy(&val, data, sizeof(int_t));
          // flip if necessary
          if (endianess != ENDIAN_NATIVE)
            flipEndian(val);
          // store in height array normalised to [0,1]
          array[i][j] = Real(val) / maxValue;
          // advance pointer
          data += sizeof(int_t);
        }
      }

      return array;
    }

    /** Specialised for 24bit as there's no native int type for that */
    RealArray2D createHeightsArray24(const char* data, size_t width, size_t height, bool endianess)
    {
      RealArray2D array (width, height);
      uint32 maxValue = (1 << 24) - 1;

      for (size_t j = 0; j < height; ++j)
      {
        for (size_t i = 0; i < width; ++i)
        {
          uint32 val = 0;
          memcpy(&val, data, 3);
          if (ENDIAN_NATIVE == ENDIAN_BIG)
          {
            // for big endian, we read to the wrong bytes of the integer, need to shift one down
            val >>= 8;
          }

          if (endianess != ENDIAN_NATIVE)
            flipEndian24(val);
          array[i][j] = Real(val) / maxValue;
          data += 3;
        }
      }

      return array;
    }
  }


  RealArray2D loadRealArrayFromRawData(const string& rawData, size_t width, size_t height, bool endianess)
  {
    size_t size = width*height;
    // determine bytes per value
    size_t bpv = rawData.size() / size;

    if (rawData.size() % size != 0)
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Raw data size does not match the given extents.", __FUNCTION__);

    // call appropriate function based on bytes per value
    switch (bpv)
    {
    case 1:
      return createHeightsArray<uint8>(rawData.data(), width, height, endianess);
    case 2:
      return createHeightsArray<uint16>(rawData.data(), width, height, endianess);
    case 3:
      return createHeightsArray24(rawData.data(), width, height, endianess);
    case 4:
      return createHeightsArray<uint32>(rawData.data(), width, height, endianess);
    default:
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Raw data height values must have a byte depth of 1, 2, 3 or 4", __FUNCTION__);
    }
  }


  RealArray2D loadRealArrayFromRawData(std::istream& rawData, size_t width, size_t height, bool endianess)
  {
    // read in file contents
    String contents (std::istreambuf_iterator<char>(rawData), (std::istreambuf_iterator<char>()));
    return loadRealArrayFromRawData(contents, width, height, endianess);
  }


  RealArray2D loadRealArrayFromRawData(DataStreamPtr rawData, size_t width, size_t height, bool endianess)
  {
    return loadRealArrayFromRawData(rawData->getAsString(), width, height, endianess);
  }


  RealArray2D loadRealArrayFromImage(const Ogre::Image& image)
  {
    size_t width = image.getWidth();
    size_t height = image.getHeight();
    // determine bytes per value
    size_t bpv = PixelUtil::getNumElemBytes(image.getFormat());
    // determine endianess
    bool endianess = (PixelUtil::isNativeEndian(image.getFormat()) ? ENDIAN_NATIVE : ~ENDIAN_NATIVE);

    // call appropriate function based on bytes per value
    switch (bpv)
    {
    case 1:
      return createHeightsArray<uint8>(reinterpret_cast<const char*>(image.getData()), width, height, endianess);
    case 2:
      return createHeightsArray<uint16>(reinterpret_cast<const char*>(image.getData()), width, height, endianess);
    case 3:
      return createHeightsArray24(reinterpret_cast<const char*>(image.getData()), width, height, endianess);
    case 4:
      return createHeightsArray<uint32>(reinterpret_cast<const char*>(image.getData()), width, height, endianess);
    default:
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Image must have 1, 2, 3 or 4 bytes per pixel", __FUNCTION__);
    }
  }



  namespace
  {
    /** Helper function to store a single Real value in a byte stream */
    void storeVal(Real val, uchar* buf, size_t bytesPerVal, bool endianess)
    {
      uint32 maxVal = numeric_limits<uint32>::max() >> 8*(4-bytesPerVal);
      uint32 write = uint32(maxVal * val);
      // adjust endianess if necessary
      if (endianess != ENDIAN_NATIVE)
        flipEndian(write);
      if (endianess == ENDIAN_LITTLE)
        memcpy(buf, reinterpret_cast<char*>(&write), bytesPerVal);
      else
        memcpy(buf, reinterpret_cast<char*>(&write)+(4-bytesPerVal), bytesPerVal);
    }

    /** Helper function to save a RealArray to a byte stream */
    uchar* saveRealArrayToBuffer(const RealArray2D& array, size_t bytesPerVal, bool endianess)
    {
      size_t size = array.size() * bytesPerVal;
      uchar* buf = ET_OGRE_ALLOC(uchar, size);

      uchar* cur = buf;
      for (size_t y = 0; y < array.sizeY(); ++y)
      {
        for (size_t x = 0; x < array.sizeX(); ++x)
        {
          storeVal(array[x][y], cur, bytesPerVal, endianess);
          cur += bytesPerVal;
        }
      }

      return buf;
    }
  }


  string saveRealArrayToRawString(const RealArray2D& array, size_t bytesPerVal, bool endianess)
  {
    if (bytesPerVal < 1 || bytesPerVal > 4)
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "bytesPerVal must be between 1 and 4", __FUNCTION__);

    uchar* buf = saveRealArrayToBuffer(array, bytesPerVal, endianess);
    string str ((char*)buf, array.size() * bytesPerVal);
    ET_OGRE_FREE(buf);
    return str;
  }

  void saveRealArrayToRawStream(const RealArray2D& array, ostream& stream, size_t bytesPerVal, bool endianess)
  {
    stream << saveRealArrayToRawString(array, bytesPerVal, endianess);
  }


  Image saveRealArrayToImage(const RealArray2D& array, PixelFormat format)
  {
    size_t bytesPerVal = PixelUtil::getNumElemBytes(format);
    bool endianess = (PixelUtil::isNativeEndian(format) ? ENDIAN_NATIVE : ~ENDIAN_NATIVE);
    uchar* buf = saveRealArrayToBuffer(array, bytesPerVal, endianess);
    Image image;
    image.loadDynamicImage(buf, array.sizeX(), array.sizeY(), 1, format, true);
    return image;
  }



  Image saveColourArrayToImage(const ColourArray2D& array, PixelFormat format)
  {
    size_t bytesPerVal = PixelUtil::getNumElemBytes(format);
    size_t size = array.size() * bytesPerVal;
    uchar* buf = ET_OGRE_ALLOC(uchar, size);
    uchar* cur = buf;
    // pack colour values
    for (size_t y = 0; y < array.sizeY(); ++y)
    {
      for (size_t x = 0; x < array.sizeX(); ++x)
      {
        PixelUtil::packColour(array[x][y], format, cur);
        cur += bytesPerVal;
      }
    }

    Image image;
    image.loadDynamicImage(buf, array.sizeX(), array.sizeY(), 1, format, true);
    return image;
  }


  ColourArray2D loadColourArrayFromImage(const Image& image)
  {
    const uchar* cur = image.getData();
    PixelFormat format = image.getFormat();
    size_t bytesPerVal = PixelUtil::getNumElemBytes(format);
    ColourArray2D array (image.getWidth(), image.getHeight());

    for (size_t y = 0; y < array.sizeY(); ++y)
    {
      for (size_t x = 0; x < array.sizeX(); ++x)
      {
        PixelUtil::unpackColour(&array[x][y], format, cur);
        cur += bytesPerVal;
      }
    }

    return array;
  }
}
