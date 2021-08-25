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

#include "ETMapGeneration.h"
#include "ETFlatTerrain.h"
#include "ETUtility.h"

using namespace Ogre;
using namespace std;

namespace ET
{
  void generateNormalMap(ColourArray2D& array, FlatTerrain* terrain)
  {
    size_t width = array.sizeX(), height = array.sizeY();
    Real stepSizeX = (terrain->getSize().x-0.01) / (width-1);
    Real stepSizeZ = (terrain->getSize().z-0.01) / (height-1);

    // fill normal map with interpolated values
    Real posX, posZ = 0;
    for (size_t z = 0; z < height; ++z)
    {
      posX = 0;
      for (size_t x = 0; x < width; ++x)
      {
        Vector3 interpNormal = terrain->interpolateNormalAt(posX, posZ);
        // need to wrap values to [0, 1] for use in colour values
        (interpNormal += 1) /= 2;
        array[x][z] = ColourValue(interpNormal.x, interpNormal.y, interpNormal.z);
        posX += stepSizeX;
      }
      posZ += stepSizeZ;
    }
  }


  void generateLightMap(ColourArray2D& array, FlatTerrain* terrain, Vector3 lightDirection,
    const ColourValue& lightColour, const ColourValue& ambient)
  {
    size_t width = array.sizeX(), height = array.sizeY();
    lightDirection.normalise();
    lightDirection = -lightDirection;
    Real stepSizeX = (terrain->getSize().x-0.01) / (width-1);
    Real stepSizeZ = (terrain->getSize().z-0.01) / (height-1);

    Real posX, posZ = 0;
    for (size_t z = 0; z < height; ++z)
    {
      posX = 0;
      for (size_t x = 0; x < width; ++x)
      {
        // interpolate normal at current position and determine dot product with light direction
        Vector3 interpNormal = terrain->interpolateNormalAt(posX, posZ);
        Real amount = max(Real(0), interpNormal.dotProduct(lightDirection));
        array[x][z] = ambient + amount*lightColour;
        posX += stepSizeX;
      }
      posZ += stepSizeZ;
    }
  }



  void generateShadowMap(ColourArray2D& array, FlatTerrain* terrain, Vector3 lightDirection,
      const ColourValue& ambient)
  {
    // algorithm for ray traced shadow map as described here:
    // http://gpwiki.org/index.php/Faster_Ray_Traced_Terrain_Shadow_Maps
    int X[3], Z[3];  // iterators; X[0] - index, X[1] - direction, X[2] - limit
    int *I, *J;
    size_t width = array.sizeX(), height = array.sizeY();

    // based on the direction of light, decide in which order to traverse
    // to speed up calculations
    X[2] = width;
    Z[2] = height;
    if (lightDirection.x < 0)
    {
      X[0] = X[2]-1;
      X[1] = -1;      
    }
    else
    {
      X[0] = 0;
      X[1] = 1;
    }
    if (lightDirection.z < 0)
    {
      Z[0] = Z[2]-1;
      Z[1] = -1;      
    }
    else
    {
      Z[0] = 0;
      Z[1] = 1;
    }
    if (Math::Abs(lightDirection.x) > Math::Abs(lightDirection.z))
    {
      I = Z;
      J = X;
    }
    else
    {
      I = X;
      J = Z;
    }

    // calculate the step size to use
    Vector3 terrainSize = terrain->getSize();
    Vector3 pos = terrain->getPosition();
    Vector3 step (terrainSize.x / width, 0, terrainSize.z / height);

    RealArray2D flagMap (width, height);

    while (1)
    {
      while (1)
      {
        // travel along terrain until we:
        // (1) intersect another point
        // (2) find another point with previous collision data
        // (3) reach the edge of the map
        float px = X[0];
        float pz = Z[0];

        // travel along ray
        while (1)
        {
          px -= lightDirection.x;
          pz -= lightDirection.z;

          // check if we've reached the boundary
          if (px < 0 || px >= width || pz < 0 || pz >= height)
          {
            flagMap[X[0]][Z[0]] = -1.0f;
            break;
          }

          // calculate interpolated values
          Vector3 curPos = pos + Vector3(px*step.x, 0, pz*step.z);
          float ipHeight = terrain->getHeightAt(curPos).y;
          float ipFlag = interpolate2D(flagMap, px, pz);

          // get distance from original point to current point
          float realXDist = (px - X[0]) * step.x;
          float realZDist = (pz - Z[0]) * step.z;
          float distance = sqrt(realXDist*realXDist + realZDist*realZDist);

          // calculate ray height at current point
          float height = terrain->getHeightAt(
            Vector3(pos.x + X[0]*step.x, pos.y, pos.z + Z[0]*step.z)).y
            - pos.y - lightDirection.y*distance;

          // check intersection with either terrain or flagMap
          // if ipHeight < ipFlag check against flagMap value
          float val = (ipHeight < ipFlag ? ipFlag : ipHeight);
          if (height < val)
          {
            // point in shadow
            flagMap[X[0]][Z[0]] = val - height;
            array[X[0]][Z[0]] = ambient;
            break;
          }

          // check if pixel we moved to is unshadowed
          // since the flagMap value is interpolated, we use an epsilon value to check
          // if it's close enough to -1 to indicate non-shadow
          const float epsilon = 0.5f;
          if (ipFlag < -1.0f+epsilon && ipFlag > -1.0f-epsilon)
          {
            flagMap[X[0]][Z[0]] = -1.0f;
            break;
          }
        }

        // update inner loop
        J[0] += J[1];
        if (J[0] >= J[2] || J[0] < 0) 
          break;
      }

      // reset inner loop starting point
      if (J[1] < 0)
        J[0] = J[2]-1;
      else
        J[0] = 0;

      // update outer loop variable
      I[0] += I[1];
      if (I[0] >= I[2] || I[0] < 0)
        break;

    }
  }
}

