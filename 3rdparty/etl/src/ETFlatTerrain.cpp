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

#include "ETFlatTerrain.h"
#include "ETBrush.h"
#include "ETUtility.h"
#include <OgreRay.h>
#include <OgreException.h>
#include <OgreStringConverter.h>
#include <cassert>
#include <iostream>

using namespace Ogre;
using namespace std;


namespace ET
{
  FlatTerrain::FlatTerrain(const RealArray2D& heights, const Vector2& gridScale, Ogre::Real maxHeight)
  : Editable(gridScale, heights.sizeX(), heights.sizeY()), mHeights(heights) 
  {
    mSizeX = heights.sizeX(); mSizeZ = heights.sizeY();
    mNumVertices = mSizeX*mSizeZ;
    mVertexScale.x = gridScale.x;
    mVertexScale.y = maxHeight;
    mVertexScale.z = gridScale.y;
    mSize.x = mVertexScale.x*(mSizeX-1);
    mSize.y = maxHeight;
    mSize.z = mVertexScale.z*(mSizeZ-1);
  }

  FlatTerrain::~FlatTerrain()
  {
    // notify the listeners
    for (std::vector<TerrainListener*>::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
      (*it)->notifyDestroyed();
  }

  void FlatTerrain::setHeightValue(unsigned int x, unsigned int z, Real value)
  {
    mHeights.at(x, z) = value;
    // mark modified vertex
    mDirtyVertices.insert(getVertexIndex(x, z));
  }


  void FlatTerrain::setHeightmap(const RealArray2D& array)
  {
    if (array.sizeX() != mHeights.sizeX() || array.sizeY() != mHeights.sizeY())
      OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "New heightmap must have same dimensions as old one", __FUNCTION__);

    mHeights = array;
    // mark all vertices as dirty
    for (unsigned int i = 0; i < mHeights.size(); ++i)
      mDirtyVertices.insert(i);
  }


  void FlatTerrain::setVertexScale(const Vector2& gridScale, Real maxHeight)
  {
    // update scale and extents
    mVertexScale.x = gridScale.x;
    mVertexScale.y = maxHeight;
    mVertexScale.z = gridScale.y;
    mSize = Vector3(mVertexScale.x*(mSizeX-1), mVertexScale.y, mVertexScale.z*(mSizeZ-1));
    // mark all vertices as dirty
    for (unsigned int i = 0; i < mHeights.size(); ++i)
      mDirtyVertices.insert(i);
  }

  void FlatTerrain::getQuad(Real posX, Real posZ, unsigned int& lx, unsigned int& lz, Real& fx, Real& fz) const
  {
    assert(posX>=0 && posZ>=0 && posX<=mSize.x && posZ<=mSize.z && "Invalid coordinates given");

    posX /= mVertexScale.x; posZ /= mVertexScale.z;
    lx = (unsigned int)posX;
    lz = (unsigned int)posZ;
    fx = posX-lx;
    fz = posZ-lz;
    if (lx == mHeights.sizeX()-1)
    {
      --lx;
      fx = 1;
    }
    if (lz == mHeights.sizeY()-1)
    {
      --lz;
      fz = 1;
    }
  }


  bool FlatTerrain::isOverTerrain(Real x, Real z) const
  {
    return (x>=0 && z >=0 && x <= mSize.x && z<=mSize.z);
  }

  Real FlatTerrain::getHeightAt(Real x, Real z) const
  {
    unsigned int a, b;
    Real fx, fz;
    getQuad(x, z, a, b, fx, fz);

    // get the lower left and upper right vertex position of the triangle
    Vector3 ll (a, mHeights.at(a,b), b);
    Vector3 ur (a+1, mHeights.at(a+1,b+1), b+1);
    // each quad consists of two triangles, determine which one we are in
    // then calculate the exact height at the given position from the triangle
    if (fx + fz <= 1)
    {
      Vector3 base (a+1, mHeights.at(a+1,b), b);
      ll -= base;
      ur -= base;
      return (base + (1-fx)*ll + fz*ur).y * mVertexScale.y;
    }
    else
    {
      Vector3 base (a, mHeights.at(a,b+1), b+1);
      ll -= base;
      ur -= base;
      return (base + fx*ur + (1-fz)*ll).y * mVertexScale.y;
    }
  }


  Vector3 FlatTerrain::getNormalAt(Real x, Real z) const
  {
    // determine the triangle the given coordinates are in
    unsigned int lx, lz;
    Real fx, fz;
    getQuad(x, z, lx, lz, fx, fz);

    Vector3 v1 = getVertexPosition(lx, lz);
    Vector3 v2 = getVertexPosition(lx, lz+1);
    Vector3 v3 = getVertexPosition(lx+1, lz);
    Vector3 v4 = getVertexPosition(lx+1, lz+1);

    if (fx+fz < 1)
    {
      Plane t (v1, v2, v3);
      return t.normal;
    }
    else
    {
      Plane t (v3, v2, v4);
      return t.normal;
    }
  }


  Vector3 FlatTerrain::interpolateNormalAt(Real x, Real z) const
  {
    unsigned int lx, lz;
    Real fx, fz;
    getQuad(x, z, lx, lz, fx, fz);

    Vector3 v1 = getVertexNormal(lx, lz);
    Vector3 v2 = getVertexNormal(lx+1, lz);
    Vector3 v3 = getVertexNormal(lx, lz+1);
    Vector3 v4 = getVertexNormal(lx+1, lz+1);
    return interpolate2D(v1, v2, v3, v4, fx, fz).normalisedCopy();
  }


  Vector3 FlatTerrain::getTangentAt(Real x, Real z) const
  {
    // determine the triangle the given coordinates are in
    unsigned int lx, lz;
    Real fx, fz;
    getQuad(x, z, lx, lz, fx, fz);

    Vector3 v1 = getVertexPosition(lx, lz);
    Vector3 v2 = getVertexPosition(lx, lz+1);
    Vector3 v3 = getVertexPosition(lx+1, lz);
    Vector3 v4 = getVertexPosition(lx+1, lz+1);

    if (fx+fz < 1)
      return (v3-v1).normalisedCopy();
    else
      return (v4-v2).normalisedCopy();
  }


  Vector3 FlatTerrain::getBinormalAt(Real x, Real z) const
  {
    // The binormal results from the cross product of tangent and normal
    return getNormalAt(x, z).crossProduct(getTangentAt(x, z));
  }



  bool FlatTerrain::isOverTerrain(const Vector3& pos) const
  {
    Vector3 localPos = transformPositionW2L(pos);
    return isOverTerrain(localPos.x, localPos.z);
  }

  Vector3 FlatTerrain::getHeightAt(const Vector3& pos) const
  {
    // retrieve the local position
    Vector3 localPos = transformPositionW2L(pos);
    // get the height in local space
    localPos.y = getHeightAt(localPos.x, localPos.z);
    // transform back to world space
    return transformPositionL2W(localPos);
  }

  Vector3 FlatTerrain::getNormalAt(const Vector3& pos) const
  {
    // retrieve local position
    Vector3 localPos = transformPositionW2L(pos);
    // get the normal
    Vector3 normal = getNormalAt(localPos.x, localPos.z);
    // return the rotated normal in world space
    return transformVectorL2W(normal);
  }

  Vector3 FlatTerrain::getTangentAt(const Vector3& pos) const
  {
    // retrieve local position
    Vector3 localPos = transformPositionW2L(pos);
    // get the tangent
    Vector3 tangent = getTangentAt(localPos.x, localPos.z);
    // return the rotated tangent in world space
    return transformVectorL2W(tangent);
  }

  Vector3 FlatTerrain::getBinormalAt(const Vector3& pos) const
  {
    // retrieve local position
    Vector3 localPos = transformPositionW2L(pos);
    // get the binormal
    Vector3 binormal = getBinormalAt(localPos.x, localPos.z);
    // return the rotated normal in world space
    return transformVectorL2W(binormal);
  }



  pair<bool, Vector3> FlatTerrain::rayIntersects(const Ray& ray)
  {
    // first step: convert the ray to vertex space
    Vector3 rayOrigin = transformPositionW2L(ray.getOrigin());
    Vector3 rayDirection = transformVectorW2L(ray.getDirection());
    // scale down to vertex level
    rayOrigin /= mVertexScale;
    rayDirection /= mVertexScale;
    rayDirection.normalise();
    Ray localRay (rayOrigin, rayDirection);
    // test if the ray actually hits the terrain's bounds
    AxisAlignedBox aabb (Vector3(0, 0, 0), Vector3(mSizeX, 1, mSizeZ));
    pair<bool, Real> aabbTest = localRay.intersects(aabb);
    pair<bool, Vector3> result (false, Vector3());
    if (!aabbTest.first)
      return result;
    // get intersection point and move inside
    Vector3 cur = localRay.getPoint(aabbTest.second);

    // now check every quad the ray touches
    int quadX = min(max(static_cast<int>(cur.x), 0), (int)mSizeX-2);
    int quadZ = min(max(static_cast<int>(cur.z), 0), (int)mSizeZ-2);
    int flipX = (rayDirection.x < 0 ? 0 : 1);
    int flipZ = (rayDirection.z < 0 ? 0 : 1);
    int xDir = (rayDirection.x < 0 ? -1 : 1);
    int zDir = (rayDirection.z < 0 ? -1 : 1);
    while (cur.y >= -1 && cur.y <= 2)
    {
      if (quadX < 0 || quadX >= (int)mSizeX-1 || quadZ < 0 || quadZ >= (int)mSizeZ-1)
        break;

      result = checkQuadIntersection(quadX, quadZ, localRay);
      if (result.first)
        break;

      // determine next quad to test
      Real xDist = (quadX - cur.x + flipX) / rayDirection.x;
      Real zDist = (quadZ - cur.z + flipZ) / rayDirection.z;
      if (xDist < zDist)
      {
        quadX += xDir;
        cur += rayDirection * xDist;
      }
      else
      {
        quadZ += zDir;
        cur += rayDirection * zDist;
      }

    }

    result.second = transformPositionL2W(result.second*mVertexScale);
    return result;
  }


  pair<bool, Vector3> FlatTerrain::checkQuadIntersection(int x, int z, const Ray& ray)
  {
    // build the two planes belonging to the quad
    Vector3 v1 (x, mHeights.at(x,z), z);
    Vector3 v2 (x+1, mHeights.at(x+1,z), z);
    Vector3 v3 (x, mHeights.at(x,z+1), z+1);
    Vector3 v4 (x+1, mHeights.at(x+1,z+1), z+1);
    Plane p1 (v1, v3, v2);
    Plane p2 (v3, v4, v2);

    // test for intersection with the two planes. Ensure that the intersection points are actually
    // still inside the triangle (with a small error margin)
    pair<bool, Real> planeInt = ray.intersects(p1);
    if (planeInt.first)
    {
      Vector3 where = ray.getPoint(planeInt.second);
      Vector3 rel = where - v1;
      if (rel.x >= -0.01 && rel.x <= 1.01 && rel.z >= -0.01 && rel.z <= 1.01 && rel.x+rel.z <= 1.01)
        return pair<bool, Vector3>(true, where);
    }
    planeInt = ray.intersects(p2);
    if (planeInt.first)
    {
      Vector3 where = ray.getPoint(planeInt.second);
      Vector3 rel = where - v1;
      if (rel.x >= -0.01 && rel.x <= 1.01 && rel.z >= -0.01 && rel.z <= 1.01 && rel.x+rel.z >= 0.99)
        return pair<bool, Vector3>(true, where);
    }

    return pair<bool, Vector3>(false, Vector3());
  }


  void FlatTerrain::deform(Real x, Real z, const Brush* brush, Real intensity)
  {
    Rect editRect = determineEditRect(x, z, brush);
    // do deformation
    for (unsigned int iz = editRect.y1; iz <= editRect.y2; ++iz)
    {
      for (unsigned int ix = editRect.x1; ix <= editRect.x2; ++ix)
      {
        // get position of the vertex
        Vector2 pos (mVertexScale.x * ix, mVertexScale.z * iz);
        pair<bool, Vector2> brushQry = brush->getBrushPosition(Vector2(x,z), pos);
        if (!brushQry.first)
          continue; // not inside brush rectangle
        Real newHeight = getHeightValue(ix, iz) + intensity * brush->getIntensityAt(brushQry.second);
        newHeight = min(Real(1), max(Real(0), newHeight));
        setHeightValue(ix, iz, newHeight);
      }
    }
  }


  void FlatTerrain::deform(const Vector3& pos, const Brush* brush, Real intensity)
  {
    Vector3 localPos = transformPositionW2L(pos);
    deform(localPos.x, localPos.z, brush, intensity);
  }




  void FlatTerrain::update()
  {
    if (!mDirtyVertices.empty())
    {
      // notify the listeners
      for (std::vector<TerrainListener*>::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
        (*it)->notifyUpdated(mDirtyVertices);
      // reset dirty list
      mDirtyVertices.clear();
    }
  }




  Vector3 FlatTerrain::getVertexPosition(unsigned int x, unsigned int z) const
  {
    Vector3 pos (x, mHeights.at(x, z), z);
    return pos * mVertexScale;
  }

  Vector3 FlatTerrain::getVertexNormal(unsigned int x, unsigned int z) const
  {
    return getNormalAt(x*mVertexScale.x, z*mVertexScale.z);
  }

  Vector3 FlatTerrain::getVertexTangent(unsigned int x, unsigned int z) const
  {
    return getTangentAt(x*mVertexScale.x, z*mVertexScale.z);
  }

  Vector3 FlatTerrain::getVertexBinormal(unsigned int x, unsigned int z) const
  {
    return getBinormalAt(x*mVertexScale.x, z*mVertexScale.z);
  }

  Vector2 FlatTerrain::getVertexTexcoord0(unsigned int x, unsigned int z) const
  {
    Vector2 texCoord (x, z);
    texCoord.x /= (mSizeX-1);
    texCoord.y /= (mSizeZ-1);
    return texCoord;
  }

  Vector2 FlatTerrain::getVertexTexcoord1(unsigned int x, unsigned int z) const
  {
    // for the whole terrain, there is no difference to be made
    // between first and second texcoord set.
    return getVertexTexcoord0(x, z);
  }

  unsigned int FlatTerrain::getTriListSize() const
  {
    // there are 2 triangles per quad, each triangle requires 3 vertices.
    return (mSizeX-1)*(mSizeZ-1)*6;
  }

  unsigned int FlatTerrain::getTriListIndex(unsigned int i) const
  {
    unsigned int quad = i / 6;
    unsigned int inQuad = i % 6;
    unsigned int quadX = quad % (mSizeX-1);
    unsigned int quadZ = quad / (mSizeX-1);

    // decide which of the 4 vertices we need to return
    switch (inQuad)
    {
    case 0:
      return getVertexIndex(quadX, quadZ);
    case 1: case 3:
      return getVertexIndex(quadX, quadZ+1);
    case 2: case 5:
      return getVertexIndex(quadX+1, quadZ);
    case 4:
      return getVertexIndex(quadX+1, quadZ+1);
    default:
      return 0;
    }
  }
}
