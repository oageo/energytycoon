#ifndef POWERPLANT_H
#define POWERPLANT_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"
#include "PowerNet.h"
#include "Event.h"
#include "Weather.h"

/*-----------------------------------------------------------------------------------------------*/

class Company;
class TransformatorSet;
class Terrain;

/*-----------------------------------------------------------------------------------------------*/

//! Represents any powerplant in the game
class Powerplant : public Collidable,
				           public Highlightable,
				           public PowerNetVertex,
                   public EventReceiver {
public:

	//! Create and name it
	Powerplant(Ogre::Vector2 pPosition, eCompanyObjectType pType, std::string pName,
				boost::shared_ptr< Company > pCompany, int pConstructionYear);

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager, boost::shared_ptr<Terrain> pTerrain);

	//! Remove from scene
	void removeFromSceneManager(void);

	//! Get subtype (e.g. "ePPNuclearLarge")
	eCompanyObjectType getSubtype(void) { return mType; }

	//! Get maximum available power
	int getPowerInMW(void);

	//! Get power still available
	int getPowerLeft(void) { return (getPowerInMW() - mPowerUsed); }

	//! Add strain from city
	void addPowerUsed(int pAmount);

	//! Add city as receiver
	void addReceiver(std::string pReceiver);

	//! Return preformated receiver string
	std::string getReceivers(void);

	//! Get powerplant name
	std::string getName(void) { return mName; }

	//! Update the particle systems
	void updateParticleSystems(void);

	//! Reset power (on powernet change)
	void setDefaultPowerAvailable();

	//! Update animations (e.g. wind turbin)
	void updateAnimations(float pElapsedTime);

	//! get Inherit maintenance settings
	bool getInheritSettings(void) { return mInheritSettings; }

	//! set Inherit maintenance settings
	void setInheritSettings(bool pInherit) { mInheritSettings = pInherit; }

	//! set Maintenance values
	void setMaintenance(size_t pMaintenance) { mMaintenance = pMaintenance; }

	//! get Maintenance values
	size_t getMaintenance(void) { return mMaintenance; }

	//! set Damaged state
	void setDamaged(void);

	//! get Damaged state
	bool getDamaged(void);

	//! restore healthy state
	void repair(size_t pMonths);

	//! get Maintenance condition
	size_t getCondition(void) { return (size_t)mCondition; }

	//! set Maintenance condition
	void setCondition(size_t pCondition) { mCondition = pCondition; }

	//! Return construction year
	size_t getConstructionYear(void) { return mConstructionYear; }

	//! Return operating costs (weekly)
	int getOperatingCosts(void);

	//! Return mandatory env costs (weekly)
	int getEnvironmentalCosts(void);

	//! Is this building under repairs
	bool isBeingRepaired(void) { return (mRepairDays != 0); }

	// Implement PowerNetVertex interface
	void reset(void);
	std::vector< Ogre::Vector3 > getLineAttachingPoints(Ogre::Vector2 pAdjacentVertexPosition,
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

	// Implement Highlightable interface
	void highlight(bool pHighlight);

private:

	//! restore healthy state
	void repairNow(void);

	//! Event handler for week passed event
	void weekPassed(EventData* pData);

	//! Event handler for day passed event
	void dayPassed(EventData* pData);

	//! Event handler for resource event
	void resourceGone(EventData* pData);

	//! Event handler for resource event
	void resourceGain(EventData* pData);

	//! New weather event handler
	void newWeather(EventData* pData);


	bool mDamaged;
	Ogre::Vector2 mPosition; //!< Position (center) of PP
	eCompanyObjectType mType; //!< Subtype
	Ogre::Entity* mEntity; //!< OGRE entity
	Ogre::Entity* mEntityFloor; //!< OGRE entity
	Ogre::Entity* mEntityRotor;
	Ogre::Entity* mEntityRotor2;
	Ogre::Entity* mEntityRotor3;
	Ogre::SceneNode* mNode; //!< OGRE node
	Ogre::SceneNode* mRotorNode;
	Ogre::SceneNode* mRotorNode2;
	Ogre::SceneNode* mRotorNode3;
	std::string mName; //!< Powerplant name
	size_t mPowerAvailable;
	size_t mPowerUsed;
	std::vector< std::string > mReceivers;
	Ogre::ParticleSystem* mPartSys; //!< OGRE particle systems
	Ogre::ParticleSystem* mPartSys2;
	Ogre::ParticleSystem* mPartSys3;
	Ogre::ParticleSystem* mPartSys4;
	bool mWasFullCapacity; //!< Was the powerplant at full capacity before redistribution
	bool mDestroyed;
	boost::shared_ptr< TransformatorSet > mTransSet;
	bool mInheritSettings;
	size_t mMaintenance;
	boost::shared_ptr< Company > mCompany;
	float mCondition;
	size_t mConstructionYear;
	Weather mCurrentWeather;
	size_t mRepairDays;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // POWERPLANT_H