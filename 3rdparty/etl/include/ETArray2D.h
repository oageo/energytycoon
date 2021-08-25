#ifndef __ET_ARRAY2D_H__
#define __ET_ARRAY2D_H__
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

#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>

namespace ET
{
  /**
   * Array2D represents a fixed size two dimensional array for arbitrary data types.
   * Values can be accessed and modified via array[x][y], as you would expect.
   */
  template<typename T>
  class Array2D
  {
  public:
    typedef std::vector<T> Container;
    // STL container definitions
    typedef T value_type;
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef int difference_type;
    typedef size_t size_type;

  private:
    /** Size in x dimension */
    size_t mXlen;
    /** Size in y dimension */
    size_t mYlen;
    /** Number of elements total */
    size_t mNumElems;
    /** The array data */
    Container mData;

  public:
    /** Create an empty Array2D */
    Array2D()
    : mXlen(0), mYlen(0), mNumElems(0)
    {
    }


    /**
     * Create a new Array2D of given dimensions and provided default value.
     * @param xlen Array size in x dimension
     * @param ylen Array size in y dimension
     * @param def  Default value
     */
    Array2D(size_t xlen, size_t ylen, const T& def = T())
    : mXlen(xlen), mYlen(ylen), mNumElems(xlen*ylen),
      mData(mNumElems, def)
    {
    }


    /** Create a copy of another Array2D of same type */
    Array2D(const Array2D<T>& other)
    : mXlen(other.sizeX()), mYlen(other.sizeY()), mNumElems(other.size()),
      mData(other.mData)
    {
    }


    /** Create a copy of another Array2D of different type */
    template<typename R>
    Array2D(const Array2D<R>& other)
    : mXlen(other.sizeX()), mYlen(other.sizeY()), mNumElems(other.size()),
      mData(other.mData)
    {
    }


    /** Retrieve a subpart of the array */
    Array2D<T> getSubArray(size_t startX, size_t startY, size_t width, size_t height)
    {
      assert(startX+width <= mXlen && startY+height <= mYlen);

      Array2D<T> sub (width, height);
      for (size_t y = 0; y < height; ++y)
      {
        for (size_t x = 0; x < width; ++x)
          sub.at(x,y) = at(x+startX, y+startY);
      }
      return sub;
    }



    /** Provides access to the array contents */
    T& at(size_t x, size_t y)
    {
      assert(x < mXlen && y < mYlen && "Array2D index out of bounds");
      return mData[x + y*mXlen];
    }

    /** Provides access to the array (treat contents as 1D array) */
    T& at(size_t i)
    {
      assert(i < mNumElems);
      return mData[i];
    }

    /** Provides const access to the array contents */
    const T& at(size_t x, size_t y) const
    {
      assert(x < mXlen && y < mYlen && "Array2D index out of bounds");
      return mData[x + y*mXlen];
    }

    /** Provides const access to the array (treat as 1D array) */
    const T& at(size_t i) const
    {
      assert(i < mNumElems);
      return mData[i];
    }


    /** Bilinearly interpolate a value at the given decimal position */
    T interpolate(float x, float y) const
    {
      assert(x >= 0 && y >= 0 && x <= mXlen-1 && y <= mYlen-1 && "Position out of range");

      int lx = int(x), ly = int(y);
      float fx = x - lx, fy = y - ly;
      if (lx == mXlen-1)
      {
        --lx;
        fx = 1;
      }
      if (ly == mYlen-1)
      {
        --ly;
        fy = 1;
      }

      float ifx = 1-fx, ify = 1-fy;
      return T(at(lx,ly)*ifx*ify + at(lx+1,ly)*fx*ify + at(lx,ly+1)*ifx*fy + at(lx+1,ly+1)*fx*fy);
    }


    /** Proxy class to support [x][y] access to the array's contents */
    class Array1DProxy
    {
    public:
      Array1DProxy(Array2D<T>& parent, size_t x): mParent(parent), mX(x) { }

      T& operator[](size_t y)
      {
        return mParent.at(mX, y);
      }

      const T& operator[](size_t y) const
      {
        return mParent.at(mX, y);
      }


    private:
      Array2D<T>& mParent;
      size_t mX;
    };

    /** Provides [x][y] access to the array contents */
    Array1DProxy operator[](size_t x)
    {
      return Array1DProxy(*this, x);
    }

    /** Provides [x][y] const access to the array contents */
    const Array1DProxy operator[](size_t x) const
    {
      return Array1DProxy(const_cast<Array2D<T>&>(*this), x);
    }



    /**
     * Provides iterator access to the contents. The elements in the 2D array are stored in
     * contiguous memory and are iterated in the sequence [0][0], [1][0], [2][0], ...
     */
    iterator begin()
    {
      return mData.begin();
    }

    iterator end()
    {
      return mData.end();
    }

    /** Provides const iterator access to the contents. */
    const_iterator begin() const
    {
      return mData.begin();
    }

    const_iterator end() const
    {
      return mData.end();
    }

    bool empty() const
    {
      return mNumElems == 0;
    }

    size_t size() const
    {
      return mNumElems;
    }

    size_t max_size() const
    {
      return mNumElems;
    }

    size_t sizeX() const
    {
      return mXlen;
    }

    size_t sizeY() const
    {
      return mYlen;
    }


    /** Swap contents with another Array2D */
    void swap(Array2D<T>& other) // throws()
    {
      std::swap(other.mData, mData);
      std::swap(other.mXlen, mXlen);
      std::swap(other.mYlen, mYlen);
      std::swap(other.mNumElems, mNumElems);
    }


    /** Copy assignment */
    template<typename R>
    Array2D<T>& operator=(const Array2D<R>& other)
    {
      // exception-safe: use swap idiom
      Array2D<T> temp (other);
      swap(temp);
      return *this;
    }


    /** Copy assignment */
    Array2D<T>& operator=(const Array2D<T>& other)
    {
      // exception-safe: use swap idiom
      Array2D<T> temp (other);
      swap(temp);
      return *this;
    }
  };
}


namespace std
{
  /** Specialised swap for Array2D */
  template<typename T>
  void swap(ET::Array2D<T>& a, ET::Array2D<T>& b)
  {
    a.swap(b);
  }
}

#endif
