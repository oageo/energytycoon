#ifndef RIVER_H
#define RIVER_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"

/*-----------------------------------------------------------------------------------------------*/

class Terrain;

/*-----------------------------------------------------------------------------------------------*/

//! Representing a river in the game
//! Rivers are static objects which can never be changed and are generated
//! when loading the map
class River : public Collidable {
public:

	//! Create the river
	River(std::vector< Ogre::Vector2 > pPoints, bool pOceanEnd = false, bool pSourceStart = false);

	//! Generates a bezier curve along the given points
	void generateAndAddToSceneManager(Ogre::SceneManager* pSceneManager,
		boost::shared_ptr< Terrain > pTerrain);

	//! Return a list of lines for bridge generation
	std::vector< Ogre::Vector2 > getLineList(void) { return mLineList; }

	// Implement Collidable interface
	bool isAutoremove(void);
	std::vector< RotatedRect > getGroundRects(void);
	Ogre::MovableObject* getMainEntityObject(void) { return 0; }	// no label

	// Implement GameObject interface
	Ogre::Vector2 getPosition(void);
	bool isSelectable(void);
	eGameObjectType getType(void);
	eCompanyObjectType getSubtype(void) { return eNull; }

private:

  // not used anymore
	RotatedRect alignPoints(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3, Ogre::Vector2 p4);


	std::vector< Ogre::Vector2 > mPoints; //!< The points given by the user
	std::vector< Ogre::Vector2 > mLineList; //!< Bezier curve lines
	std::vector< RotatedRect > mCollidableRects; //!< For Collidable interface
	bool mSourceStart; //!< Is the source of the river on the map
	bool mOceanEnd; //!< Does it end in the ocean
};

/*-----------------------------------------------------------------------------------------------*/

#endif // RIVER_H