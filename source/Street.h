#ifndef STREET_H
#define STREET_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"

/*-----------------------------------------------------------------------------------------------*/

class City;
class Terrain;

/*-----------------------------------------------------------------------------------------------*/

//! Representing a street in the game
//! Streets are static objects which can never be changed and are generated when 
//! loading the map
class Street : public Collidable {
public:

  //! Create the street
  Street(int pX1, int pY1, int pX2, int pY2);

  //! Generates a Bezier representation of the street
  void generateAndAddToSceneManager(Ogre::SceneManager* pSceneManager,
    boost::shared_ptr< Terrain > pTerrain);

  //! Return a list of lines for bridge generation
  std::vector< Ogre::Vector2 > getLineList(void) { return mLineList; }

  //! Update car positions
  void updateCars(float pElapsedTime);

  //! Load car meshes
  void loadCars(void);

  // Implement Collidable interface
  bool isAutoremove(void);
  std::vector<RotatedRect> getGroundRects(void);
  Ogre::MovableObject* getMainEntityObject(void) { return 0; } // streets don't need labels

  // Implement GameObject interface
  Ogre::Vector2 getPosition(void);
  bool isSelectable(void);
  eGameObjectType getType(void);
  eCompanyObjectType getSubtype(void) { return eNull; }

private:

  // Retrieve points for bezier curve
  void upDown(std::vector< Ogre::Vector2 > &pPoints, int pX1, int pX2, int pY1, int pY2);
  void downUp(std::vector< Ogre::Vector2 > &pPoints, int pX1, int pX2, int pY1, int pY2);
  void leftRight(std::vector< Ogre::Vector2 > &pPoints, int pX1, int pX2, int pY1, int pY2);
  void rightLeft(std::vector< Ogre::Vector2 > &pPoints, int pX1, int pX2, int pY1, int pY2);

  // not used anymore
  RotatedRect alignPoints(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3, Ogre::Vector2 p4);


  int mX1, mY1, mX2, mY2;	//!< Coordinates
  std::vector< RotatedRect > mCollidableRects; //!< For collidable interface implementation
  std::vector< Ogre::Vector2 > mLineList; //!< Line list to find intersection with rivers
  std::vector< Ogre::Vector3 > mCarPath; //!< Car path for this street
  std::vector< Ogre::Vector2 > mBezierPoints; //!< Points to generate bezier curve
  Ogre::SceneNode* mCarNode; //!< OGRE scene node for car
  Ogre::Entity* mCar; //!< Car mesh
  Ogre::MaterialPtr mCarMaterial; //!< Car material
  bool mCarDriving; //!< Car data
  Ogre::SceneManager* mSceneManager; //!< OGRE scene manager
  bool mCarFadingIn; // Some car stuff
  bool mCarFadingOut;
  float mCarFader;
  Ogre::Vector3 mCurrentCarPath;
  float mCurrentLengthDone;
  int mPathIndex;
  float mWait;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // STREET_H