#ifndef CITY_H
#define CITY_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"
#include "PowerNet.h"

/*-----------------------------------------------------------------------------------------------*/

class Terrain;
class TransformatorSet;

/*-----------------------------------------------------------------------------------------------*/

//! A city in the game
class City : public Collidable,
			 public Highlightable,
			 public PowerNetVertex
{
public:

	//! Create and name it
	City(Ogre::Vector2 pPosition, std::string mName, std::string pSize);

	//! Cleanup
	City(void);

	//! Return name
	Ogre::UTFString getName(void);

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager, boost::shared_ptr<Terrain> pTerrain);

	//! Get residents
	size_t getResidentCount(void);

	//! Get maximum consumption
	size_t getConsumption(void);

	//! Get power still needed
	size_t getPowerNeeded(void);

	//! Get power currently supplied by player
	size_t getCurrentSupply(void) { return mCurrentSupply; }

	//! Add power supplied (power-net distribution)
	void addPowerSupplied(int pAmount);

	//! Add powerplant as supplier
	void addSupplier(std::string pSupplier);

	//! Get preformated suppliers string
	std::string getSuppliers(void);

	//! Counter for powerplant naming
	size_t increaseNamedAfter(void) { mNamedAfterCounter++; return mNamedAfterCounter; }

	//! Update the animations of this mesh
	void updateAnimations(float pElapsedTime);

	//! get Inherit price/advertising settings?
	bool getInheritSettings(void) { return mInheritSettings; }

	//! set Inherit price/advertising settings?
	void setInheritSettings(bool pInherit) { mInheritSettings = pInherit; }

	//! set Custom price
	void setPrice(size_t pPrice) { mPrice = pPrice; }

	//! get Custom price
	size_t getPrice(void) { return mPrice; }

	//! set Custom advertising
	void setAdvertising(size_t pAdvertising) { mAdvertising = pAdvertising; }

	//! get Custom advertising
	size_t getAdvertising(void) { return mAdvertising; }

	//! get customers
	size_t getCustomers(void) { return mCustomers; }

	//! set customers
	void setCustomers(int pNewCustomers) { mCustomers = pNewCustomers; }

	//! Get city size
	int getSize(void);

	// Implement PowerNetVertex interface
	void reset(void);
	std::vector<Ogre::Vector3> getLineAttachingPoints(Ogre::Vector2 pAdjacentVertexPosition,
    bool pCheckOnly = false);
	size_t getAttachedCount(void) { return -1; }

	// Implement Collidable interface
	bool isAutoremove(void);
	std::vector<RotatedRect> getGroundRects(void);
	Ogre::MovableObject* getMainEntityObject(void) { return mEntity; }

	// Implement GameObject interface
	Ogre::Vector2 getPosition(void);
	bool isSelectable(void);
	eGameObjectType getType(void);
	eCompanyObjectType getSubtype(void) { return eNull; }

	// Implement Highlightable interface
	void highlight(bool pHighlight);

private:

	//! Week passed event handler
	void weekPassed(EventData* pData);

	Ogre::Vector2 mPosition; //!< Position (center)
	Ogre::UTFString mName; //!< City name
	std::string mSize; //!< City size
	Ogre::Entity* mEntity; //!< OGRE entities
	Ogre::Entity* mEntityFloor;
	Ogre::SceneNode* mNode; //!< OGRE node
	size_t mResidentCount;
	size_t mConsumption; //!< in MW
	size_t mNamedAfterCounter; //!< Counter for powerplant naming
	size_t mCurrentSupply; //!< in MW
	size_t mCustomers;
	size_t mPrice; //!< ct./kWh
	size_t mAdvertising; //!< EUR/Week
	std::vector<std::string> mSuppliers; //!< Supplying powerplants
	boost::shared_ptr<TransformatorSet> mTransSet; //!< Connecting transformators
	bool mInheritSettings; //!< Inherit global price/ads settings
};

/*-----------------------------------------------------------------------------------------------*/

#endif // CITY_H
