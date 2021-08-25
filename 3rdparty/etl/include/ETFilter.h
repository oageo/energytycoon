#ifndef __ET_FILTER_H__
#define __ET_FILTER_H__
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
#include <OgreMath.h>
#include <vector>
#include <iostream>

namespace ET
{
  /**
   * A simple box filter which you can apply to an arbitrary 2D array
   * to blur its contents.
   * @param array The array to filter
   * @param kernel The radius of the kernel. 0 means no effect, 
   *   1 means look at one additional pixel to each side (3x3 block in total)
   */
  template<class T>
  void boxFilter(ET::Array2D<T>& array, size_t kernel)
  {
    size_t ks = 2*kernel+1;

    // filter in x direction
    for (size_t y = 0; y < array.sizeY(); ++y)
    {
      for (size_t x = kernel; x < array.sizeX()-kernel; ++x)
      {
        T accum = array.at(x, y);
        for (size_t i = 1; i <= kernel; ++i)
        {
          accum += array.at(x-i, y);
          accum += array.at(x+i, y);
        }
        array.at(x,y) = accum / ks;
      }
    }
    // filter in y direction
    for (size_t x = 0; x < array.sizeX(); ++x)
    {
      for (size_t y = kernel; y < array.sizeY()-kernel; ++y)
      {
        T accum = array.at(x, y);
        for (size_t i = 1; i <= kernel; ++i)
        {
          accum += array.at(x, y-i);
          accum += array.at(x, y+i);
        }
        array.at(x,y) = accum / ks;
      }
    }
  }


  /**
   * A gaussian blur filter which you can apply to an arbitrary 2D array
   * to blur its contents.
   * @param array The array to filter
   * @param kernel The radius of the kernel. 0 means no effect, 
   *   1 means look at one additional pixel to each side (3x3 block in total)
   * @param sigma Width of the gaussian distribution
   */
  template<class T>
  void gaussFilter(ET::Array2D<T>& array, size_t kernel, Ogre::Real sigma)
  {
    // calculate gaussian kernel
    std::vector<Ogre::Real> gauss (kernel+1);
    Ogre::Real renorm = 0;
    for (size_t i = 0; i <= kernel; ++i)
    {
      gauss[i] = Ogre::Math::Exp(-(i*i/sigma/sigma/2));
      renorm += 2*gauss[i];
    }
    renorm -= gauss[0];
    // renormalise kernel
    for (size_t i = 0; i <= kernel; ++i)
    {
      gauss[i] /= renorm;
      std::cout << "Kernel[" << i << "]: " << gauss[i] << "\n";
    }

    // filter in x direction
    for (size_t y = 0; y < array.sizeY(); ++y)
    {
      for (size_t x = kernel; x < array.sizeX()-kernel; ++x)
      {
        T accum = array.at(x, y) * gauss[0];
        for (size_t i = 1; i <= kernel; ++i)
        {
          accum += array.at(x-i, y) * gauss[i];
          accum += array.at(x+i, y) * gauss[i];
        }
        array.at(x,y) = accum;
      }
    }
    // filter in y direction
    for (size_t x = 0; x < array.sizeX(); ++x)
    {
      for (size_t y = kernel; y < array.sizeY()-kernel; ++y)
      {
        T accum = array.at(x, y) * gauss[0];
        for (size_t i = 1; i <= kernel; ++i)
        {
          accum += array.at(x, y-i) * gauss[i];
          accum += array.at(x, y+i) * gauss[i];
        }
        array.at(x,y) = accum;
      }
    }
  }
}

#endif

