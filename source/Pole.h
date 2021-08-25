#ifndef POLE_H
#define POLE_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"
#include "PowerNet.h"

/*-----------------------------------------------------------------------------------------------*/

class Terrain;

/*-----------------------------------------------------------------------------------------------*/

//! Representing a power net pole in the game
class Pole : public Collidable,
			       public Highlightable,
			       public PowerNetVertex {
public:

	//! Create it
	Pole(Ogre::Vector2 pPosition, eCompanyObjectType pType);

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager, boost::shared_ptr<Terrain> pTerrain);

	//! Remove from scene
	void removeFromSceneManager();

	//! "Upgrade" to intersection
	void changeIntoIntersection(void);

	//! "Downgrade" to simple pole
	void removeIntersection(void);

	//! Add capacity strain
	void addStrain(int pAmount);

	//! Return max capacity
	int getCapacity(void);

	//! Return remaining capacity
	int getCapacityLeft(void);

	//! Get subtype
	eCompanyObjectType getSubtype(void) { return mType; }

	//! Set subtype
	void setType(eCompanyObjectType pType);

	//! Set the pole rotation
	void setRotation(Ogre::Radian pRotation);


	// Implement PowerNetVertex interface
	void reset(void);
	std::vector<Ogre::Vector3> getLineAttachingPoints(Ogre::Vector2 pAdjacentVertexPosition,
											bool pCheckOnly = false);
	size_t getAttachedCount(void);


	// Implement Collidable interface 
	bool isAutoremove(void);
	std::vector<RotatedRect> getGroundRects(void);
	Ogre::MovableObject* getMainEntityObject(void) { return mEntity; }

	// Implement GameObject interface
	Ogre::Vector2 getPosition(void);
	bool isSelectable(void);
	eGameObjectType getType(void);

	// Implement Highlightable
	void highlight(bool pHighlight);

private:

  //! Change into intersection
	void showIntersection(bool pShow);

	Ogre::Vector2 mPosition; //!< Position (center)
	eCompanyObjectType mType; //!< Subtype
	Ogre::SceneNode* mNode; //!< OGRE node
	Ogre::Entity* mEntity; //!< OGRE entity
	Ogre::SceneManager* mSceneManager; //!< OGRE scenemanager
	int mCapacity; //!< Max capacity
	int mCurrentStrain; //!< Current power
	Ogre::Radian mCurrentRotation; //!< Current rotation
	bool mIntersection; //!< Is it an intersection
	int mAttachingCounter; //!< 2-16 lines
	bool mShowIntersection; //!< is it an intersection
	float mHeight; //!< height of the pole
	bool mDestroyed; //!< still in game?
};

/*-----------------------------------------------------------------------------------------------*/

#endif // POLE_H