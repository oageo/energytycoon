#ifndef __ET_UTILITY_H__
#define __ET_UTILITY_H__
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
#include <OgreDataStream.h>
#include <OgrePixelFormat.h>
#include <string>
#include <iosfwd>
#include <cassert>
#include <cmath>

namespace ET
{
  class FlatTerrain;


  /** Linearly interpolate between two values */
  template <typename T>
  T interpolate(const T& v1, const T& v2, Ogre::Real f)
  {
    assert(f >= 0 && f <= 1 && "Interpolation factor out of range");

    return T(v1*(1-f) + v2*f);
  }

  /** Bilinearly interpolate between four values */
  template <typename T>
  T interpolate2D(const T& v1, const T& v2, const T& v3, const T& v4, Ogre::Real fx, Ogre::Real fy)
  {
    T tmp1 = interpolate(v1, v2, fx);
    T tmp2 = interpolate(v3, v4, fx);
    return interpolate(tmp1, tmp2, fy);
  }

  /** Bilinearly interpolate from a 2D array */
  template <typename T>
  T interpolate2D(const Array2D<T>& array, Ogre::Real x, Ogre::Real y)
  {
    assert (x >= 0 && y >= 0 && x <= array.sizeX()-1 && y <= array.sizeY()-1);
    Ogre::Real fx = std::floor(x), cx = std::ceil(x);
    Ogre::Real fy = std::floor(y), cy = std::ceil(y);
    Ogre::Real dx = cx-fx, dy = cy-fy;
    return interpolate2D(array.at(size_t(fx), size_t(fy)), array.at(size_t(cx), size_t(fy)),
        array.at(size_t(fx), size_t(cy)), array.at(size_t(cx), size_t(cy)), dx, dy);
  }


  /** Create a 2D heights array from an Image */
  RealArray2D loadRealArrayFromImage(const Ogre::Image& image);

  /**
   * Create a 2D heights array from raw data provided as a string.
   * @param rawData String containing the raw byte data
   * @param width Width of the heightmap in the raw data block
   * @param height Height of the heightmap in the raw data block
   * @param endianess Layout of the data in regards to endianess
   */
  RealArray2D loadRealArrayFromRawData(const std::string& rawData, size_t width, size_t height,
    bool endianess = ENDIAN_NATIVE);

  /**
   * Create a 2D heights array from raw data provided as a DataStream.
   * @param rawData DataStream containing the raw byte data
   * @param width Width of the heightmap in the raw data block
   * @param height Height of the heightmap in the raw data block
   * @param endianess Layout of the data in regards to endianess
   */
  RealArray2D loadRealArrayFromRawData(Ogre::DataStreamPtr rawData, size_t width, size_t height,
    bool endianess = ENDIAN_NATIVE);

  /**
   * Create a 2D heights array from raw data provided as a std::istream.
   * @param rawData istream containing the raw byte data
   * @param width Width of the heightmap in the raw data block
   * @param height Height of the heightmap in the raw data block
   * @param endianess Layout of the data in regards to endianess
   */
  RealArray2D loadRealArrayFromRawData(std::istream& rawData, size_t width, size_t height,
    bool endianess = ENDIAN_NATIVE);


  /** Save a 2D heights array to an Image with given pixel format */
  Ogre::Image saveRealArrayToImage(const RealArray2D& array, Ogre::PixelFormat format);

  /** Save a 2D heights array to a string in raw format with given byte depth */
  std::string saveRealArrayToRawString(const RealArray2D& array, size_t bytesPerVal, bool endianess = ENDIAN_NATIVE);

  /** Save a 2D heights array to a stream in raw format with given byte depth */
  void saveRealArrayToRawStream(const RealArray2D& array, std::ostream& stream, size_t bytesPerVal, bool endianess = ENDIAN_NATIVE);



  /** Load a 2D colour array from image */
  ColourArray2D loadColourArrayFromImage(const Ogre::Image& image);

  /** Save a 2D colour array to image */
  Ogre::Image saveColourArrayToImage(const ColourArray2D& array, Ogre::PixelFormat format);



}

#endif
