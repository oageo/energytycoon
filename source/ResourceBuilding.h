#ifndef RESOURCEBUILDING_H
#define RESOURCEBUILDING_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"
#include "Event.h"

/*-----------------------------------------------------------------------------------------------*/

class Company;
class Terrain;
class PowerNet;

/*-----------------------------------------------------------------------------------------------*/

//! Represents a resource gathering building in the game (gas, coal or uranium)
class ResourceBuilding : public Collidable,
						             public Highlightable,
						             public EventReceiver {
public:

	//! Create building
	ResourceBuilding(Ogre::Vector2 pPosition, eCompanyObjectType pType, int mCounter,
		boost::shared_ptr<Company> pCompany);

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager, boost::shared_ptr<Terrain> pTerrain);

	//! Remove from scene
	void removeFromSceneManager();

	//! Gets building subtype (e.g. "eREUraniumLarge")
	eCompanyObjectType getSubtype(void) { return mType; }

	//! Set state to damaged (no resource output)
	void setDamaged(void);

	//! Get damaged state
	bool getDamaged(void);

	//! Repair building
	void repair(size_t pMonths);

	//! get inherit maintenance settings
	bool getInheritSettings(void) { return mInheritSettings; }

	//! set inherit maintenance settings
	void setInheritSettings(bool pInherit) { mInheritSettings = pInherit; }

	//! get maintainance Settings
	void setMaintenance(size_t pMaintenance) { mMaintenance = pMaintenance; }

	//! set maintainance Settings
	size_t getMaintenance(void) { return mMaintenance; }

	//! Get resource facility name
	std::string getName(void) { return mName; }

	//! Get counter of all similiar resource facilites
	size_t getCounter(void) { return mCounter; }

	//! Return operating costs
	int getOperatingCosts(void);

	//! Return env costs
	int getEnvironmentalCosts(void);

	//! Get maintainance condition
	size_t getCondition(void) { return (size_t)mCondition; }

	//! Set maintainance condition
	void setCondition(size_t pCondition) { mCondition = pCondition; }

	//! Is this building currently under repairs
	bool isBeingRepaired(void) { return (mRepairDays != 0); }

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

	//! restore healthy state
	void repairNow(void);

	//! Event handler for week passed event
	void weekPassed(EventData* pData);

	//! Event handler for day passed event
	void dayPassed(EventData* pData);


	boost::shared_ptr< Company > mCompany; //!< players company
	bool mDamaged; //!< Is it damaged
	Ogre::Vector2 mPosition; //!< Position (center) of the building
	eCompanyObjectType mType; //!< Subtype
	Ogre::SceneNode* mNode; //!< OGRE node
	Ogre::Entity* mEntity; //!< OGRE entity
	Ogre::Entity* mEntityFloor; //!< entity floor (shadow)
	bool mInheritSettings; //!< Inherit maintenance settings
	size_t mMaintenance; //!< unique building maintenance
	float mCondition; //!< Current condition (percent)
	std::string mName; //!< Building name
	bool mDestroyed; //!< Still in scene/game?
	size_t mCounter; //!< Resource facility counter
	size_t mRepairDays; //!< repair days left
};

/*-----------------------------------------------------------------------------------------------*/

#endif // RESOURCEBUILDING_H