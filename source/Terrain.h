#ifndef TERRAIN_H
#define TERRAIN_H

/*-----------------------------------------------------------------------------------------------*/

#include <ETFlatTerrain.h>
#include <ETPage.h>
#include <ETUtility.h>

/*-----------------------------------------------------------------------------------------------*/

const float cTerrainScaleY = 40.0f;	//!< Maximum terrain height
const float cFloorDistance = 0.5f; //!< Default distance objects <-> terrain

/*-----------------------------------------------------------------------------------------------*/

//! Create and manage editable terrain
//! Given a heightmap, material and terrain.cfg this class handles all the terrain
class Terrain {
public:

  //! Create the terrain
  Terrain(std::string pMapPrefix, Ogre::SceneManager* pSceneManager);

  //! Flatten a quad before building a structure
  void flattenQuad(Ogre::Vector2 pCenter, unsigned int pSideLength, float pHeight);

  //! Get terrain weidth
  int getWidth(void) { return mWidth; }

  //! Get terrain height
  int getHeight(void) { return mHeight; }

  //! Get width scale ( terrain width / heightmap size )
  int getScaleX(void) { return mScaleX; }

  //! Get height scale (terrain height / heightmap size )
  int getScaleZ(void) { return mScaleZ; }

  //! Get the precise height
  float getHeightAt(float pX, float pZ);

  //! Get the heightmap height
  float getHeightValueAt(int pX, int pY);

  //! Set the heightmap height and deform terrain
  void setHeightValueAt(int pX, int pY, float pHeight);

  //! Get an ray intersection
  Ogre::Vector3 getIntersectionFromRay(const Ogre::Ray& pRay);

  //! Returns intersection with maximum height from given quad
  Ogre::Vector3 getQuadIntersectionFromRay(const Ogre::Ray& pRay, int pSideLength);

  //! Returns maximum height from given quad at pX, pY
  float getHighestFromQuad(int pX, int pY, int pSideLength, bool pAverage = false);

  //! Update the terrain, has to be called after deforming
  void update(void);

private:

  //! Read width, height and scale from file
  void evaluateParams(void);


  int mWidth;	//!< Width used in game logic
  int mHeight; //!< Height
  int mScaleX; //!< Horizontal scale of heightmap
  int mScaleZ; //!< Vertical scale of heightmap
  std::string mMapPrefix; //!< Short map name (e.g. "de", "us")
  boost::shared_ptr< ET::FlatTerrain > mFlatTerrain; //!< Terrain data
  boost::shared_ptr< ET::Page > mHandle; //!< Terrain page
};

/*-----------------------------------------------------------------------------------------------*/

#endif // TERRAIN_H