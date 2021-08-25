#ifndef COMPANYBUILDING_H
#define COMPANYBUILDING_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"

/*-----------------------------------------------------------------------------------------------*/

class Terrain;
class PowerNet;
class EventData;

/*-----------------------------------------------------------------------------------------------*/

//! Represents a company building (e.g. Research)
class CompanyBuilding : public Collidable,
						            public Highlightable {
public:

	//! Create it
	CompanyBuilding(Ogre::Vector2 pPosition, eCompanyObjectType pType);

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager, boost::shared_ptr<Terrain> pTerrain);
	
	//! Remove from scene
	void removeFromSceneManager();

	//! Get subtype (e.g. "eCOResearch")
	eCompanyObjectType getSubtype(void) { return mType; }


	// Implement Collidable interface
	bool isAutoremove(void);
	std::vector<RotatedRect> getGroundRects(void);
	Ogre::MovableObject* getMainEntityObject(void) { return mEntity; }

	// Implement GameObject interface
	Ogre::Vector2 getPosition(void);
	bool isSelectable(void);
	eGameObjectType getType(void);

	// Implement Highlightable interface
	void highlight(bool pHighlight);

private:

	//! Event handler for day passed event
	void dayPassed(EventData* pData);


	Ogre::Vector2 mPosition; //!< Position (center)
	eCompanyObjectType mType; //!< Subtype
	Ogre::SceneNode* mNode; //!< OGRE node
	Ogre::Entity* mEntity; //!< OGRE entity
	Ogre::Entity* mEntityFloor;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // COMPANYBUILDING_H
