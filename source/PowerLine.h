#ifndef POWERLINE_H
#define POWERLINE_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"

/*-----------------------------------------------------------------------------------------------*/

class Pole;
class PowerNetVertex;
class Terrain;
class Map;

/*-----------------------------------------------------------------------------------------------*/

class LineCollidable : public Collidable {
public:

  //! Create it
	LineCollidable(RotatedRect pRect) : mRect(pRect) {}

  //! Return collidable rects
	std::vector<RotatedRect> getGroundRects() 
	{ std::vector<RotatedRect> lTemp; lTemp.push_back(mRect); return lTemp; }

	// rest not relevant here
	bool isAutoremove(void) { return true; }
	Ogre::MovableObject* getMainEntityObject() { return 0; }
	Ogre::Vector2 getPosition(void) { return Ogre::Vector2(-1.0,-1.0); }
	bool isSelectable(void) { return false; }
	eGameObjectType getType(void) { return (eGameObjectType)0; }
	eCompanyObjectType getSubtype(void)  { return (eCompanyObjectType)0; }

private:

	RotatedRect mRect; //!< Rect beneath power lines
};

/*-----------------------------------------------------------------------------------------------*/

//! A power line
class PowerLine : public GameObject {
public:

	//! Create
	PowerLine(boost::shared_ptr< Collidable > pFrom, boost::shared_ptr< Collidable > pTo,
		boost::shared_ptr< Terrain > pTerrain, boost::shared_ptr< Map > pMap, bool pSuggestion = false,
			  Ogre::Vector2 pSuggestionPosition = Ogre::Vector2(0.0,0.0), bool pInvisible = false);
	
  //! Remove (from scene manager)
  ~PowerLine();

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager);

	//! Remove from scene
	void removeFromSceneManager(void);

	//! Change suggestion position (when building)
	void resetSuggestionPosition(Ogre::Vector2 pPosition);

	//! Change to red colour
	void changeNoBuildColour(void);

	//! Get source position
	Ogre::Vector2 getFrom(void);

	//! Get target position
	Ogre::Vector2 getTo(void);

	//! Get suggestion position to check for collisions
	Ogre::Vector2 getSuggestionPosition(void);

	//! Adjust or create the lines
	void adjust(boost::shared_ptr< PowerNetVertex > pFrom, boost::shared_ptr< PowerNetVertex > pTo,
			    bool pFromIntersection = false);


	// Implement GameObject
	Ogre::Vector2 getPosition(void);
	bool isSelectable(void);
	eGameObjectType getType(void);
	eCompanyObjectType getSubtype(void) { return eNull; }

private:

	//! Create the hanging line(s)
	void addHangingLineToManual(Ogre::Vector3 pFrom, Ogre::Vector3 pTo, Ogre::ManualObject* pObject);

	bool mTreesRemoved; //!< if trees already have been removed (speedup)
	bool mInvisible; //!< is the line invisible (i.e. underwater line)
	boost::shared_ptr< Collidable > mFrom, mTo; //!< target and source nodes
	boost::shared_ptr< Terrain > mTerrain; //!< terrain pointer
	boost::shared_ptr< Map > mMap; //!< map pointer
	bool mSuggestion; //!< Is it still a suggestion
	Ogre::Vector2 mSuggestionPosition; //!< Current suggestion position
	Ogre::ManualObject* mManualObject; //!< OGRE manual object
	Ogre::SceneNode* mNode; //!< OGRE node
};

/*-----------------------------------------------------------------------------------------------*/

#endif // POWERLINE_H